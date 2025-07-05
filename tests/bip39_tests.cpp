#include "../bip39/bip39.h"
#include "../bip32/bip32.h"
#include "../utils/utils.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <gtest/gtest.h>
using json = nlohmann::json;


class BIP39Test : public testing::Test{
protected:
	static json j;
	static void SetUpTestSuite(){
		std::ifstream file(BIP39_TEST_VECTORS_PATH);
		if(!file){
			std::cerr << "Error opening BIP39_vectors.json file\n";
			std::cout << "TEST_VECTORS_PATH = " << BIP39_TEST_VECTORS_PATH << std::endl;
			exit(1);
		}
		file >> j;
	}
};
json BIP39Test::j;

TEST_F(BIP39Test, ValidateBIP39Vectors){
	auto en_vectors = j["english"];
	int test_num = 1;

	for(const auto& vector : en_vectors){
		std::cout << "--- Running Test Vector #" << test_num << " ---\n";

		std::vector<unsigned char> entropy_ = hexToBytes(vector[0].get<std::string>());
		std::string mnemonic_ = vector[1].get<std::string>();
		std::vector<unsigned char> seed_ = hexToBytes(vector[2].get<std::string>());
		std::string xprv = vector[3].get<std::string>();

		std::vector<unsigned char> decoded = base58CheckDecode(xprv);
		if(decoded.size() != 78){
			throw std::runtime_error("Invalid decoded xprv size");
		}

		std::vector<unsigned char> master_private_key_(decoded.begin()+46, decoded.begin()+78);
		std::vector<unsigned char> master_chain_code_(decoded.begin()+13, decoded.begin()+45);

		/////////////////////////////////////////////////////////////////////////////////////////////
		//                                      TESTS
		/////////////////////////////////////////////////////////////////////////////////////////////
		// 1. Mnemonic
		int ENT = entropy_.size() * 8;
		std::vector<unsigned char> hash = sha256_raw(entropy_);
		std::string mnemonic = generateMnemonic(ENT, entropy_, hash);
		ASSERT_EQ(mnemonic, mnemonic_);
		std::cout << "✓ Mnemonic matched\n";

		// 2. Seed
		std::vector<unsigned char> seed = generateSeed(mnemonic, "TREZOR");
		ASSERT_EQ(seed, seed_);
		std::cout << "✓ Seed matched\n";

		// 3. Master Private Key
		std::vector<unsigned char> seed_key = {'B','i','t','c','o','i','n',' ','s','e','e','d'};
		std::vector<unsigned char> I = hmac_sha512(seed_key, seed);
		std::vector<unsigned char> master_private_key(I.begin(), I.begin()+32);
		ASSERT_EQ(master_private_key, master_private_key_);
		std::cout << "✓ Master private key matched\n";

		// 4. Master Chain Code
		std::vector<unsigned char> master_chain_code(I.begin()+32, I.end());
		ASSERT_EQ(master_chain_code, master_chain_code_);
		std::cout << "✓ Master chain code matched\n";

		std::cout << "--- Test Vector #" << test_num << " PASSED ---\n\n";
		test_num++;
	}
	std::cout << "\n=== All " << test_num - 1 << " test vectors passed! ===\n";
}
