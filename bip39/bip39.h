#ifndef BIP39_H
#define BIP39_H

#include <vector>
#include <string>

std::vector<unsigned char> generateEntropy(int ENT);
void validateENT(int& ENT);
std::string generateMnemonic(int& ENT, std::vector<unsigned char>& entropy, std::vector<unsigned char>& hash);
std::vector<unsigned char> generateSeed(const std::string& mnemonic, const std::string& passphrase = "");
std::vector<unsigned char> sha256_raw(const std::vector<unsigned char>& data);

#endif
