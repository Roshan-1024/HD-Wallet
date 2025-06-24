#include "address_generator.h"
#include "../keccak/keccak.h"

std::vector<unsigned char> generateBitcoinStyleAddress(const std::vector<unsigned char>& final_public_key){
	return {};
}

std::vector<unsigned char> generateEVMStyleAddress(const std::vector<unsigned char>& final_public_key){
	std::vector<unsigned char> x_y(final_public_key.begin()+1, final_public_key.end());
	std::vector<unsigned char> hash = keccak256(x_y);
	std::vector<unsigned char> address(hash.end()-20, hash.end());
	return address;
}

std::vector<unsigned char> generateTronStyleAddress(const std::vector<unsigned char>& final_public_key){
	return {};
}
