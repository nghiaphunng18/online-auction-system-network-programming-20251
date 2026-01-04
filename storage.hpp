#pragma once
#include "model.hpp"
#include <string>
#include <unordered_map>
#include <vector>

struct DraftKey {
    int room_id;
    int seller_id;
};
struct DraftKeyHash {
    size_t operator()(DraftKey const& k) const noexcept;
};
struct DraftKeyEq {
    bool operator()(DraftKey const& a, DraftKey const& b) const noexcept;
};

struct PersistState {
    std::unordered_map<int, User> users;
    std::unordered_map<std::string, int> user_by_name;
    std::unordered_map<int, Room> rooms;
    std::unordered_map<int, Item> items;
    std::vector<HistoryRow> history;
    std::unordered_map<DraftKey, std::vector<DraftItem>, DraftKeyHash, DraftKeyEq> drafts;

    int next_user_id = 1;
    int next_room_id = 1;
    int next_item_id = 1;
};

bool save_state_file(const std::string& path, const PersistState& st);
bool load_state_file(const std::string& path, PersistState& st, std::string& err);
