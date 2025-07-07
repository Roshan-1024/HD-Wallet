#include "test_utils.h"
#include "../utils/utils.h"
#include "../bip32/bip32.h"
using json = nlohmann::json;

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
