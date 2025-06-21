#ifndef COIN_CONFIG_H
#define COIN_CONFIG_H

#include <string>
#include <stdexcept>
#include <unordered_map>
#include <iostream>

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

CoinType getCoinTypeFromUserChoice(){
	std::unordered_map<int, CoinType> coinMap = {
		{1, CoinType::Bitcoin},
		{2, CoinType::Ethereum},
		{3, CoinType::BSC},
		{4, CoinType::Polygon},
		{5, CoinType::Tron},
	};

	int choice;
	std::cout << "Choose the Coin type:\n";
	std::cout << "1. Bitcoin\n";
	std::cout << "2. Ethereum\n";
	std::cout << "3. BSC\n";
	std::cout << "4. Polygon\n";
	std::cout << "5. Tron\n";
	std::cout << "Choice: ";
	std::cin >> choice;

	while (coinMap.count(choice) != 1) {
		std::cout << "Invalid choice. Try again: ";
		std::cin >> choice;
	}

	return coinMap[choice];
}

#endif
