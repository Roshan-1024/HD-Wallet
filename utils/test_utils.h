#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

std::pair<std::vector<unsigned char>, std::vector<unsigned char>> deriveMasterKeys(const nlohmann::json& vector);

std::pair<std::vector<unsigned char>, std::vector<unsigned char>> decodeBase58XprvXpub(const std::string& xprv, const std::string& xpub);

#endif
