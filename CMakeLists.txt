set(CMAKE_TOOLCHAIN_FILE "D:/vcpkg/scripts/buildsystems/vcpkg.cmake")
cmake_minimum_required(VERSION 3.10)
project(MarketMaker LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(websocketpp CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)

# Source files
set(SOURCES
    src/main.cpp
    src/exchange/binance_api.cpp
    src/order/order_manager.cpp
    src/utils/logger.cpp
    src/utils/crypto.cpp
    src/include.hpp
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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Only add /O2 for Release builds to avoid conflict with /RTC1 in Debug
if(MSVC)
    target_compile_options(market_maker PRIVATE $<$<CONFIG:Release>:/O2> /W4)
else()
    target_compile_options(market_maker PRIVATE /W4)
endif()