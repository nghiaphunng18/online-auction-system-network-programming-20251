#pragma once
#include <deque>
#include <string>
#include <unordered_set>
#include <vector>

// Room lifecycle:
// - PENDING: owner is preparing/publishing items (or waiting to START)
// - STARTED: auction is running (there may or may not be a current item)
// - ENDED  : room is closed forever (not listable, not joinable)
enum class RoomStatus { PENDING = 0, STARTED = 1, ENDED = 2 };
enum class ItemState  { QUEUED = 0, RUNNING = 1, SOLD = 2, EXPIRED = 3 };

struct User {
    int id = 0;
    std::string username;
    std::string password;
    std::string role = "BOTH";
    int current_room_id = 0; // runtime only; when load -> reset 0
};

struct DraftItem {
    std::string draft_id; // D1, D2...
    int room_id = 0;
    int seller_user_id = 0;
    std::string name;
    int start_price = 0;
    int buy_now_price = 0;
    int duration_sec = 0;
};

struct Item {
    int id = 0;
    int room_id = 0;
    int seller_user_id = 0;
    std::string name;
    int start_price = 0;
    int buy_now_price = 0;
    int duration_sec = 0;

    // runtime
    ItemState state = ItemState::QUEUED;
    int current_price = 0;
    int leader_user_id = 0;
    long long start_time_ms = 0;
    long long end_time_ms = 0;
    bool alert_30_sent = false;

    // result
    int winner_user_id = 0;
    int final_price = 0;
    std::string reason; // NORMAL_END / BUY_NOW / EXPIRED
    long long end_at_ms = 0;
};

struct Room {
    int id = 0;
    std::string name;
    int owner_user_id = 0;
    RoomStatus status = RoomStatus::PENDING;

    // runtime: participants only for connected users; when load -> empty
    std::unordered_set<int> participants;

    std::deque<int> queue; // item_ids
    int current_item_id = 0;
};

struct HistoryRow {
    int item_id = 0;
    int room_id = 0;
    int seller_user_id = 0;
    int winner_user_id = 0;
    int final_price = 0;
    std::string reason;
    long long end_at_ms = 0;
};
