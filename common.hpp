#pragma once
#include <string>
#include <vector>

long long now_ms();
std::string trim(std::string s);
std::vector<std::string> split_ws(const std::string& s);
std::vector<std::string> split_char(const std::string& s, char delim);
bool icontains(const std::string& a, const std::string& b);

bool set_nonblock(int fd);
bool parse_int(const std::string& s, int& out);

std::string mmss(long long ms_left);

// ----- persistence helpers (escape) -----
std::string esc_field(const std::string& s);
std::string unesc_field(const std::string& s);
std::vector<std::string> split_escaped_pipe(const std::string& line);
