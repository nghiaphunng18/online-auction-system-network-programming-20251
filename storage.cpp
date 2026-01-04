#include "storage.hpp"
#include "common.hpp"

#include <fstream>

using namespace std;

size_t DraftKeyHash::operator()(DraftKey const& k) const noexcept {
    return (size_t)k.room_id * 1315423911u ^ (size_t)k.seller_id * 2654435761u;
}
bool DraftKeyEq::operator()(DraftKey const& a, DraftKey const& b) const noexcept {
    return a.room_id == b.room_id && a.seller_id == b.seller_id;
}

static string b2s(bool b) { return b ? "1" : "0"; }
static bool s2b(const string& s) { return s == "1" || s == "true" || s == "TRUE"; }

bool save_state_file(const string& path, const PersistState& st) {
    ofstream f(path, ios::out | ios::trunc);
    if (!f) return false;

    f << "META|" << st.next_user_id << "|" << st.next_room_id << "|" << st.next_item_id << "\n";

    f << "USERS|" << st.users.size() << "\n";
    for (auto& [id, u] : st.users) {
        f << "U|" << u.id
          << "|" << esc_field(u.username)
          << "|" << esc_field(u.password)
          << "|" << esc_field(u.role)
          << "|0\n";
    }

    f << "ROOMS|" << st.rooms.size() << "\n";
    for (auto& [id, r] : st.rooms) {
        f << "R|" << r.id
          << "|" << esc_field(r.name)
          << "|" << r.owner_user_id
          << "|" << (int)r.status
          << "|" << r.current_item_id
          << "\n";

        f << "P|" << r.id << "|0\n";

        f << "Q|" << r.id << "|" << r.queue.size();
        for (int iid : r.queue) f << "|" << iid;
        f << "\n";
    }

    f << "ITEMS|" << st.items.size() << "\n";
    for (auto& [id, it] : st.items) {
        f << "I|" << it.id
          << "|" << it.room_id
          << "|" << it.seller_user_id
          << "|" << esc_field(it.name)
          << "|" << it.start_price
          << "|" << it.buy_now_price
          << "|" << it.duration_sec
          << "|" << (int)it.state
          << "|" << it.current_price
          << "|" << it.leader_user_id
          << "|" << it.start_time_ms
          << "|" << it.end_time_ms
          << "|" << b2s(it.alert_30_sent)
          << "|" << it.winner_user_id
          << "|" << it.final_price
          << "|" << esc_field(it.reason)
          << "|" << it.end_at_ms
          << "\n";
    }

    f << "HISTORY|" << st.history.size() << "\n";
    for (auto& h : st.history) {
        f << "H|" << h.item_id
          << "|" << h.room_id
          << "|" << h.seller_user_id
          << "|" << h.winner_user_id
          << "|" << h.final_price
          << "|" << esc_field(h.reason)
          << "|" << h.end_at_ms
          << "\n";
    }

    size_t draft_count = 0;
    for (auto& [k, vec] : st.drafts) draft_count += vec.size();
    f << "DRAFTS|" << draft_count << "\n";
    for (auto& [k, vec] : st.drafts) {
        for (auto& d : vec) {
            f << "D|" << d.room_id
              << "|" << d.seller_user_id
              << "|" << esc_field(d.draft_id)
              << "|" << esc_field(d.name)
              << "|" << d.start_price
              << "|" << d.buy_now_price
              << "|" << d.duration_sec
              << "\n";
        }
    }

    f << "END\n";
    return true;
}

static bool read_line(ifstream& f, string& out) {
    out.clear();
    return (bool)std::getline(f, out);
}

