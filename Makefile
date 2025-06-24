# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./bip39 -I./bip32 -I./config -I./utils -I./address -I./keccak

# Output binary
TARGET = wallet

# Source files
SRC = main.cpp bip39/bip39.cpp bip32/bip32.cpp utils/utils.cpp address/address_generator.cpp keccak/keccak.cpp

# OpenSSL flags
LDLIBS = -lssl -lcrypto

# Default target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

# Clean build artifacts
clean:
	rm -f $(TARGET)
