cmake_minimum_required(VERSION 3.14)
project(wallet VERSION 0.1.0 LANGUAGES CXX)

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
target_compile_definitions(wallet PRIVATE
    EN_WORDLISTS_PATH="${PROJECT_SOURCE_DIR}/wordlists/english.txt"
)

# Find and link OpenSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(wallet PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Show all warnings
target_compile_options(wallet PRIVATE -Wall -Wextra -pedantic)

# -----------------------------------
# GoogleTest Setup
# -----------------------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# -----------------------------------
# Test Sources
# -----------------------------------
set(TEST_SOURCES
    bip32/bip32.cpp
    tests/bip39_tests.cpp
    bip39/bip39.cpp
    utils/utils.cpp
    address/address_generator.cpp
    keccak/keccak.cpp
)

# Include paths for tests
include_directories(
    ${PROJECT_SOURCE_DIR}/bip39
    ${PROJECT_SOURCE_DIR}/utils
)

# Define test executable
add_executable(bip39_tests ${TEST_SOURCES})

target_compile_definitions(bip39_tests PRIVATE
    TEST_VECTORS_PATH="${PROJECT_SOURCE_DIR}/tests/bip39_vectors.json"
    EN_WORDLISTS_PATH="${PROJECT_SOURCE_DIR}/wordlists/english.txt"
)

# Link test executable with GTest and your libraries
target_link_libraries(bip39_tests
    GTest::gtest
    GTest::gtest_main
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Test discovery
include(GoogleTest)
gtest_discover_tests(bip39_tests)
