cmake_minimum_required(VERSION 3.14)
project(wallet VERSION 1.0 LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directory for the binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/bip39
    ${PROJECT_SOURCE_DIR}/bip32
    ${PROJECT_SOURCE_DIR}/config
    ${PROJECT_SOURCE_DIR}/utils
    ${PROJECT_SOURCE_DIR}/address
    ${PROJECT_SOURCE_DIR}/keccak
)

# Source files
set(SOURCES
    main.cpp
    bip39/bip39.cpp
    bip32/bip32.cpp
    utils/utils.cpp
    address/address_generator.cpp
    keccak/keccak.cpp
)

# Define the executable
add_executable(wallet ${SOURCES})

# Find and link OpenSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(wallet PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Show all warnings
target_compile_options(wallet PRIVATE -Wall -Wextra -pedantic)
