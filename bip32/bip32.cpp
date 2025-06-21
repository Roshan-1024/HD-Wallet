#include "utils.h"
#include "bip32.h"
#include <openssl/hmac.h>
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <cstdint>
#include <regex>

std::vector<unsigned char> hmac_sha512(const std::vector<unsigned char>& key,
				       const std::vector<unsigned char>& message){
	unsigned int len = 64; // SHA-512 outputs 64 bytes
	std::vector<unsigned char> hmac_result(len);

	HMAC(
	    EVP_sha512(),                     // Use SHA-512
	    key.data(), key.size(),           // HMAC key
	    message.data(), message.size(),   // message (the seed)
	    hmac_result.data(), &len          // output
	);

	return hmac_result;
}

std::vector<uint32_t> parsePath(const std::string path){
	std::vector<uint32_t> parsedPath;

	// Match both hardened (e.g., /44') and non-hardened (e.g., /0) path components
	// Group 1: index number, Group 2: optional apostrophe indicating hardened
	std::regex re(R"(/([0-9]+)(')?)");
	std::sregex_iterator it(path.begin(), path.end(), re);
	std::sregex_iterator end;

	while(it != end){
		uint32_t num = static_cast<uint32_t>(std::stoul((*it)[1]));

		bool isHardened = (*it)[2].matched;
		if(isHardened){
			num |= 0x80000000;
		}
		parsedPath.push_back(num);
		it++;
	}
	return parsedPath;
}

std::pair<std::vector<unsigned char>, std::vector<unsigned char>> CKD_priv(
	const std::vector<unsigned char>& parent_private_key,
	const std::vector<unsigned char>& parent_chain_code,
	uint32_t index){

	std::vector<unsigned char> data;

	// Serialize the 32 bit (4 bytes) index in big-endian byte order
	std::vector<unsigned char> serialized_be_index = ser32_be(index);

	EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_secp256k1);
	BN_CTX* ctx = BN_CTX_new();
	if(index & 0x80000000){ // Hardened
		// data = 0x00 || ser_256(parent_private_key) || ser_32(index) as per BIP32
		data = {0x00};

		// Serialize the 256 bit (32 bytes) parent_private_key in big-endian byte order
		if (parent_private_key.size() != 32)	// Assert that it's already serialized
			throw std::invalid_argument("Parent private key must be 32 bytes");
		const auto& serialized_be_parent_private_key = parent_private_key;

		data.insert(data.end(), serialized_be_parent_private_key.begin(), serialized_be_parent_private_key.end());
	}
	else{ // Non-hardened
		// Convert parent private key to BIGNUM
		BIGNUM* parent_private_key_bn = BN_new();
		BN_bin2bn(parent_private_key.data(), parent_private_key.size(), parent_private_key_bn);

		// parent public key = parent_private_key_bn * G
		EC_POINT* parent_public_key_point = EC_POINT_new(group);
		EC_POINT_mul(group, parent_public_key_point, parent_private_key_bn, nullptr, nullptr, ctx);

		// Serialize the point using compressed format
		std::vector<unsigned char> parent_public_key(33);
		EC_POINT_point2oct(group, parent_public_key_point, POINT_CONVERSION_COMPRESSED, parent_public_key.data(), parent_public_key.size(), ctx);

		// data = ser_p(point(parent_private_key)) + ser_32(index)
		data = parent_public_key;

		BN_free(parent_private_key_bn);
		EC_POINT_free(parent_public_key_point);
	}

	data.insert(data.end(), serialized_be_index.begin(), serialized_be_index.end());

	std::vector<unsigned char> I = hmac_sha512(parent_chain_code, data);

	std::vector<unsigned char> IL(I.begin(), I.begin()+32);
	std::vector<unsigned char> IR(I.begin()+32, I.end());

	// Child chain code = IR
	const auto& child_chain_code = IR;

	// Convert IL and parent private key to BIGNUMs
	BIGNUM* IL_bn = BN_new();
	BIGNUM* parent_private_key_bn = BN_new();
	BN_bin2bn(IL.data(), IL.size(), IL_bn);
	BN_bin2bn(parent_private_key.data(), parent_private_key.size(), parent_private_key_bn);

	// Get curve order n
	BIGNUM* order = BN_new();
	EC_GROUP_get_order(group, order, nullptr);

	// child private key = (parse(IL) + parent private key) mod n
	BIGNUM* child_private_key_bn = BN_new();
	BN_mod_add(child_private_key_bn, IL_bn, parent_private_key_bn, order, ctx);

	// NOTE: In case parse256(IL) >= n or child private key = 0, then proceed with the next value of i
	if(BN_cmp(IL_bn, order) >= 0 || BN_is_zero(child_private_key_bn))
		throw std::runtime_error("Invalid child key generated");

	// Serialize child private key in big-endian byte order
	std::vector<unsigned char> serialized_be_child_private_key = ser256_be(child_private_key_bn);

	BN_free(IL_bn);
	BN_free(parent_private_key_bn);
	BN_free(child_private_key_bn);
	BN_free(order);
	EC_GROUP_free(group);
	BN_CTX_free(ctx);

	return {serialized_be_child_private_key, child_chain_code};
}
