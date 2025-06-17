cmake_minimum_required(VERSION 3.10) project(market_maker LANGUAGES CXX)

Set C++ standard

set(CMAKE_CXX_STANDARD 17) set(CMAKE_CXX_STANDARD_REQUIRED ON) set(CMAKE_CXX_EXTENSIONS OFF)

Define directories

set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src") set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include") set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib") set(BUILD_DIR "${CMAKE_SOURCE_DIR}/build")

Include directories

include_directories( ${SOURCE_DIR} ${INCLUDE_DIR} ${INCLUDE_DIR}/websocketpp ${INCLUDE_DIR}/cpprest ${INCLUDE_DIR}/openssl )

Collect all source files

file(GLOB_RECURSIVE SOURCES "${SOURCE_DIR}/*.cpp" )

Create executable

add_executable(market_maker ${SOURCES})

Find and link OpenSSL

find_package(OpenSSL REQUIRED) if (OPENSSL_FOUND) target_include_directories(market_maker PRIVATE ${OPENSSL_INCLUDE_DIR}) target_link_libraries(market_maker PRIVATE OpenSSL::SSL OpenSSL::Crypto) endif()

Find cpprestsdk

find_library(CPPREST_LIBRARY NAMES cpprest libcpprest PATHS ${LIB_DIR} ) if (CPPREST_LIBRARY) target_link_libraries(market_maker PRIVATE ${CPPREST_LIBRARY}) else() message(FATAL_ERROR "cpprestsdk library not found in ${LIB_DIR}") endif()

Since websocketpp is header-only, no library linking is needed

Just ensure include directory is added above

Platform-specific linking

if (WIN32) target_link_libraries(market_maker PRIVATE ws2_32 wsock32) endif()

Set output directories

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}) set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR}) set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})

Enable warnings

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|MSVC") target_compile_options(market_maker PRIVATE $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wpedantic> $<$<CXX_COMPILER_ID:MSVC>:/W4> ) endif()

Optional: Enable debug symbols

if (CMAKE_BUILD_TYPE MATCHES Debug) target_compile_options(market_maker PRIVATE -g) endif()