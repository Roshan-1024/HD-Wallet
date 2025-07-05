cmake_minimum_required(VERSION 3.14)
project(wallet VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(
    ${PROJECT_SOURCE_DIR}/bip39
    ${PROJECT_SOURCE_DIR}/bip32
    ${PROJECT_SOURCE_DIR}/config
    ${PROJECT_SOURCE_DIR}/utils
    ${PROJECT_SOURCE_DIR}/address
    ${PROJECT_SOURCE_DIR}/keccak
)

set(SOURCES
    main.cpp
    bip39/bip39.cpp
    bip32/bip32.cpp
    utils/utils.cpp
    address/address_generator.cpp
    keccak/keccak.cpp
)

add_executable(wallet ${SOURCES})
target_compile_definitions(wallet PRIVATE
    EN_WORDLISTS_PATH="${PROJECT_SOURCE_DIR}/wordlists/english.txt"
)

find_package(OpenSSL REQUIRED)
target_link_libraries(wallet PRIVATE OpenSSL::SSL OpenSSL::Crypto)

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
#	Unit Tests
# -----------------------------------
set(TEST_SOURCES
    tests/bip39_tests.cpp
    bip39/bip39.cpp
    tests/bip32_tests.cpp
    bip32/bip32.cpp
    utils/utils.cpp
    address/address_generator.cpp
    keccak/keccak.cpp
)

add_executable(unit_tests ${TEST_SOURCES})

target_compile_definitions(unit_tests PRIVATE
    BIP39_TEST_VECTORS_PATH="${PROJECT_SOURCE_DIR}/tests/bip39_vectors.json"
    BIP32_TEST_VECTORS_PATH="${PROJECT_SOURCE_DIR}/tests/bip32_vectors.json"
    EN_WORDLISTS_PATH="${PROJECT_SOURCE_DIR}/wordlists/english.txt"
)

target_link_libraries(unit_tests
    GTest::gtest
    GTest::gtest_main
    OpenSSL::SSL
    OpenSSL::Crypto
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
