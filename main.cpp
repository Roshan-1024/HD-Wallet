#include "utils/utils.h"
#include "bip39/bip39.h"
#include "bip32/bip32.h"
#include <iostream>
#include <cstdio>
#include <cstdint>

int main(){
	///////////////////////////////////////////////////////////////////////
	//				BIP-39
	///////////////////////////////////////////////////////////////////////
	int ENT = getEntropySizeFromUserChoice();
	CoinType coin = getCoinTypeFromUserChoice();

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
	std::cout << "master chain code  = ";
	printHex(master_chain_code);

	std::string derivationPath = getDerivationPath(coin);
	std::vector<uint32_t> parsedPath = parsePath(derivationPath);

	std::vector<unsigned char> final_private_key = master_private_key;
	std::vector<unsigned char> final_chain_code = master_chain_code;
	for(uint32_t index : parsedPath){
		auto [private_key, chain_code] = CKD_priv(final_private_key, final_chain_code, index);
		final_private_key = std::move(private_key);
		final_chain_code = std::move(chain_code);
	}

	std::vector<unsigned char> final_public_key = getPublicKeyFromPrivateKey(final_private_key, requiresCompressedPublicKey(coin));
	std::vector<unsigned char> address = getAddress(final_public_key, coin);

	std::cout << "Final chain code   = ";
	printHex(final_chain_code);
	std::cout << "Final Public Key   = ";
	printHex(final_public_key);
	std::cout << "Final Private Key  = ";
	printHex(final_private_key);
	std::cout << "Address = ";
	printHex(address);
}
