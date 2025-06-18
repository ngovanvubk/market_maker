#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <cpprest/json.h>
#include <cpprest/http_client.h>
#include <cpprest/uri.h>

// Common type definitions
using Clock = std::chrono::high_resolution_clock;

// Common constants
const double SPREAD = 0.001;  // 0.1% spread
const double ORDER_QUANTITY = 0.01;  // Fixed order quantity

// Utility function declarations
void log(const std::string& message);