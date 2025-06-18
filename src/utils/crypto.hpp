#pragma once
#include <string>
#include "include.hpp"
namespace utils
{
    std::string hmac_sha256(const std::string &key, const std::string &data);
}