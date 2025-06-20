#include "bip39/bip39.h"
#include <iostream>
#include <cstdio>

int main(){
	int ENT;
	std::cout << "Specify ENT length in bits (128, 160, 192, 224, 256): ";
	std::cin >> ENT;
	validateENT(ENT);

	std::vector<unsigned char> entropy = generateEntropy(ENT);
	std::vector<unsigned char> hash = sha256_raw(entropy);
	std::string mnemonic = generateMnemonic(ENT, entropy, hash);
	std::vector<unsigned char> seed = generateSeed(mnemonic);

	std::cout << "Mnemonic = " << mnemonic << '\n';

	std::cout << "Seed = ";
	for(auto byte : seed)
		printf("%02x", byte);
	std::cout << '\n';
}

