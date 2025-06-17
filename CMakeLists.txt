cmake_minimum_required(VERSION 3.10)
project(MarketMaker LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(websocketpp CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Source files
set(SOURCES
    src/main.cpp
    src/exchange/binance_api.cpp
    src/order/order_manager.cpp
    src/utils/logger.cpp
    src/utils/crypto.cpp
)

# Create executable
add_executable(market_maker ${SOURCES})

# Link libraries
target_link_libraries(market_maker
    PRIVATE
    websocketpp::websocketpp
    cpprestsdk::cpprest
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread
)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Enable warnings and optimizations
target_compile_options(market_maker PRIVATE -Wall -Wextra -O2)