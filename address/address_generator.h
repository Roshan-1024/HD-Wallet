#ifndef ADDRESS_GENERATOR_H
#define ADDRESS_GENERATOR_H

#include <vector>

std::vector<unsigned char> generateBitcoinStyleAddress(const std::vector<unsigned char>& final_public_key);
std::vector<unsigned char> generateEVMStyleAddress(const std::vector<unsigned char>& final_public_key);
std::vector<unsigned char> generateTronStyleAddress(const std::vector<unsigned char>& final_public_key);

#endif
