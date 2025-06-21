#include "bip32.h"
#include <openssl/hmac.h>
#include <iostream>
#include <iomanip>
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

void printHex(const std::vector<unsigned char>& data) {
    for (unsigned char c : data)
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)c;
    std::cout << std::dec << std::endl;
}

std::vector<uint32_t> parsePath(std::string path){
	std::vector<uint32_t> parsedPath;

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
