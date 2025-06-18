#include "logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace {
std::ofstream log_file;
std::mutex log_mutex;
}

void log(const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex);
    
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] " << message;
    
    // Output to console
    std::cout << ss.str() << std::endl;
    
    // Output to file
    if (!log_file.is_open()) {
        log_file.open("logs/market_maker.log", std::ios::app);
    }
    
    if (log_file.is_open()) {
        log_file << ss.str() << std::endl;
        log_file.flush();
    }
}