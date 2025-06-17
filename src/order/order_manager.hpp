#pragma once
#include <mutex>

namespace order
{
    extern std::mutex order_book_mutex;
    extern double best_bid, best_ask, mid_price;
    extern std::string bid_order_id, ask_order_id;

    void update_orders(double new_mid_price);
}