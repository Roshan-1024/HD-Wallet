#include "bip39.h"
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
#include <cstdlib>

#define FILE_NAME "wordlists/english.txt"

std::vector<unsigned char> generateSeed(const std::string& mnemonic, const std::string& passphrase){
	std::string salt = "mnemonic" + passphrase;
	std::vector<unsigned char> seed(64); // 512 bits = 64 bytes

	int val = PKCS5_PBKDF2_HMAC(
		mnemonic.c_str(), mnemonic.length(),
		reinterpret_cast<const unsigned char*>(salt.c_str()), salt.length(),
		2048,
		EVP_sha512(),
		64,
		seed.data()
	);
	if(val == 0){
		std::cout << "Error generating seed.\n";
		exit(0);
	}

	return seed;
}
// Converts a hex string to a vector of unsigned char (bytes)
std::vector<unsigned char> hexStringToBytes(const std::string& hex) {
	std::vector<unsigned char> bytes;
	for (size_t i = 0; i < hex.length(); i += 2) {
		std::string byteString = hex.substr(i, 2);
		unsigned char byte = (unsigned char) strtol(byteString.c_str(), nullptr, 16);
		bytes.push_back(byte);
	}
	return bytes;
}
std::vector<unsigned char> sha256_raw(const std::vector<unsigned char>& data) {
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256(data.data(), data.size(), hash);

	return std::vector<unsigned char>(hash, hash + SHA256_DIGEST_LENGTH);
}

std::string getLine(int n){
	std::ifstream file(FILE_NAME);
	if(!file){
		std::cout << "File could not be opened.\n";
		exit(0);
	}

	std::string word;
	int currentLine = 0;
	while(std::getline(file, word)){
		if(currentLine == n){
			file.close();
			return word;
		}
		currentLine++;
	}
	return word;
}

int toInt(const std::string& bits){
	return std::stoi(bits, nullptr, 2);
}

std::vector<unsigned char> generateEntropy(int ENT){
	int byte_len = ENT/8;
	std::vector<unsigned char> entropy(byte_len);
	if (!RAND_bytes(entropy.data(), byte_len)) {
		std::cerr << "Error generating secure random bytes.\n";
		exit(0);
	}
	return entropy;
}

void validateENT(int& ENT){
	if(ENT != 128 && ENT != 160 && ENT != 192 && ENT != 224 && ENT != 256){
		std::cerr << "Invalid ENT length.\n";
		exit(0);
	}
}

std::string generateMnemonic(int& ENT, std::vector<unsigned char>& entropy, std::vector<unsigned char>& hash){
	int CS = ENT/32;
	std::string hashBits;
	for(auto byte : hash)
		hashBits += std::bitset<8>(byte).to_string();
	std::string checkSum = hashBits.substr(0, CS);

	std::string entropyBits;
	for (auto byte : entropy)
	    entropyBits += std::bitset<8>(byte).to_string();

	std::string entropyWithCheckSum = entropyBits + checkSum;

	std::string mnemonic;
	for(size_t i = 0; i < entropyWithCheckSum.length(); i += 11){
		int row = toInt(entropyWithCheckSum.substr(i, 11));
		mnemonic += getLine(row);
		mnemonic += ' ';
	}
	mnemonic.pop_back(); // Remove trailing space character

	return mnemonic;
}
