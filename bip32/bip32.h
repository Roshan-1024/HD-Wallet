#ifndef BIP32_H
#define BIP32_H

#include <vector>
#include <string>
#include <cstdint>

std::vector<unsigned char> hmac_sha512(const std::vector<unsigned char>& key,
				       const std::vector<unsigned char>& message);
void printHex(const std::vector<unsigned char>& data);
std::vector<uint32_t> parsePath(std::string path);

#endif
