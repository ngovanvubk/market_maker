
#include "exchange/binance_api.hpp"
#include "order/order_manager.hpp"
#include <csignal>

volatile sig_atomic_t running = true;

void signal_handler(int signal) {
    running = false;
}

int main() {
    // Set up signal handling
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    try {
        // Initialize WebSocket connection
        exchange::init_websocket();

        // Main loop
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        std::cout << "Shutting down market maker..." << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}


