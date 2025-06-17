#include "logger.hpp"
#include "include/include.hpp"
// Logging function with timestamp
void log(const std::string &message)
{
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::lock_guard<std::mutex> lock(order_book_mutex);
    log_file << std::put_time(std::localtime(&now_c), "%F %T") << " " << message << std::endl;
    log_file.flush();
    std::cout << std::put_time(std::localtime(&now_c), "%F %T") << " " << message << std::endl;
}