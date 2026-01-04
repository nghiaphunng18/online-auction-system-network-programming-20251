// client.cpp - Auction Client CLI (C++17, POSIX sockets)
// Build: g++ -std=c++17 -O2 client.cpp -o client
// Run  : ./client 127.0.0.1 8080

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>

using namespace std;

static bool connect_to(const string& host, int port, int& outfd) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);

    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
        close(fd);
        return false;
    }
    if (connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(fd);
        return false;
    }
    outfd = fd;
    return true;
}

static bool send_all(int fd, const string& s) {
    size_t off = 0;
    while (off < s.size()) {
        ssize_t n = send(fd, s.data() + off, s.size() - off, 0);
        if (n < 0) return false;
        off += (size_t)n;
    }
    return true;
}

int main(int argc, char** argv) {
    string host = "127.0.0.1";
    int port = 8080;
    if (argc >= 2) host = argv[1];
    if (argc >= 3) port = atoi(argv[2]);

    int fd = -1;
    if (!connect_to(host, port, fd)) {
        cerr << "Cannot connect\n";
        return 1;
    }

    cout << "Connected. Type HELP for commands. Type QUIT to exit.\n\n";

    string inbuf;

    while (true) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        FD_SET(STDIN_FILENO, &rfds);
        int maxfd = (fd > STDIN_FILENO ? fd : STDIN_FILENO);

        int n = select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("select");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &rfds)) {
            string line;
            if (!getline(cin, line)) break;

            if (line == "QUIT") {
                send_all(fd, "QUIT\n");
                cout << "Goodbye!\n";
                break;
            }

            line += "\n";
            if (!send_all(fd, line)) break;
        }

        if (FD_ISSET(fd, &rfds)) {
            char buf[4096];
            ssize_t r = recv(fd, buf, sizeof(buf), 0);
            if (r <= 0) {
                cout << "Disconnected.\n";
                break;
            }
            inbuf.append(buf, buf + r);
            while (true) {
                auto pos = inbuf.find('\n');
                if (pos == string::npos) break;
                string line = inbuf.substr(0, pos);
                inbuf.erase(0, pos + 1);
                cout << line << "\n";
            }
        }
    }

    close(fd);
    return 0;
}
