#ifndef UTILS_H
#define UTILS_H

#include "../config/coin_config.h"
#include <iostream>
#include <vector>
#include <cstdint>
#include <openssl/bn.h>

////////////////////////////////////////////////////////
//			User Input Handling
////////////////////////////////////////////////////////
int getEntropySizeFromUserChoice();

CoinType getCoinTypeFromUserChoice();

////////////////////////////////////////////////////////
//			Input Validation
////////////////////////////////////////////////////////
inline void validateENT(int& ENT){
	if(ENT != 128 && ENT != 160 && ENT != 192 && ENT != 224 && ENT != 256){
		std::cerr << "Invalid ENT length.\n";
		exit(1);
	}
}

////////////////////////////////////////////////////////
//			Output Utilities
////////////////////////////////////////////////////////
void printHex(const std::vector<unsigned char>& data);

////////////////////////////////////////////////////////
//			Serialization functions
////////////////////////////////////////////////////////
std::vector<unsigned char> ser32_be(uint32_t index);

std::vector<unsigned char> ser256_be(const BIGNUM* bn);

////////////////////////////////////////////////////////
//			Coin-type utilities
////////////////////////////////////////////////////////
inline bool requiresCompressedPublicKey(CoinType coin){
	switch(coin){
		case CoinType::Bitcoin: return true;
		case CoinType::Ethereum: return false;
		case CoinType::BSC: return false;
		case CoinType::Polygon: return false;
		case CoinType::Tron: return false;
		default: throw std::runtime_error("Unsupported Coin Type");
	}
}
#endif
