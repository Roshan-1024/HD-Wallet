#ifndef COIN_CONFIG_H
#define COIN_CONFIG_H

#include <string>
#include <stdexcept>

enum class CoinType{
	Bitcoin,
	Ethereum,
	BSC,
	Polygon,
	Tron
};

// Get the Derivation Path of each coin as per BIP44.
inline std::string getDerivationPath(CoinType coin){
	switch(coin){
		case CoinType::Bitcoin:
			return "m/44'/0'/0'/0/0";
		case CoinType::Ethereum:
		case CoinType::BSC:
			return "m/44'/60'/0'/0/0";
		case CoinType::Polygon:
			return "m/44'/137'/0'/0/0";
		case CoinType::Tron:
			return "m/44'/195'/0'/0/0";
		default:
			throw std::runtime_error("Unsupported Coin Type");
	}
}

// Get Coin Name for logs.
inline std::string getCoinName(CoinType coin){
	switch(coin){
		case CoinType::Bitcoin: return "Bitcoin";
		case CoinType::Ethereum: return "Ethereum";
		case CoinType::BSC: return "BSC";
		case CoinType::Polygon: return "Polygon";
		case CoinType::Tron: return "Tron";
		default: throw std::runtime_error("Unsupported Coin Type");
	}
}

#endif
