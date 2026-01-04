// server.cpp - Auction Server (modular + persistence + logs + draft edit/remove)
// Build: g++ -std=c++17 -O2 server.cpp common.cpp storage.cpp -o server
// Run  : ./server 8080

#include "common.hpp"
#include "model.hpp"
#include "storage.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <csignal>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

struct Session {
    int fd = -1;
    bool authed = false;
    int user_id = 0;
    string inbuf;
    string outbuf;
};

class Server {
public:
    explicit Server(int port)
        : port_(port) {}

    bool run() {
        open_logs();

        // load state if exists
        {
            PersistState st;
            string e;
            if (load_state_file(state_path_, st, e)) {
                users_ = std::move(st.users);
                user_by_name_ = std::move(st.user_by_name);
                rooms_ = std::move(st.rooms);
                items_ = std::move(st.items);
                history_ = std::move(st.history);
                drafts_ = std::move(st.drafts);
                next_user_id_ = st.next_user_id;
                next_room_id_ = st.next_room_id;
                next_item_id_ = st.next_item_id;
                log_server("[BOOT] loaded state: users=" + to_string(users_.size()) +
                           " rooms=" + to_string(rooms_.size()) +
                           " items=" + to_string(items_.size()));
            } else {
                seed_users();
                log_server("[BOOT] no state loaded (" + e + "), seeded default users");
            }
        }

        if (!setup_listen()) return false;

        g_instance = this;
        signal(SIGINT, &Server::on_sigint);

        cout << "[SERVER] Listening on port " << port_ << "\n";

        long long last_tick = now_ms();
        while (!shutdown_requested_) {
            fd_set rfds, wfds;
            FD_ZERO(&rfds);
            FD_ZERO(&wfds);

            int maxfd = listen_fd_;
            FD_SET(listen_fd_, &rfds);

            for (auto& [fd, s] : sessions_) {
                FD_SET(fd, &rfds);
                if (!s.outbuf.empty()) FD_SET(fd, &wfds);
                maxfd = max(maxfd, fd);
            }

            timeval tv{};
            tv.tv_sec = 0;
            tv.tv_usec = 200 * 1000;

            int n = select(maxfd + 1, &rfds, &wfds, nullptr, &tv);
            if (n < 0) {
                continue;
            }

            if (FD_ISSET(listen_fd_, &rfds)) accept_new();

            vector<int> to_close;
            for (auto& [fd, s] : sessions_) {
                if (FD_ISSET(fd, &wfds)) {
                    if (!flush_out(s)) to_close.push_back(fd);
                }
            }
            for (auto& [fd, s] : sessions_) {
                if (FD_ISSET(fd, &rfds)) {
                    if (!recv_in(s)) to_close.push_back(fd);
                }
            }

            sort(to_close.begin(), to_close.end());
            to_close.erase(unique(to_close.begin(), to_close.end()), to_close.end());
            for (int fd : to_close) close_session(fd);

            long long now = now_ms();
            if (now - last_tick >= 200) {
                last_tick = now;
                auction_tick(now);
                flush_rooms_changed(now);
            }
        }

        save_state();
        log_server("[SHUTDOWN] saved state & exit");
        cout << "\n[SERVER] Shutdown complete.\n";
        return true;
    }

private:
    // networking
    int port_ = 0;
    int listen_fd_ = -1;
    unordered_map<int, Session> sessions_;
    unordered_map<int, int> fd_by_user_;

    // data
    unordered_map<int, User> users_;
    unordered_map<string, int> user_by_name_;
    unordered_map<int, Room> rooms_;
    unordered_map<int, Item> items_;
    vector<HistoryRow> history_;
    unordered_map<DraftKey, vector<DraftItem>, DraftKeyHash, DraftKeyEq> drafts_;

    int next_user_id_ = 1;
    int next_room_id_ = 1;
    int next_item_id_ = 1;

    // persistence/logs
    const string state_path_ = "auction_state.txt";
    ofstream server_log_;
    ofstream history_log_;
    ofstream chat_log_;
    bool shutdown_requested_ = false;

    bool rooms_changed_pending_ = false;
    long long rooms_changed_last_ms_ = 0;


    static Server* g_instance;
    static void on_sigint(int) {
        if (g_instance) g_instance->shutdown_requested_ = true;
    }

private:
    void open_logs() {
        server_log_.open("server.log", ios::out | ios::app);
        history_log_.open("history.log", ios::out | ios::app);
        chat_log_.open("chat.log", ios::out | ios::app);
    }

    void log_server(const string& s) {
        if (server_log_) server_log_ << now_ms() << " " << s << "\n";
    }

    void log_history(const HistoryRow& h) {
        if (!history_log_) return;
        history_log_ << h.end_at_ms
                     << " item=" << h.item_id
                     << " room=" << h.room_id
                     << " seller=" << h.seller_user_id
                     << " winner=" << h.winner_user_id
                     << " price=" << h.final_price
                     << " reason=" << h.reason
                     << "\n";
    }

    void log_chat(long long ts, const string& from, const string& to, int room_id, const string& msg) {
        if (!chat_log_) return;
        chat_log_ << ts << " room=" << room_id << " from=" << from << " to=" << to << " msg=" << msg << "\n";
    }


    void save_state() {
        PersistState st;
        st.users = users_;
        st.user_by_name = user_by_name_;
        st.rooms = rooms_;
        st.items = items_;
        st.history = history_;
        st.drafts = drafts_;
        st.next_user_id = next_user_id_;
        st.next_room_id = next_room_id_;
        st.next_item_id = next_item_id_;

        for (auto& [rid, r] : st.rooms) r.participants.clear();
        for (auto& [uid, u] : st.users) u.current_room_id = 0;

        if (!save_state_file(state_path_, st)) {
            log_server("[SAVE] failed to save state file");
        } else {
            log_server("[SAVE] state saved to " + state_path_);
        }
    }

    bool setup_listen() {
        listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd_ < 0) { perror("socket"); return false; }

        int yes = 1;
        setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons((uint16_t)port_);

