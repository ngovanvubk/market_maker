// HMAC-SHA256 signature for Binance API
#include "crypto.hpp"
#include "include.hpp"
std::string hmac_sha256(const std::string &key, const std::string &data)
{
    unsigned char *digest;
    digest = HMAC(EVP_sha256(), key.c_str(), key.length(),
                  reinterpret_cast<const unsigned char *>(data.c_str()), data.length(), nullptr, nullptr);
    char md_string[65];
    for (int i = 0; i < 32; i++)
    {
        sprintf(&md_string[i * 2], "%02x", (unsigned int)digest[i]);
    }
    return std::string(md_string);
}