bool load_state_file(const string& path, PersistState& st, string& err) {
    ifstream f(path);
    if (!f) { err = "cannot open file"; return false; }

    PersistState tmp;

    string line;
    while (read_line(f, line)) {
        line = trim(line);
        if (line.empty()) continue;

        auto parts_raw = split_escaped_pipe(line);
        if (parts_raw.empty()) continue;

        vector<string> parts;
        parts.reserve(parts_raw.size());
        for (auto& p : parts_raw) parts.push_back(unesc_field(p));

        string tag = parts[0];
        if (tag == "END") break;

        if (tag == "META") {
            if (parts.size() != 4) { err = "bad META"; return false; }
            if (!parse_int(parts[1], tmp.next_user_id) ||
                !parse_int(parts[2], tmp.next_room_id) ||
                !parse_int(parts[3], tmp.next_item_id)) {
                err = "bad META numbers"; return false;
            }
        } else if (tag == "USERS" || tag == "ROOMS" || tag == "ITEMS" || tag == "HISTORY" || tag == "DRAFTS") {
            continue;
        } else if (tag == "U") {
            if (parts.size() != 6) { err = "bad U row"; return false; }
            User u;
            if (!parse_int(parts[1], u.id)) { err = "bad user id"; return false; }
            u.username = parts[2];
            u.password = parts[3];
            u.role = parts[4];
            u.current_room_id = 0;
            tmp.users[u.id] = u;
            tmp.user_by_name[u.username] = u.id;
        } else if (tag == "R") {
            if (parts.size() != 6) { err = "bad R row"; return false; }
            Room r;
            int stt = 0;
            if (!parse_int(parts[1], r.id)) { err = "bad room id"; return false; }
            r.name = parts[2];
            if (!parse_int(parts[3], r.owner_user_id)) { err = "bad owner"; return false; }
            if (!parse_int(parts[4], stt)) { err = "bad room status"; return false; }
            // Backward compatible mapping:
            // - old versions: 0=IDLE, 1=RUNNING
            // - new versions: 0=PENDING, 1=STARTED, 2=ENDED
            if (stt <= 0) r.status = RoomStatus::PENDING;
            else if (stt == 1) r.status = RoomStatus::STARTED;
            else r.status = RoomStatus::ENDED;
            if (!parse_int(parts[5], r.current_item_id)) { err = "bad current item"; return false; }
            r.participants.clear();
            tmp.rooms[r.id] = r;
        } else if (tag == "P") {
            continue;
        } else if (tag == "Q") {
            if (parts.size() < 3) { err = "bad Q row"; return false; }
            int rid = 0, cnt = 0;
            if (!parse_int(parts[1], rid) || !parse_int(parts[2], cnt)) { err = "bad Q header"; return false; }
            if (!tmp.rooms.count(rid)) { err = "Q references unknown room"; return false; }
            tmp.rooms[rid].queue.clear();
            for (int i = 0; i < cnt; i++) {
                int iid = 0;
                if ((int)parts.size() <= 3 + i) { err = "Q count mismatch"; return false; }
                if (!parse_int(parts[3 + i], iid)) { err = "bad iid"; return false; }
                tmp.rooms[rid].queue.push_back(iid);
            }
        } else if (tag == "I") {
            if (parts.size() != 18) { err = "bad I row"; return false; }
            Item it;
            int state = 0;
            if (!parse_int(parts[1], it.id) ||
                !parse_int(parts[2], it.room_id) ||
                !parse_int(parts[3], it.seller_user_id)) { err = "bad item ids"; return false; }
            it.name = parts[4];
            if (!parse_int(parts[5], it.start_price) ||
                !parse_int(parts[6], it.buy_now_price) ||
                !parse_int(parts[7], it.duration_sec) ||
                !parse_int(parts[8], state) ||
                !parse_int(parts[9], it.current_price) ||
                !parse_int(parts[10], it.leader_user_id)) { err = "bad item numbers"; return false; }

            it.start_time_ms = stoll(parts[11]);
            it.end_time_ms   = stoll(parts[12]);
            it.alert_30_sent = s2b(parts[13]);

            if (!parse_int(parts[14], it.winner_user_id) ||
                !parse_int(parts[15], it.final_price)) { err = "bad item result"; return false; }
            it.reason = parts[16];
            it.end_at_ms = stoll(parts[17]);

            it.state = (ItemState)state;
            tmp.items[it.id] = it;
        } else if (tag == "H") {
            if (parts.size() != 8) { err = "bad H row"; return false; }
            HistoryRow h;
            if (!parse_int(parts[1], h.item_id) ||
                !parse_int(parts[2], h.room_id) ||
                !parse_int(parts[3], h.seller_user_id) ||
                !parse_int(parts[4], h.winner_user_id) ||
                !parse_int(parts[5], h.final_price)) { err = "bad history numbers"; return false; }
            h.reason = parts[6];
            h.end_at_ms = stoll(parts[7]);
            tmp.history.push_back(h);
        } else if (tag == "D") {
            if (parts.size() != 8) { err = "bad D row"; return false; }
            DraftItem d;
            if (!parse_int(parts[1], d.room_id) ||
                !parse_int(parts[2], d.seller_user_id)) { err = "bad draft ids"; return false; }
            d.draft_id = parts[3];
            d.name = parts[4];
            if (!parse_int(parts[5], d.start_price) ||
                !parse_int(parts[6], d.buy_now_price) ||
                !parse_int(parts[7], d.duration_sec)) { err = "bad draft numbers"; return false; }

            DraftKey k{d.room_id, d.seller_user_id};
            tmp.drafts[k].push_back(d);
        }
    }

    // Reset volatile fields on boot (safer)
    for (auto& [uid, u] : tmp.users) u.current_room_id = 0;
    for (auto& [rid, r] : tmp.rooms) r.participants.clear();

    // Reset volatile fields on boot:
    // - rooms STARTED can't continue after restart -> move back to PENDING
    // - any RUNNING item is re-queued
    // - ENDED rooms stay ENDED (but are not joinable/listable)
    for (auto& [rid, r] : tmp.rooms) {
        // If there were RUNNING items saved, requeue them
        if (r.current_item_id != 0 && tmp.items.count(r.current_item_id)) {
            auto& it = tmp.items[r.current_item_id];
            if (it.state == ItemState::RUNNING) {
                it.state = ItemState::QUEUED;
                it.leader_user_id = 0;
                it.alert_30_sent = false;
                r.queue.push_front(it.id);
            }
        }

        r.current_item_id = 0;

        if (r.status == RoomStatus::ENDED) {
            // Safety: ended rooms should not keep any live queue
            r.queue.clear();
        } else {
            // Any non-ended room is reset to PENDING on boot
            r.status = RoomStatus::PENDING;
        }
    }

    st = std::move(tmp);
    return true;
}