        if (bind(listen_fd_, (sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("bind");
            return false;
        }
        if (listen(listen_fd_, 64) < 0) {
            perror("listen");
            return false;
        }

        set_nonblock(listen_fd_);
        return true;
    }

    int create_user(const string& u, const string& p, const string& role) {
        if (user_by_name_.count(u)) return user_by_name_[u];
        User x;
        x.id = next_user_id_++;
        x.username = u;
        x.password = p;
        x.role = role;
        users_[x.id] = x;
        user_by_name_[u] = x.id;
        return x.id;
    }

    void seed_users() {
        create_user("seller1", "pass1", "BOTH");
        create_user("buyer1",  "pass1", "BOTH");
        create_user("buyer2",  "pass1", "BOTH");
    }

    void accept_new() {
        while (true) {
            sockaddr_in caddr{};
            socklen_t clen = sizeof(caddr);
            int cfd = accept(listen_fd_, (sockaddr*)&caddr, &clen);
            if (cfd < 0) break;
            set_nonblock(cfd);

            Session s;
            s.fd = cfd;
            sessions_[cfd] = s;

            send_line(cfd, "OK HELLO Welcome to AuctionServer\n");
            send_line(cfd, "OK HINT Try: HELP\n");
            log_server("[NET] accepted fd=" + to_string(cfd));
        }
    }

    bool flush_out(Session& s) {
        if (s.outbuf.empty()) return true;
        ssize_t sent = ::send(s.fd, s.outbuf.data(), s.outbuf.size(), 0);
        if (sent < 0) return false;
        s.outbuf.erase(0, (size_t)sent);
        return true;
    }

    void queue_send(Session& s, const string& line) { s.outbuf += line; }

    void send_line(int fd, const string& line) {
        auto it = sessions_.find(fd);
        if (it == sessions_.end()) return;
        queue_send(it->second, line);
    }

    bool recv_in(Session& s) {
        char buf[4096];
        while (true) {
            ssize_t r = recv(s.fd, buf, sizeof(buf), 0);
            if (r == 0) return false;
            if (r < 0) break;

            s.inbuf.append(buf, buf + r);

            while (true) {
                size_t pos = s.inbuf.find('\n');
                if (pos == string::npos) break;
                string line = s.inbuf.substr(0, pos);
                s.inbuf.erase(0, pos + 1);
                line = trim(line);
                if (!line.empty()) handle_line(s, line);
            }
        }
        return true;
    }

    void close_session(int fd) {
        auto it = sessions_.find(fd);
        if (it == sessions_.end()) return;

        Session s = it->second;

        if (s.authed) {
            int uid = s.user_id;
            auto uit = users_.find(uid);
            if (uit != users_.end()) {
                int rid = uit->second.current_room_id;
                if (rid) leave_room(uid, rid, true, nullptr);
            }
            fd_by_user_.erase(uid);
        }

        close(fd);
        sessions_.erase(fd);
        log_server("[NET] closed fd=" + to_string(fd));
    }

    void reply(Session& s, const string& line) { queue_send(s, line); }
    void err(Session& s, const string& code, const string& msg) {
        queue_send(s, "ERR " + code + " " + msg + "\n");
    }

    void evt_to_user(int uid, const string& line) {
        auto it = fd_by_user_.find(uid);
        if (it == fd_by_user_.end()) return;
        send_line(it->second, line);
    }

    void evt_to_room(int rid, const string& line) {
        auto it = rooms_.find(rid);
        if (it == rooms_.end()) return;
        for (int uid : it->second.participants) evt_to_user(uid, line);
    }


    void evt_to_all_authed(const string& line) {
        for (auto& [fd, sess] : sessions_) {
            if (!sess.authed) continue;
            queue_send(sess, line);
        }
    }

    void mark_rooms_changed() {
        rooms_changed_pending_ = true;
    }

    void flush_rooms_changed(long long now) {
        if (!rooms_changed_pending_) return;
        if (now - rooms_changed_last_ms_ < 150) return;
        rooms_changed_last_ms_ = now;
        rooms_changed_pending_ = false;
        evt_to_all_authed("EVT ROOMS_CHANGED\n");
    }

    string build_current_line(const Item& itx, long long now) {
        long long remaining = itx.end_time_ms - now;
        if (remaining < 0) remaining = 0;
        return "EVT CURRENT " + to_string(itx.id) + " " + itx.name + " " + username_of(itx.seller_user_id) +
               " " + to_string(itx.start_price) + " " + to_string(itx.buy_now_price) + " " +
               to_string(itx.current_price) + " " +
               (itx.leader_user_id ? username_of(itx.leader_user_id) : "none") + " " +
               mmss(remaining) + "\n";
    }

    void broadcast_current(int rid, long long now) {
        auto it = rooms_.find(rid);
        if (it == rooms_.end()) return;
        int iid = it->second.current_item_id;
        if (!iid) return;
        auto itm = items_.find(iid);
        if (itm == items_.end()) return;
        evt_to_room(rid, build_current_line(itm->second, now));
    }

    string build_chat_users_line(int rid) {
        auto it = rooms_.find(rid);
        if (it == rooms_.end()) return "EVT CHAT_USERS 0\n";
        vector<string> names;
        names.reserve(it->second.participants.size());
        for (int uid : it->second.participants) {
            auto uit = users_.find(uid);
            if (uit != users_.end()) names.push_back(uit->second.username);
        }
        sort(names.begin(), names.end());
        string line = "EVT CHAT_USERS " + to_string((int)names.size());
        for (auto& u : names) line += " " + u;
        line += "\n";
        return line;
    }

    // For Rooms tab quick-chat: list participants of a room by id (even if requester not in room)
    // EVT ROOM_USERS <room_id> <n> <u1> ...
    string build_room_users_line(int rid) {
        auto it = rooms_.find(rid);
        if (it == rooms_.end()) return "EVT ROOM_USERS " + to_string(rid) + " 0\n";
        vector<string> names;
        names.reserve(it->second.participants.size());
        for (int uid : it->second.participants) {
            auto uit = users_.find(uid);
            if (uit != users_.end()) names.push_back(uit->second.username);
        }
        sort(names.begin(), names.end());
        string line = "EVT ROOM_USERS " + to_string(rid) + " " + to_string((int)names.size());
        for (auto& u : names) line += " " + u;
        line += "\n";
        return line;
    }

    void broadcast_chat_users(int rid) {
        evt_to_room(rid, build_chat_users_line(rid));
    }


    string username_of(int uid) {
        auto it = users_.find(uid);
        return (it == users_.end()) ? "?" : it->second.username;
    }

    static const char* room_status_str(RoomStatus st) {
        switch (st) {
            case RoomStatus::PENDING: return "PENDING";
            case RoomStatus::STARTED: return "STARTED";
            case RoomStatus::ENDED:   return "ENDED";
        }
        return "PENDING";
    }

    void print_help(Session& s) {
        reply(s, "OK HELP\n");
        reply(s, "EVT HELP AUTH: REGISTER <u> <p> | LOGIN <u> <p> | LOGOUT | QUIT\n");
        reply(s, "EVT HELP ROOM: LIST_ROOMS | LIST_MY_ROOMS | CREATE_ROOM <name> | JOIN_ROOM <id_or_name> | LEAVE_ROOM\n");
        reply(s, "EVT HELP SELLER: ADD_ITEM name|start|buy_now|duration | DRAFT_LIST | EDIT_ITEM d|field|value | REMOVE_ITEM d | CLEAR_DRAFT | OK | START | END\n");
        reply(s, "EVT HELP BUYER: BID <item_id> <amount> | BUY_NOW <item_id>\n");
        reply(s, "EVT HELP VIEW: VIEW RUNNING|QUEUED|ENDED | SEARCH <keyword> | HISTORY ME\n");
        reply(s, "EVT HELP CHAT: CHAT_USERS | CHAT_TO <user>|<msg> | CHAT_ROOM |<msg>\n");
        reply(s, "EVT HELP ADMIN: SAVE (force save)\n");
        reply(s, "EVT HELP_END\n");
    }

    // room membership
    void join_room(int uid, int rid, Session& s) {
        if (!rooms_.count(rid)) return err(s, "ROOM_NOT_FOUND", "No such room");
        if (rooms_[rid].status == RoomStatus::ENDED) return err(s, "ROOM_ENDED", "Room already ended");
        int old = users_[uid].current_room_id;
        if (old && old != rid) leave_room(uid, old, true, nullptr);

        users_[uid].current_room_id = rid;
        rooms_[rid].participants.insert(uid);
        mark_rooms_changed();

        reply(s, "OK ROOM_JOINED " + to_string(rid) + " " + rooms_[rid].name + "\n");
        evt_to_room(rid, "EVT USER_JOINED " + users_[uid].username + "\n");
        broadcast_chat_users(rid);
        send_snapshot(uid, rid);
    }

    void leave_room(int uid, int rid, bool silent_reply, Session* ps) {
        auto it = rooms_.find(rid);
        if (it == rooms_.end()) return;

        it->second.participants.erase(uid);
        mark_rooms_changed();
        users_[uid].current_room_id = 0;

        evt_to_room(rid, "EVT USER_LEFT " + users_[uid].username + "\n");
        broadcast_chat_users(rid);
        if (!silent_reply && ps) reply(*ps, "OK ROOM_LEFT " + to_string(rid) + "\n");
    }

    void send_snapshot(int uid, int rid) {
        auto& r = rooms_[rid];
        string st = room_status_str(r.status);
                string ownerName = username_of(r.owner_user_id);
        evt_to_user(uid, "EVT ROOM_SNAPSHOT " + to_string(r.id) + " " + r.name + " " + st + " " +
                         to_string((int)r.participants.size()) + " " + to_string(r.owner_user_id) + " " + ownerName + "\n");

        if (r.current_item_id) {
            auto& itx = items_[r.current_item_id];
            long long tl = itx.end_time_ms - now_ms();
            evt_to_user(uid, "EVT CURRENT " + to_string(itx.id) + " " + itx.name + " " + username_of(itx.seller_user_id) +
                             " " + to_string(itx.start_price) + " " + to_string(itx.buy_now_price) + " " +
                             to_string(itx.current_price) + " " +
                             (itx.leader_user_id ? username_of(itx.leader_user_id) : "none") + " " +
                             mmss(tl) + "\n");
        } else {
            evt_to_user(uid, "EVT CURRENT NONE\n");
        }

        int topn = (int)min<size_t>(5, r.queue.size());
        evt_to_user(uid, "EVT QUEUE_TOP " + to_string(topn) + "\n");
        for (int i = 0; i < topn; i++) {
            auto& itx = items_[r.queue[i]];
            evt_to_user(uid, "EVT QUEUED " + to_string(itx.id) + " " + itx.name + " " +
                             to_string(itx.start_price) + " " + to_string(itx.buy_now_price) + " " +
                             to_string(itx.duration_sec) + "\n");
        }
        evt_to_user(uid, "EVT SNAPSHOT_END\n");
    }

    // auction engine
    void start_next_in_room(int rid, long long now) {
        auto& r = rooms_[rid];
        if (r.current_item_id != 0) return;

        // Only progress auction when room is STARTED
        if (r.status != RoomStatus::STARTED) return;

        if (r.queue.empty()) {
            // No more items -> go back to PENDING (owner may publish more or END)
            r.status = RoomStatus::PENDING;
            evt_to_room(rid, "EVT ROOM_PENDING " + to_string(rid) + "\n");
            return;
        }

        int iid = r.queue.front();
        r.queue.pop_front();

        auto& itx = items_[iid];
        itx.state = ItemState::RUNNING;
        itx.current_price = itx.start_price;
        itx.leader_user_id = 0;
        itx.start_time_ms = now;
        itx.end_time_ms = now + (long long)itx.duration_sec * 1000LL;
        itx.alert_30_sent = false;

        r.current_item_id = iid;
        r.status = RoomStatus::STARTED;

        evt_to_room(rid, "EVT ITEM_START " + to_string(itx.id) + " " + itx.name + " " + username_of(itx.seller_user_id) +
                         " " + to_string(itx.start_price) + " " + to_string(itx.buy_now_price) + " " +
                         to_string(itx.current_price) + " none " + mmss(itx.end_time_ms - now_ms()) + "\n");
        broadcast_current(rid, now_ms());
    }

    void finalize_item(int rid, int iid, int winner_uid, int final_price, const string& reason) {
        auto& r = rooms_[rid];
        auto& itx = items_[iid];

        itx.state = (winner_uid ? ItemState::SOLD : ItemState::EXPIRED);
        itx.winner_user_id = winner_uid;
        itx.final_price = final_price;
        itx.reason = reason;
        itx.end_at_ms = now_ms();

        HistoryRow h;
        h.item_id = iid;
        h.room_id = rid;
        h.seller_user_id = itx.seller_user_id;
        h.winner_user_id = winner_uid;
        h.final_price = final_price;
        h.reason = reason;
        h.end_at_ms = itx.end_at_ms;
        history_.push_back(h);
        log_history(h);

        if (winner_uid) {
            evt_to_room(rid, "EVT SOLD " + to_string(iid) + " " + username_of(winner_uid) + " " +
                             to_string(final_price) + " " + reason + "\n");
            evt_to_room(rid, "EVT ITEM_END " + to_string(iid) + " SOLD " + reason + "\n");
        } else {
            evt_to_room(rid, "EVT EXPIRED " + to_string(iid) + "\n");
            evt_to_room(rid, "EVT ITEM_END " + to_string(iid) + " EXPIRED " + reason + "\n");
        }

        r.current_item_id = 0;
        if (r.status != RoomStatus::ENDED && r.queue.empty()) {
            r.status = RoomStatus::PENDING;
            evt_to_room(rid, "EVT ROOM_PENDING " + to_string(rid) + "\n");
        }
    }

    void end_room(int rid, int by_uid) {
        if (!rooms_.count(rid)) return;
        auto& r = rooms_[rid];
        if (r.status == RoomStatus::ENDED) return;

        // Mark room ended first so finalize_item won't revert it to PENDING
        r.status = RoomStatus::ENDED;

        // Finalize current item (if any)
        if (r.current_item_id != 0 && items_.count(r.current_item_id)) {
            auto& itx = items_[r.current_item_id];
            if (itx.state == ItemState::RUNNING) {
                if (itx.leader_user_id != 0) {
                    finalize_item(rid, itx.id, itx.leader_user_id, itx.current_price, "ENDED_BY_SELLER");
                } else {
                    finalize_item(rid, itx.id, 0, 0, "ENDED_BY_SELLER");
                }
            } else {
                r.current_item_id = 0;
            }
        } else {
            r.current_item_id = 0;
        }

        // Expire all queued items
        while (!r.queue.empty()) {
            int iid = r.queue.front();
            r.queue.pop_front();

            if (!items_.count(iid)) continue;
            auto& itx = items_[iid];
            if (itx.state == ItemState::QUEUED) {
                itx.state = ItemState::EXPIRED;
                itx.winner_user_id = 0;
                itx.final_price = 0;
                itx.reason = "ROOM_ENDED";
                itx.end_at_ms = now_ms();

                HistoryRow h;
                h.item_id = iid;
                h.room_id = rid;
                h.seller_user_id = itx.seller_user_id;
                h.winner_user_id = 0;
                h.final_price = 0;
                h.reason = itx.reason;
                h.end_at_ms = itx.end_at_ms;
                history_.push_back(h);
                log_history(h);
            }
        }

        // Kick everyone out and clear membership
        vector<int> members(r.participants.begin(), r.participants.end());
        for (int uid : members) {
            evt_to_user(uid, "EVT ROOM_ENDED " + to_string(rid) + "\n");
            users_[uid].current_room_id = 0;
        }
        r.participants.clear();

        mark_rooms_changed();


        log_server("[ROOM] ended rid=" + to_string(rid) + " by=" + to_string(by_uid));
    }

    void auction_tick(long long now) {
        for (auto& [rid, r] : rooms_) {
            if (r.current_item_id == 0) continue;

            auto& itx = items_[r.current_item_id];
            if (itx.state != ItemState::RUNNING) continue;

            long long remaining = itx.end_time_ms - now;

            if (!itx.alert_30_sent && remaining <= 30000 && remaining > 0) {
                itx.alert_30_sent = true;
                evt_to_room(rid, "EVT ALERT_30S " + to_string(itx.id) + " " + mmss(remaining) + "\n");
            }

            if (remaining <= 0) {
                if (itx.leader_user_id != 0) {
                    finalize_item(rid, itx.id, itx.leader_user_id, itx.current_price, "NORMAL_END");
                } else {
                    finalize_item(rid, itx.id, 0, 0, "EXPIRED");
                }
                if (rooms_[rid].queue.empty()) {
                    end_room(rid, rooms_[rid].owner_user_id);
                    mark_rooms_changed();
                } else {
                    start_next_in_room(rid, now_ms());
                }
            }
        }
    }

    // Draft helpers
    DraftItem* find_draft(int rid, int seller, const string& did) {
        DraftKey k{rid, seller};
        auto it = drafts_.find(k);
        if (it == drafts_.end()) return nullptr;
        for (auto& d : it->second) {
            if (d.draft_id == did) return &d;
        }
        return nullptr;
    }

    bool remove_draft(int rid, int seller, const string& did) {
        DraftKey k{rid, seller};
        auto it = drafts_.find(k);
        if (it == drafts_.end()) return false;
        auto& vec = it->second;
        auto before = vec.size();
        vec.erase(remove_if(vec.begin(), vec.end(), [&](const DraftItem& d){ return d.draft_id == did; }), vec.end());
        if (vec.empty()) drafts_.erase(k);
        return vec.size() != before;
    }

    // command handler
    void handle_line(Session& s, const string& line) {
        auto toks = split_ws(line);
        if (toks.empty()) return;

        string cmd = toks[0];
        for (auto& ch : cmd) ch = (char)toupper((unsigned char)ch);

        if (cmd == "HELP") return print_help(s);

        if (cmd == "SAVE") {
            save_state();
            return reply(s, "OK SAVED\n");
        }

        if (cmd == "REGISTER") {
            if (toks.size() < 3) return err(s, "INVALID", "Usage: REGISTER <username> <password>");
            string u = toks[1], p = toks[2];
            if (user_by_name_.count(u)) return err(s, "USER_EXISTS", "Username already exists");
            int uid = create_user(u, p, "BOTH");
            save_state();
            return reply(s, "OK REGISTERED " + to_string(uid) + "\n");
        }

        if (cmd == "LOGIN") {
            if (toks.size() < 3) return err(s, "INVALID", "Usage: LOGIN <username> <password>");
            string u = toks[1], p = toks[2];
            if (!user_by_name_.count(u)) return err(s, "AUTH_FAILED", "Bad credentials");
            int uid = user_by_name_[u];
            if (users_[uid].password != p) return err(s, "AUTH_FAILED", "Bad credentials");

            if (fd_by_user_.count(uid) && fd_by_user_[uid] != s.fd) {
                int oldfd = fd_by_user_[uid];
                send_line(oldfd, "ERR KICKED Logged in elsewhere\n");
                close_session(oldfd);
            }

            s.authed = true;
            s.user_id = uid;
            fd_by_user_[uid] = s.fd;

            log_server("[AUTH] login uid=" + to_string(uid) + " user=" + users_[uid].username);
            reply(s, "OK LOGIN " + to_string(uid) + " " + users_[uid].role + "\n");
            return reply(s, "OK HINT Try: HELP\n");
        }

        if (!s.authed) return err(s, "NOT_AUTHENTICATED", "Please LOGIN first");

        if (cmd == "LOGOUT") {
            int uid = s.user_id;
            int rid = users_[uid].current_room_id;
            if (rid) leave_room(uid, rid, true, nullptr);

            fd_by_user_.erase(uid);
            s.authed = false;
            s.user_id = 0;

            log_server("[AUTH] logout uid=" + to_string(uid));
            return reply(s, "OK LOGOUT\n");
        }

        if (cmd == "QUIT") {
            reply(s, "OK BYE\n");
            shutdown(s.fd, SHUT_RDWR);
            return;
        }

                if (cmd == "LIST_ROOMS") {
            int visible = 0;
            for (auto& [rid, r] : rooms_) if (r.status != RoomStatus::ENDED) visible++;

            reply(s, "OK ROOMS " + to_string(visible) + "\n");
            for (auto& [rid, r] : rooms_) {
                if (r.status == RoomStatus::ENDED) continue;
                string st = room_status_str(r.status);

                string ownerName = username_of(r.owner_user_id);
                reply(s, "EVT ROOM " + to_string(r.id) + " " + r.name + " " + st + " " +
                             to_string((int)r.participants.size()) + " " +
                             to_string(r.owner_user_id) + " " + ownerName + "\n");
            }
            return reply(s, "EVT ROOMS_END\n");
        }

        if (cmd == "LIST_MY_ROOMS") {
            int count = 0;
            for (auto& [rid, r] : rooms_) {
                if (r.owner_user_id == s.user_id) count++;
            }

            reply(s, "OK ROOMS " + to_string(count) + "\n");
            for (auto& [rid, r] : rooms_) {
                if (r.owner_user_id != s.user_id) continue;
                string st = room_status_str(r.status);

                string ownerName = username_of(r.owner_user_id);
                reply(s, "EVT ROOM " + to_string(r.id) + " " + r.name + " " + st + " " +
                             to_string((int)r.participants.size()) + " " +
                             to_string(r.owner_user_id) + " " + ownerName + "\n");
            }
            return reply(s, "EVT ROOMS_END\n");
        }

        // Get participant usernames of a room by id (for Rooms tab quick-chat)
        // ROOM_USERS <room_id>
        if (cmd == "ROOM_USERS") {
            if (toks.size() < 2) return err(s, "INVALID", "Usage: ROOM_USERS <room_id>");
            int rid = 0;
            if (!parse_int(toks[1], rid)) return err(s, "INVALID", "room_id must be a number");
            if (!rooms_.count(rid)) return err(s, "ROOM_NOT_FOUND", "No such room");

            reply(s, "OK ROOM_USERS\n");
            return reply(s, build_room_users_line(rid));
        }

        if (cmd == "CREATE_ROOM") {
            string name = trim(line.substr(toks[0].size()));
            if (name.empty()) name = "Room" + to_string(next_room_id_);

            int rid = next_room_id_++;
            Room r;
            r.id = rid;
            r.name = name;
            r.owner_user_id = s.user_id;
            rooms_[rid] = r;
            mark_rooms_changed();

            log_server("[ROOM] created rid=" + to_string(rid) + " name=" + name);
            reply(s, "OK ROOM_CREATED " + to_string(rid) + " " + name + "\n");
            join_room(s.user_id, rid, s);
            save_state();
            return;
        }

        if (cmd == "JOIN_ROOM") {
            string key = trim(line.substr(toks[0].size()));
            if (key.empty()) return err(s, "INVALID", "Usage: JOIN_ROOM <room_id_or_name>");

            int rid = 0;
            if (parse_int(key, rid)) {
                if (!rooms_.count(rid)) return err(s, "ROOM_NOT_FOUND", "No such room");
                return join_room(s.user_id, rid, s);
            }

            // Join by exact room name
            int found = 0;
            for (auto& [id, r] : rooms_) {
                if (r.name == key) { found = id; break; }
            }
            if (!found) return err(s, "ROOM_NOT_FOUND", "No such room name");
            return join_room(s.user_id, found, s);
        }

        if (cmd == "LEAVE_ROOM") {
            int rid = users_[s.user_id].current_room_id;
            if (!rid) return err(s, "NOT_IN_ROOM", "Not in any room");
            leave_room(s.user_id, rid, false, &s);
            return;
        }

        /* =======================
         * Chat (private + room)
         * Protocol:
         *  - CHAT_USERS
         *  - CHAT_TO <username>|<message...>
         *  - CHAT_ROOM |<message...>
         * Events:
         *  - EVT CHAT_USERS <n> <u1> ... <un>
         *  - EVT CHAT_FROM <from>|<message...>
         *  - EVT CHAT_ROOM <from>|<message...>
         * ======================= */
        if (cmd == "CHAT_USERS") {
            int rid = users_[s.user_id].current_room_id;
            if (!rid) return err(s, "NOT_IN_ROOM", "Join a room first");
            reply(s, "OK CHAT_USERS\n");
            return reply(s, build_chat_users_line(rid));
        }

        if (cmd == "CHAT_TO") {
            // Parse from raw line to preserve spaces: CHAT_TO <user>|<msg...>
            string rest = trim(line.substr(7)); // after "CHAT_TO"
            auto bar = rest.find('|');
            if (bar == string::npos) return err(s, "INVALID", "Usage: CHAT_TO <user>|<message>");
            string to_name = trim(rest.substr(0, bar));
            string msg = rest.substr(bar + 1);

            if (to_name.empty()) return err(s, "INVALID", "recipient required");
            if (msg.empty()) return err(s, "INVALID", "message required");

            // sanitize
            for (auto& ch : msg) {
                if (ch == '\r' || ch == '\n') ch = ' ';
            }
            if ((int)msg.size() > 600) msg.resize(600);

            if (!user_by_name_.count(to_name)) return err(s, "USER_NOT_FOUND", "No such user");
            int to_uid = user_by_name_[to_name];
            auto itfd = fd_by_user_.find(to_uid);
            if (itfd == fd_by_user_.end()) return err(s, "USER_OFFLINE", "User is offline");

            const string from_name = users_[s.user_id].username;
            // deliver
            send_line(itfd->second, "EVT CHAT_FROM " + from_name + "|" + msg + "\n");
            log_chat(now_ms(), from_name, to_name, users_[s.user_id].current_room_id, msg);

            return reply(s, "OK CHAT_TO\n");
        }

        if (cmd == "CHAT_ROOM") {
            int rid = users_[s.user_id].current_room_id;
            if (!rid) return err(s, "NOT_IN_ROOM", "Join a room first");

            // Parse: CHAT_ROOM |<msg...>
            string rest = trim(line.substr(9)); // after "CHAT_ROOM"
            auto bar = rest.find('|');
            if (bar == string::npos) return err(s, "INVALID", "Usage: CHAT_ROOM |<message>");
            string msg = rest.substr(bar + 1);
            if (msg.empty()) return err(s, "INVALID", "message required");

            for (auto& ch : msg) {
                if (ch == '\r' || ch == '\n') ch = ' ';
            }
            if ((int)msg.size() > 600) msg.resize(600);

            const string from_name = users_[s.user_id].username;
            evt_to_room(rid, "EVT CHAT_ROOM " + from_name + "|" + msg + "\n");
            log_chat(now_ms(), from_name, "ROOM", rid, msg);
            return reply(s, "OK CHAT_ROOM\n");
        }


        if (cmd == "ADD_ITEM") {
            int rid = users_[s.user_id].current_room_id;
            if (!rid) return err(s, "NOT_IN_ROOM", "Join a room first");

            if (!rooms_.count(rid)) return err(s, "ROOM_NOT_FOUND", "No such room");
            auto& room = rooms_[rid];
            if (room.status == RoomStatus::ENDED) return err(s, "ROOM_ENDED", "Room already ended");
            if (room.owner_user_id != s.user_id) return err(s, "NOT_OWNER", "Only room owner can add items");
            if (room.status != RoomStatus::PENDING) return err(s, "ROOM_NOT_PENDING", "Room is not pending (cannot add items)");

            string payload = trim(line.substr(toks[0].size()));
            auto parts = split_char(payload, '|');
            if (parts.size() != 4) return err(s, "INVALID_ITEM_FORMAT", "Use: ADD_ITEM name|start|buy_now|duration");

            string name = trim(parts[0]);
            int start=0, buy=0, dur=0;
            if (name.empty()) return err(s, "INVALID_ITEM_FORMAT", "name required");
            if (!parse_int(parts[1], start) || !parse_int(parts[2], buy) || !parse_int(parts[3], dur))
                return err(s, "INVALID_ITEM_FORMAT", "start/buy_now/duration must be numbers");
            if (start <= 0) return err(s, "INVALID_PRICE", "start must be > 0");
            if (dur < 10) return err(s, "INVALID_PRICE", "duration must be >= 10s");
            if (buy != 0 && buy < start) return err(s, "INVALID_PRICE", "buy_now must be 0 or >= start");

            DraftKey dk{rid, s.user_id};
            auto& vec = drafts_[dk];
            string did = "D" + to_string((int)vec.size() + 1);

            DraftItem d;
            d.draft_id = did;
            d.room_id = rid;
            d.seller_user_id = s.user_id;
            d.name = name;
            d.start_price = start;
            d.buy_now_price = buy;
            d.duration_sec = dur;
            vec.push_back(d);

            reply(s, "OK DRAFT_ADDED " + did + "\n");
            reply(s, "EVT DRAFT_ITEM " + did + " " + name + " " + to_string(start) + " " +
                         to_string(buy) + " " + to_string(dur) + "\n");
            return;
        }

        if (cmd == "DRAFT_LIST") {
            int rid = users_[s.user_id].current_room_id;
            if (!rid) return err(s, "NOT_IN_ROOM", "Join a room first");

            if (!rooms_.count(rid)) return err(s, "ROOM_NOT_FOUND", "No such room");
            auto& room = rooms_[rid];
            if (room.status == RoomStatus::ENDED) return err(s, "ROOM_ENDED", "Room already ended");
            if (room.owner_user_id != s.user_id) return err(s, "NOT_OWNER", "Only room owner can view drafts");
            if (room.status != RoomStatus::PENDING) return err(s, "ROOM_NOT_PENDING", "Room is not pending");

            DraftKey dk{rid, s.user_id};
            auto it = drafts_.find(dk);
            int k = (it == drafts_.end()) ? 0 : (int)it->second.size();
            reply(s, "OK DRAFT_LIST " + to_string(k) + "\n");
            if (it != drafts_.end()) {
                for (auto& d : it->second) {
                    reply(s, "EVT DRAFT " + d.draft_id + " " + d.name + " " +
                                 to_string(d.start_price) + " " + to_string(d.buy_now_price) + " " +
                                 to_string(d.duration_sec) + "\n");
                }
            }
            return reply(s, "EVT DRAFT_END\n");
        }

        if (cmd == "EDIT_ITEM") {
            int rid = users_[s.user_id].current_room_id;
            if (!rid) return err(s, "NOT_IN_ROOM", "Join a room first");

            if (!rooms_.count(rid)) return err(s, "ROOM_NOT_FOUND", "No such room");
            auto& room = rooms_[rid];
            if (room.status == RoomStatus::ENDED) return err(s, "ROOM_ENDED", "Room already ended");
            if (room.owner_user_id != s.user_id) return err(s, "NOT_OWNER", "Only room owner can edit drafts");
            if (room.status != RoomStatus::PENDING) return err(s, "ROOM_NOT_PENDING", "Room is not pending (cannot edit drafts)");

            string payload = trim(line.substr(toks[0].size()));
            auto parts = split_char(payload, '|');
            if (parts.size() != 3) return err(s, "INVALID", "Use: EDIT_ITEM D1|field|value (field=name|start|buy_now|duration)");

            string did = trim(parts[0]);
            string field = trim(parts[1]);
            string value = trim(parts[2]);

            auto* d = find_draft(rid, s.user_id, did);
            if (!d) return err(s, "NOT_FOUND", "Draft not found");

            for (auto& ch : field) ch = (char)tolower((unsigned char)ch);

            if (field == "name") {
                if (value.empty()) return err(s, "INVALID", "name cannot be empty");
                d->name = value;
            } else if (field == "start") {
                int v=0; if (!parse_int(value, v)) return err(s, "INVALID", "start must be number");
                if (v <= 0) return err(s, "INVALID_PRICE", "start must be > 0");
                d->start_price = v;
                if (d->buy_now_price != 0 && d->buy_now_price < d->start_price)
                    return err(s, "INVALID_PRICE", "buy_now must be 0 or >= start");
            } else if (field == "buy_now" || field == "buynow") {
                int v=0; if (!parse_int(value, v)) return err(s, "INVALID", "buy_now must be number");
                if (v != 0 && v < d->start_price) return err(s, "INVALID_PRICE", "buy_now must be 0 or >= start");
                d->buy_now_price = v;
            } else if (field == "duration") {
                int v=0; if (!parse_int(value, v)) return err(s, "INVALID", "duration must be number");
                if (v < 10) return err(s, "INVALID_PRICE", "duration must be >= 10s");
                d->duration_sec = v;
            } else {
                return err(s, "INVALID", "field must be name|start|buy_now|duration");
            }

            reply(s, "OK DRAFT_UPDATED " + did + "\n");
            return reply(s, "EVT DRAFT " + d->draft_id + " " + d->name + " " +
                             to_string(d->start_price) + " " + to_string(d->buy_now_price) + " " +
                             to_string(d->duration_sec) + "\n");
        }

        if (cmd == "REMOVE_ITEM") {
            int rid = users_[s.user_id].current_room_id;
            if (!rid) return err(s, "NOT_IN_ROOM", "Join a room first");
            if (toks.size() < 2) return err(s, "INVALID", "Usage: REMOVE_ITEM <draft_id>");

            if (!rooms_.count(rid)) return err(s, "ROOM_NOT_FOUND", "No such room");
            auto& room = rooms_[rid];
            if (room.status == RoomStatus::ENDED) return err(s, "ROOM_ENDED", "Room already ended");
            if (room.owner_user_id != s.user_id) return err(s, "NOT_OWNER", "Only room owner can remove drafts");
            if (room.status != RoomStatus::PENDING) return err(s, "ROOM_NOT_PENDING", "Room is not pending (cannot remove drafts)");

            string did = toks[1];
            if (!remove_draft(rid, s.user_id, did)) return err(s, "NOT_FOUND", "Draft not found");
            return reply(s, "OK DRAFT_REMOVED " + did + "\n");
        }
        if (cmd == "CLEAR_DRAFT") {
            int rid = users_[s.user_id].current_room_id;
            if (!rid) return err(s, "NOT_IN_ROOM", "Join a room first");

            if (!rooms_.count(rid)) return err(s, "ROOM_NOT_FOUND", "No such room");
            auto& room = rooms_[rid];
            if (room.status == RoomStatus::ENDED) return err(s, "ROOM_ENDED", "Room already ended");
            if (room.owner_user_id != s.user_id) return err(s, "NOT_OWNER", "Only room owner can clear drafts");
            if (room.status != RoomStatus::PENDING) return err(s, "ROOM_NOT_PENDING", "Room is not pending (cannot clear drafts)");

            DraftKey dk{rid, s.user_id};
            int removed = 0;
            auto it = drafts_.find(dk);
            if (it != drafts_.end()) {
                removed = (int)it->second.size();
                drafts_.erase(it);
            }

            reply(s, "OK DRAFT_CLEARED " + to_string(removed) + "\n");
            return reply(s, "EVT DRAFT_CLEARED " + to_string(removed) + "\n");
        }


        if (cmd == "OK") {
            int rid = users_[s.user_id].current_room_id;
            if (!rid) return err(s, "NOT_IN_ROOM", "Join a room first");

            if (!rooms_.count(rid)) return err(s, "ROOM_NOT_FOUND", "No such room");
            auto& room = rooms_[rid];
            if (room.status == RoomStatus::ENDED) return err(s, "ROOM_ENDED", "Room already ended");
            if (room.owner_user_id != s.user_id) return err(s, "NOT_OWNER", "Only room owner can publish items");
            if (room.status != RoomStatus::PENDING) return err(s, "ROOM_NOT_PENDING", "Room is not pending (cannot publish)");

            DraftKey dk{rid, s.user_id};
            auto it = drafts_.find(dk);
            if (it == drafts_.end() || it->second.empty()) return err(s, "DRAFT_EMPTY", "No draft items");

            auto& vec = it->second;
            int count = (int)vec.size();
            vector<int> new_ids;
            new_ids.reserve(vec.size());

            for (auto& d : vec) {
                Item item;
                item.id = next_item_id_++;
                item.room_id = rid;
                item.seller_user_id = s.user_id;
                item.name = d.name;
                item.start_price = d.start_price;
                item.buy_now_price = d.buy_now_price;
                item.duration_sec = d.duration_sec;
                item.state = ItemState::QUEUED;
                item.current_price = item.start_price;
                items_[item.id] = item;

                rooms_[rid].queue.push_back(item.id);
                new_ids.push_back(item.id);
            }
            drafts_.erase(dk);

            reply(s, "OK PUBLISHED " + to_string(count) + "\n");

            string seller = username_of(s.user_id);
            evt_to_room(rid, "EVT PUBLISHED " + seller + " " + to_string(count) + "\n");
            for (int iid : new_ids) {
                auto& itx = items_[iid];
                evt_to_room(rid, "EVT QUEUE_ADDED " + to_string(itx.id) + " " + itx.name + " " +
                                 to_string(itx.start_price) + " " + to_string(itx.buy_now_price) + " " +
                                 to_string(itx.duration_sec) + "\n");
            }

            // Do NOT auto-start. Owner must explicitly START.
            rooms_[rid].status = RoomStatus::PENDING;
            evt_to_room(rid, "EVT ROOM_PENDING " + to_string(rid) + "\n");
            save_state();
            return;
        }

        if (cmd == "START") {
            int rid = users_[s.user_id].current_room_id;
            if (!rid) return err(s, "NOT_IN_ROOM", "Join a room first");
            if (!rooms_.count(rid)) return err(s, "ROOM_NOT_FOUND", "No such room");

            auto& room = rooms_[rid];
            if (room.status == RoomStatus::ENDED) return err(s, "ROOM_ENDED", "Room already ended");
            if (room.owner_user_id != s.user_id) return err(s, "NOT_OWNER", "Only room owner can start");

            if (room.current_item_id == 0 && room.queue.empty())
                return err(s, "NO_ITEMS", "No published items to start");

            room.status = RoomStatus::STARTED;
            mark_rooms_changed();
            evt_to_room(rid, "EVT ROOM_STARTED " + to_string(rid) + "\n");
            reply(s, "OK ROOM_STARTED " + to_string(rid) + "\n");

            if (room.current_item_id == 0) start_next_in_room(rid, now_ms());
            save_state();
            return;
        }

        if (cmd == "END") {
            int rid = users_[s.user_id].current_room_id;
            if (!rid) return err(s, "NOT_IN_ROOM", "Join a room first");
            if (!rooms_.count(rid)) return err(s, "ROOM_NOT_FOUND", "No such room");

            auto& room = rooms_[rid];
            if (room.status == RoomStatus::ENDED) return err(s, "ROOM_ENDED", "Room already ended");
            if (room.owner_user_id != s.user_id) return err(s, "NOT_OWNER", "Only room owner can end");

            end_room(rid, s.user_id);
            reply(s, "OK ROOM_ENDED " + to_string(rid) + "\n");
            save_state();
            return;
        }

        if (cmd == "VIEW") {
            if (toks.size() < 2) return err(s, "INVALID", "Usage: VIEW RUNNING|QUEUED|ENDED");
            string kind = toks[1];
            for (auto& ch : kind) ch = (char)toupper((unsigned char)ch);

            int rid = users_[s.user_id].current_room_id;
            if (!rid) return err(s, "NOT_IN_ROOM", "Join a room first");

            if (kind == "RUNNING") {
                reply(s, "OK VIEW_RUNNING\n");
                int cid = rooms_[rid].current_item_id;
                if (!cid) return reply(s, "EVT CURRENT NONE\n");
                auto& itx = items_[cid];
                long long tl = itx.end_time_ms - now_ms();
                return reply(s, "EVT CURRENT " + to_string(itx.id) + " " + itx.name + " " + username_of(itx.seller_user_id) +
                             " " + to_string(itx.start_price) + " " + to_string(itx.buy_now_price) + " " +
                             to_string(itx.current_price) + " " +
                             (itx.leader_user_id ? username_of(itx.leader_user_id) : "none") + " " +
                             mmss(tl) + "\n");
            }

            if (kind == "QUEUED") {
                auto& q = rooms_[rid].queue;
                reply(s, "OK VIEW_QUEUED " + to_string((int)q.size()) + "\n");
                for (int iid : q) {
                    auto& itx = items_[iid];
                    reply(s, "EVT QUEUED " + to_string(itx.id) + " " + itx.name + " " +
                             to_string(itx.start_price) + " " + to_string(itx.buy_now_price) + " " +
                             to_string(itx.duration_sec) + "\n");
                }
                return reply(s, "EVT QUEUED_END\n");
            }

            if (kind == "ENDED") {
                int n = 0; for (auto& h : history_) if (h.room_id == rid) n++;
                reply(s, "OK VIEW_ENDED " + to_string(n) + "\n");
                for (auto& h : history_) {
                    if (h.room_id != rid) continue;
                    string winner = h.winner_user_id ? username_of(h.winner_user_id) : "none";
                    string status = (h.winner_user_id ? "SOLD" : "EXPIRED");
                    reply(s, "EVT ENDED " + to_string(h.item_id) + " " + status + " " + winner + " " +
                             to_string(h.final_price) + " " + h.reason + " " + to_string(h.end_at_ms) + "\n");
                }
                return reply(s, "EVT ENDED_END\n");
            }

            return err(s, "INVALID", "VIEW expects RUNNING|QUEUED|ENDED");
        }

        if (cmd == "BID") {
            if (toks.size() < 3) return err(s, "INVALID", "Usage: BID <item_id> <amount>");
            int iid=0, amount=0;
            if (!parse_int(toks[1], iid) || !parse_int(toks[2], amount))
                return err(s, "INVALID", "item_id/amount must be numbers");

            if (!items_.count(iid)) return err(s, "ITEM_NOT_FOUND", "No such item");
            auto& itx = items_[iid];
            if (itx.state != ItemState::RUNNING) return err(s, "ITEM_NOT_RUNNING", "Item not running");

            if (!rooms_.count(itx.room_id)) return err(s, "ROOM_NOT_FOUND", "No such room");
            if (rooms_[itx.room_id].status != RoomStatus::STARTED)
                return err(s, "ROOM_NOT_STARTED", "Room is not started");

            int rid = users_[s.user_id].current_room_id;
            if (!rid || rid != itx.room_id) return err(s, "NOT_IN_ROOM", "Join the room of this item");
            if (s.user_id == itx.seller_user_id) return err(s, "FORBIDDEN", "Seller cannot bid");

            int min_req = itx.current_price + 10000;
            if (amount < min_req) return err(s, "BID_TOO_LOW", "min=" + to_string(min_req));

            long long now = now_ms();
            long long remaining = itx.end_time_ms - now;

            itx.current_price = amount;
            itx.leader_user_id = s.user_id;

            if (remaining <= 30000) {
                itx.end_time_ms = now + 30000;
                remaining = 30000;
                evt_to_room(itx.room_id, "EVT TIME_EXTENDED " + to_string(iid) + " 30\n");
            } else {
                remaining = itx.end_time_ms - now;
            }

            reply(s, "OK BID_ACCEPTED " + to_string(iid) + " " + to_string(amount) + "\n");
            evt_to_room(itx.room_id, "EVT PRICE_UPDATE " + to_string(iid) + " " + to_string(amount) + " " +
                        username_of(s.user_id) + " " + mmss(remaining) + "\n");
            return;
        }

        if (cmd == "BUY_NOW") {
            if (toks.size() < 2) return err(s, "INVALID", "Usage: BUY_NOW <item_id>");
            int iid=0;
            if (!parse_int(toks[1], iid)) return err(s, "INVALID", "item_id must be number");

            if (!items_.count(iid)) return err(s, "ITEM_NOT_FOUND", "No such item");
            auto& itx = items_[iid];

            if (itx.state != ItemState::RUNNING) return err(s, "ITEM_NOT_RUNNING", "Item not running");
            if (!rooms_.count(itx.room_id)) return err(s, "ROOM_NOT_FOUND", "No such room");
            if (rooms_[itx.room_id].status != RoomStatus::STARTED)
                return err(s, "ROOM_NOT_STARTED", "Room is not started");
            if (itx.buy_now_price <= 0) return err(s, "BUY_NOW_NOT_AVAILABLE", "No buy_now");

            int rid = users_[s.user_id].current_room_id;
            if (!rid || rid != itx.room_id) return err(s, "NOT_IN_ROOM", "Join the room of this item");
            if (s.user_id == itx.seller_user_id) return err(s, "FORBIDDEN", "Seller cannot buy_now");

            finalize_item(itx.room_id, iid, s.user_id, itx.buy_now_price, "BUY_NOW");
            reply(s, "OK BUY_NOW_SUCCESS " + to_string(iid) + " " + to_string(itx.buy_now_price) + "\n");
// If this was the last item, end the room (kick everyone out). Otherwise continue to next item.
            if (rooms_[itx.room_id].queue.empty()) {
                end_room(itx.room_id, s.user_id);
            } else {
                start_next_in_room(itx.room_id, now_ms());
            }

            save_state();
            return;
        }

        if (cmd == "HISTORY") {
            if (toks.size() < 2) return err(s, "INVALID", "Usage: HISTORY ME");
            string sub = toks[1];
            for (auto& ch : sub) ch = (char)toupper((unsigned char)ch);
            if (sub != "ME") return err(s, "INVALID", "Only HISTORY ME supported");

            int uid = s.user_id;
            vector<HistoryRow> rows;
            for (auto& h : history_) {
                if (h.seller_user_id == uid || h.winner_user_id == uid) rows.push_back(h);
            }

            reply(s, "OK HISTORY " + to_string((int)rows.size()) + "\n");
            for (auto& h : rows) {
                string role = (h.seller_user_id == uid ? "SELLER" : "BUYER");
                reply(s, "EVT HIST " + to_string(h.item_id) + " " + to_string(h.room_id) + " " + role + " " +
                         username_of(h.seller_user_id) + " " +
                         (h.winner_user_id ? username_of(h.winner_user_id) : string("-")) + " " +
                         to_string(h.final_price) + " " + h.reason + " " + to_string(h.end_at_ms) + "\n");
            }
            return reply(s, "EVT HISTORY_END\n");
        }

        if (cmd == "SEARCH") {
            string keyword = trim(line.substr(toks[0].size()));
            if (keyword.empty()) return err(s, "INVALID", "Usage: SEARCH <keyword>");

            vector<Item*> results;
            for (auto& [iid, itx] : items_) {
                if (icontains(itx.name, keyword)) results.push_back(&itx);
            }

            reply(s, "OK SEARCH " + to_string((int)results.size()) + "\n");
            for (auto* itp : results) {
                string st;
                switch (itp->state) {
                    case ItemState::QUEUED: st = "QUEUED"; break;
                    case ItemState::RUNNING: st = "RUNNING"; break;
                    case ItemState::SOLD: st = "SOLD"; break;
                    case ItemState::EXPIRED: st = "EXPIRED"; break;
                }
                reply(s, "EVT RESULT " + to_string(itp->id) + " " + to_string(itp->room_id) + " " + st + " " +
                         itp->name + " " + to_string(itp->current_price) + "\n");
            }
            return reply(s, "EVT SEARCH_END\n");
        }

        err(s, "UNKNOWN_CMD", "Unknown command (try HELP)");
    }
};

Server* Server::g_instance = nullptr;

int main(int argc, char** argv) {
    int port = 8080;
    if (argc >= 2) port = atoi(argv[1]);
    Server s(port);
    if (!s.run()) return 1;
    return 0;
}
