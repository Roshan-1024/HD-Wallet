#include "utils.h"
#include "../config/coin_config.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdint>
#include <unordered_map>
#include <openssl/bn.h>
#include <vector>

int getEntropySizeFromUserChoice(){
	std::unordered_map<int, int> MS_Map = {
		{1, 12},
		{2, 15},
		{3, 18},
		{4, 21},
		{5, 24},
	};

	int choice;
	std::cout << "Choose the number of words for your Mnemonics:\n"
		  << "1. 12 Words\n"
		  << "2. 15 Words\n"
		  << "3. 18 Words\n"
		  << "4. 21 Words\n"
		  << "5. 24 Words\n"
		  << "Choice: ";
	std::cin >> choice;
	std::cout << '\n';

	while(MS_Map.count(choice) != 1){
		std::cout << "Invalid choice. Try again: ";
		std::cin >> choice;
	}

	int MS = MS_Map[choice];
	int ENT = (32 * MS) / 3; // ENT in bits
	validateENT(ENT); // Redundant (currently)
	return ENT;
}


CoinType getCoinTypeFromUserChoice(){
	std::unordered_map<int, CoinType> coinMap = {
		{1, CoinType::Bitcoin},
		{2, CoinType::Ethereum},
		{3, CoinType::BSC},
		{4, CoinType::Polygon},
		{5, CoinType::Tron},
	};

	int choice;
	std::cout << "Choose the Coin type:\n"
		  << "1. Bitcoin\n"
		  << "2. Ethereum\n"
		  << "3. BSC\n"
		  << "4. Polygon\n"
		  << "5. Tron\n"
		  << "Choice: ";
	std::cin >> choice;
	std::cout << '\n';

	while(coinMap.count(choice) != 1){
		std::cout << "Invalid choice. Try again: ";
		std::cin >> choice;
	}

	return coinMap[choice];
}

void printHex(const std::vector<unsigned char>& data) {
    for (unsigned char c : data)
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)c;
    std::cout << std::dec << std::endl;
}

// Serialize a 32 bit unsigned int in big-endian byte order
std::vector<unsigned char> ser32_be(uint32_t index){
	return {
		static_cast<unsigned char>((index >> 24) & 0xFF),
		static_cast<unsigned char>((index >> 16) & 0xFF),
		static_cast<unsigned char>((index >> 8) & 0xFF),
		static_cast<unsigned char>((index >> 0) & 0xFF)
	};
}

std::vector<unsigned char> ser256_be(const BIGNUM* bn){
	std::vector<unsigned char> out(32); // 256 bits = 32 bytes
	int written = BN_bn2binpad(bn, out.data(), 32);

	if(written != 32){
		throw std::runtime_error("BIGNUM serialization failed in ser256 function");
	}

	return out;
}
