#include <openssl/sha.h>
#include <openssl/rand.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <sstream>
#include <vector>
#include <bitset>

#define FILE_NAME "wordlist.txt"

std::vector<unsigned char> sha256_raw(const std::vector<unsigned char>& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(data.data(), data.size(), hash);

    return std::vector<unsigned char>(hash, hash + SHA256_DIGEST_LENGTH);
}

std::string getLine(int n){
	std::ifstream file(FILE_NAME);
	if(!file){
		std::cout << "File could not be opened.\n";
		exit(0);
	}

	std::string word;
	int currentLine = 0;
	while(std::getline(file, word)){
		if(currentLine == n){
			file.close();
			return word;
		}
		currentLine++;
	}
	return word;
}

int toInt(const std::string& bits){
	return std::stoi(bits, nullptr, 2);
}

std::vector<unsigned char> generateEntropy(int ENT){
	int byte_len = ENT/8;
	std::vector<unsigned char> entropy(byte_len);
	if (!RAND_bytes(entropy.data(), byte_len)) {
		std::cerr << "Error generating secure random bytes.\n";
		exit(0);
	}
	return entropy;
}

void validateENT(int& ENT){
	if(ENT != 128 && ENT != 160 && ENT != 192 && ENT != 224 && ENT != 256){
		std::cerr << "Invalid ENT length.\n";
		exit(0);
	}
}

int main(){
	std::string words;

	int ENT;
	std::cout << "Specify ENT length in bits (128, 160, 192, 224, 256): ";
	std::cin >> ENT;
	validateENT(ENT);

	std::vector<unsigned char> entropy = generateEntropy(ENT);
	std::vector<unsigned char> hash = sha256_raw(entropy);

	int CS = ENT/32;
	std::string hashBits;
	for(auto byte : hash){
		hashBits += std::bitset<8>(byte).to_string();
	}
	std::string checkSum = hashBits.substr(0, CS);

	std::string entropyBits;
	for (auto byte : entropy)
	    entropyBits += std::bitset<8>(byte).to_string();

	std::string entropyWithCheckSum = entropyBits + checkSum;

	for(int i = 0; i < entropyWithCheckSum.length(); i += 11){
		int row = toInt(entropyWithCheckSum.substr(i, 11));
		words += getLine(row);
		if(i != entropyWithCheckSum.length()-1){
			words += ' ';
		}
	}

	std::cout << words << '\n';
}
