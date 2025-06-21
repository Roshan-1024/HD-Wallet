#include "config/coin_config.h"
#include "bip39/bip39.h"
#include "bip32/bip32.h"
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <unordered_map>

int main(){
	///////////////////////////////////////////////////////////////////////
	//				BIP-39
	///////////////////////////////////////////////////////////////////////
	int ENT = getEntropySizeFromUserChoice();
	validateENT(ENT); // Redundant (currently)

	std::vector<unsigned char> entropy = generateEntropy(ENT);
	std::vector<unsigned char> hash = sha256_raw(entropy);
	std::string mnemonic = generateMnemonic(ENT, entropy, hash);
	std::vector<unsigned char> seed = generateSeed(mnemonic);

	std::cout << "Mnemonic = " << mnemonic << '\n';

	std::cout << "Seed = ";
	for(auto byte : seed)
		printf("%02x", byte);
	std::cout << '\n';

	///////////////////////////////////////////////////////////////////////
	//				BIP-32
	///////////////////////////////////////////////////////////////////////
	std::vector<unsigned char> seed_key = {'B','i','t','c','o','i','n',' ','s','e','e','d'};
	std::vector<unsigned char> I = hmac_sha512(seed_key, seed);
	std::vector<unsigned char> master_private_key(I.begin(), I.begin()+32);
	std::vector<unsigned char> master_chain_code(I.begin()+32, I.end());

	std::cout << "master private key = ";
	printHex(master_private_key);
	std::cout << "master chain code =  ";
	printHex(master_chain_code);
}
