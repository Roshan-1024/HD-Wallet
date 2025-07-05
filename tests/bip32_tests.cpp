#include "../bip32/bip32.h"
#include "../utils/utils.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <cstdint>
#include <stdexcept>
#include <gtest/gtest.h>
using json = nlohmann::json;


class BIP32Test : public testing::Test{
protected:
	static json vectors;
	static void SetUpTestSuite(){
		std::ifstream file(BIP32_TEST_VECTORS_PATH);
		if(!file){
			throw std::runtime_error("Error opening " + std::string(BIP32_TEST_VECTORS_PATH));
		}
		file >> vectors;
	}
};
json BIP32Test::vectors;

std::pair<std::vector<unsigned char>, std::vector<unsigned char>> deriveMasterKeys(const json& vector){
	std::vector<unsigned char> seed = hexToBytes(vector["seed"].get<std::string>());
	std::vector<unsigned char> seed_key = {'B','i','t','c','o','i','n',' ','s','e','e','d'};
	std::vector<unsigned char> I = hmac_sha512(seed_key, seed);
	std::vector<unsigned char> master_private_key(I.begin(), I.begin()+32);
	std::vector<unsigned char> master_chain_code(I.begin()+32, I.end());
	return {master_private_key, master_chain_code};
}

std::pair<std::vector<unsigned char>, std::vector<unsigned char>> decodeBase58XprvXpub(const std::string& xprv, const std::string& xpub){
	std::vector<unsigned char> decoded_xprv = base58CheckDecode(xprv);
	std::vector<unsigned char> decoded_xpub = base58CheckDecode(xpub);
	
	if(decoded_xprv.size() != 78){
		throw std::runtime_error("Invalid decoded xprv size");
	}
	if(decoded_xpub.size() != 78){
		throw std::runtime_error("Invalid decoded xpub size");
	}
	std::vector<unsigned char> private_key_(decoded_xprv.begin()+46, decoded_xprv.begin()+78);
	std::vector<unsigned char> public_key_(decoded_xpub.begin()+45, decoded_xpub.begin()+78);
	return {private_key_, public_key_};
}

TEST_F(BIP32Test, ValidateBIP32Vectors){
	int test_num = 1;

	for(const auto& vector : vectors){
		std::cout << "--- Running Test Vector #" << test_num << " ---\n";

		std::string derivationPath = vector["path"].get<std::string>();
		std::vector<uint32_t> parsedPath = parsePath(derivationPath);
		
		std::cout << "Path: " << derivationPath << '\n';
		//////////////////////////////////////////////////////////////////////
		//		TEST MASTER PRIVATE AND PUBLIC KEYS
		//////////////////////////////////////////////////////////////////////
		auto [master_private_key, master_chain_code] = deriveMasterKeys(vector);
		std::vector<unsigned char> master_public_key = getPublicKeyFromPrivateKey(master_private_key, /*toCompress*/true);

		std::string xprv = vector["keys"]["m"][0].get<std::string>();
		std::string xpub = vector["keys"]["m"][1].get<std::string>();
		auto [master_private_key_, master_public_key_] = decodeBase58XprvXpub(xprv, xpub);
		ASSERT_EQ(master_private_key, master_private_key_);
		std::cout << "✓ Master Private Key matched\n";
		ASSERT_EQ(master_public_key, master_public_key_);
		std::cout << "✓ Master Public Key matched\n";

		//////////////////////////////////////////////////////////////////////
		//		TEST CHILD PRIVATE AND PUBLIC KEYS
		//////////////////////////////////////////////////////////////////////
		std::vector<unsigned char> child_private_key = master_private_key;
		std::vector<unsigned char> child_chain_code = master_chain_code;

		std::string fullPath = "m";
		for(const uint32_t& index : parsedPath){
			auto [private_key, chain_code] = CKD_priv(child_private_key, child_chain_code, index);
			child_private_key = std::move(private_key);
			child_chain_code = std::move(chain_code);
			std::vector<unsigned char> child_public_key = getPublicKeyFromPrivateKey(child_private_key, /*toCompress=*/true);

			std::string node;
			if(index >= 0x80000000){ // hardened
				node = std::to_string(index ^ 0x80000000) + "'";
			}
			else{ // non-hardened
				node = std::to_string(index);
			}
			fullPath += "/" + node;

			xprv = vector["keys"][node][0].get<std::string>();
			xpub = vector["keys"][node][1].get<std::string>();
			auto [child_private_key_, child_public_key_] = decodeBase58XprvXpub(xprv, xpub);
			ASSERT_EQ(child_private_key, child_private_key_);
			std::cout << "✓ Child Private Key matched at node " << fullPath << '\n';
			ASSERT_EQ(child_public_key, child_public_key_);
			std::cout << "✓ Child Public Key matched at node " << fullPath << '\n';
		}

		std::cout << "--- Test Vector #" << test_num << " PASSED ---\n\n";
		test_num++;
	}
	std::cout << "\n=== All " << test_num - 1 << " test vectors passed! ===\n";
}
