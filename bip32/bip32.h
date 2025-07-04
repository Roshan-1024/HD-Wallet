#ifndef BIP32_H
#define BIP32_H

#include "../utils/utils.h"
#include <vector>
#include <string>
#include <cstdint>

//////////////////////////////////////////////////////////////////////////////////
//				Hashing Functions
//////////////////////////////////////////////////////////////////////////////////
std::vector<unsigned char> hmac_sha512(const std::vector<unsigned char>& key, const std::vector<unsigned char>& message);

//////////////////////////////////////////////////////////////////////////////////
//				BIP-32 Path Utilities
//////////////////////////////////////////////////////////////////////////////////
std::vector<uint32_t> parsePath(std::string path);

//////////////////////////////////////////////////////////////////////////////////
//				Child Key Derivation (Private)
//////////////////////////////////////////////////////////////////////////////////
std::pair<std::vector<unsigned char>, std::vector<unsigned char>> CKD_priv(
	const std::vector<unsigned char>& parent_private_key,
	const std::vector<unsigned char>& parent_chain_code,
	uint32_t index);

//////////////////////////////////////////////////////////////////////////////////
//				Generate Public Key
//////////////////////////////////////////////////////////////////////////////////
std::vector<unsigned char> getPublicKeyFromPrivateKey(const std::vector<unsigned char>& private_key, bool toCompress);

//////////////////////////////////////////////////////////////////////////////////
//				Generate Public Key
//////////////////////////////////////////////////////////////////////////////////
std::vector<unsigned char> getAddress(const std::vector<unsigned char>& final_public_key, CoinType coin);
#endif
