#include "order_manager.hpp"
#include "exchange/binance_api.hpp"

namespace order {

// Initialize global variables
std::mutex order_book_mutex;
double best_bid = 0.0, best_ask = 0.0, mid_price = 0.0;
std::string bid_order_id, ask_order_id;

// Update orders based on new mid-price
void update_orders(double new_mid_price) {
    auto start = Clock::now();
    {
        std::lock_guard<std::mutex> lock(order_book_mutex);
        if (!bid_order_id.empty())
            exchange::cancel_order(bid_order_id);
        if (!ask_order_id.empty())
            exchange::cancel_order(ask_order_id);

        double bid_price = new_mid_price * (1.0 - SPREAD);
        double ask_price = new_mid_price * (1.0 + SPREAD);

        exchange::place_order("BUY", bid_price, ORDER_QUANTITY, bid_order_id);
        exchange::place_order("SELL", ask_price, ORDER_QUANTITY, ask_order_id);

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start).count();
        log("Order update latency: " + std::to_string(duration / 1000.0) + " ms");
    }
}

}  // namespace order
