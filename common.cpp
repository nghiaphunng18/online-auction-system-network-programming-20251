#include "common.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cerrno>
#include <climits>
#include <cstring>
#include <fcntl.h>
#include <sstream>
#include <unistd.h>

using namespace std;

long long now_ms() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

string trim(string s) {
    auto issp = [](unsigned char c) { return std::isspace(c); };
    while (!s.empty() && issp((unsigned char)s.front())) s.erase(s.begin());
    while (!s.empty() && issp((unsigned char)s.back())) s.pop_back();
    return s;
}

vector<string> split_ws(const string& s) {
    vector<string> out;
    istringstream iss(s);
    string tok;
    while (iss >> tok) out.push_back(tok);
    return out;
}

vector<string> split_char(const string& s, char delim) {
    vector<string> out;
    string cur;
    for (char c : s) {
        if (c == delim) {
            out.push_back(cur);
            cur.clear();
        } else cur.push_back(c);
    }
    out.push_back(cur);
    return out;
}

bool icontains(const string& a, const string& b) {
    auto lower = [](string x) {
        for (auto& ch : x) ch = (char)tolower((unsigned char)ch);
        return x;
    };
    return lower(a).find(lower(b)) != string::npos;
}

bool set_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return false;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) == 0;
}

bool parse_int(const string& s, int& out) {
    string t = trim(s);
    if (t.empty()) return false;
    char* end = nullptr;
    errno = 0;
    long v = strtol(t.c_str(), &end, 10);
    if (errno != 0) return false;
    if (end == t.c_str() || *end != '\0') return false;
    if (v < INT_MIN || v > INT_MAX) return false;
    out = (int)v;
    return true;
}

string mmss(long long ms_left) {
    if (ms_left < 0) ms_left = 0;
    long long sec = ms_left / 1000;
    long long m = sec / 60;
    long long s = sec % 60;
    char buf[32];
    snprintf(buf, sizeof(buf), "%02lld:%02lld", m, s);
    return buf;
}

// Escape rules for persistence (single-line, pipe-delimited):
// - '\\'  -> '\\\\'
// - '|'    -> '\\|'
// - '\n'/'\r' removed
string esc_field(const string& s) {
    string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        if (c == '\n' || c == '\r') continue;
        if (c == '\\') { out += "\\\\"; continue; }
        if (c == '|')  { out += "\\|";  continue; }
        out.push_back(c);
    }
    return out;
}

string unesc_field(const string& s) {
    string out;
    out.reserve(s.size());
    bool esc = false;
    for (char c : s) {
        if (!esc) {
            if (c == '\\') esc = true;
            else out.push_back(c);
        } else {
            out.push_back(c);
            esc = false;
        }
    }
    if (esc) out.push_back('\\');
    return out;
}

vector<string> split_escaped_pipe(const string& line) {
    vector<string> out;
    string cur;
    cur.reserve(line.size());
    bool esc = false;

    for (char c : line) {
        if (!esc) {
            if (c == '\\') {
                esc = true;
                cur.push_back(c);
            } else if (c == '|') {
                out.push_back(cur);
                cur.clear();
            } else {
                cur.push_back(c);
            }
        } else {
            cur.push_back(c);
            esc = false;
        }
    }
    out.push_back(cur);
    return out;
}
