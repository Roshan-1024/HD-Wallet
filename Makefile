# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./bip39

# Output binary
TARGET = wallet

# Source files
SRC = main.cpp bip39/bip39.cpp

# OpenSSL flags
LDLIBS = -lssl -lcrypto

# Default target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

# Clean build artifacts
clean:
	rm -f $(TARGET)

