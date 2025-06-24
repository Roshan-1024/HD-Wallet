/*
 * This file is based on the tiny_sha3 project by Markku-Juhani O. Saarinen
 * https://github.com/mjosaarinen/tiny_sha3
 *
 * MIT License
 * Copyright (c) 2015 Markku-Juhani O. Saarinen
 *
 * Modifications made for Ethereum-style Keccak-256 hashing:
 *  - Removed context structs and generic SHA3 APIs
 *  - Switched to Ethereum padding (0x01 … 0x80)
 *  - Converted to C++ (std::vector-based API)
 *  - Removed SHAKE and XOF functionality
 *
 * The original license is included below:
 *
 * -------------------------------------------------------------------------
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015 Markku-Juhani O. Saarinen
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * -------------------------------------------------------------------------
 */

#include "keccak.h"
#include <vector>
#include <cstdint>
#include <cstddef>

// Rotate left for 64-bit value
static inline uint64_t ROTL64(uint64_t x, int r) {
    return (x << r) | (x >> (64 - r));
}

// Keccak-f[1600] permutation constants
static const uint64_t KECCAKF_RNDC[24] = {
    0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL,
    0x8000000080008000ULL, 0x000000000000808bULL, 0x0000000080000001ULL,
    0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008aULL,
    0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
    0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL,
    0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
    0x000000000000800aULL, 0x800000008000000aULL, 0x8000000080008081ULL,
    0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};
static const int KECCAKF_ROTC[24] = {
     1,  3,   6,  10,  15,  21,  28,  36,
    45, 55,   2,  14,  27,  41,  56,   8,
    25, 43,  62,  18,  39,  61,  20,  44
};
static const int KECCAKF_PILN[24] = {
    10, 7, 11, 17, 18,  3,  5, 16,
     8,21, 24,  4, 15, 23, 19, 13,
    12, 2, 20, 14, 22,  9,  6,  1
};

// The Keccak-f[1600] permutation
static void keccakf(uint64_t st[25]) {
    for (int round = 0; round < 24; round++) {
        // Theta
        uint64_t bc[5];
        for (int i = 0; i < 5; i++)
            bc[i] = st[i] ^ st[i+5] ^ st[i+10] ^ st[i+15] ^ st[i+20];
        for (int i = 0; i < 5; i++) {
            uint64_t t = bc[(i+4)%5] ^ ROTL64(bc[(i+1)%5], 1);
            for (int j = 0; j < 25; j += 5)
                st[j+i] ^= t;
        }
        // Rho & Pi
        uint64_t t = st[1];
        for (int i = 0; i < 24; i++) {
            int j = KECCAKF_PILN[i];
            bc[0] = st[j];
            st[j] = ROTL64(t, KECCAKF_ROTC[i]);
            t = bc[0];
        }
        // Chi
        for (int j = 0; j < 25; j += 5) {
            for (int i = 0; i < 5; i++)
                bc[i] = st[j+i];
            for (int i = 0; i < 5; i++)
                st[j+i] ^= (~bc[(i+1)%5]) & bc[(i+2)%5];
        }
        // Iota
        st[0] ^= KECCAKF_RNDC[round];
    }
}

// Ethereum-style Keccak-256: input -> 32-byte hash
std::vector<unsigned char> keccak256(const std::vector<unsigned char>& input) {
    const size_t HASH_BYTES = 32;
    const size_t RATE = 1088 / 8;    // 136 bytes

    uint64_t state[25] = {0};
    size_t in_len = input.size();
    size_t offset = 0;

    // Absorb phase
    while (offset + RATE <= in_len) {
        for (size_t i = 0; i < RATE/8; i++) {
            uint64_t lane = 0;
            for (int b = 0; b < 8; b++)
                lane |= (uint64_t)input[offset + i*8 + b] << (8*b);
            state[i] ^= lane;
        }
        keccakf(state);
        offset += RATE;
    }
    // Partial block
    uint8_t temp[RATE] = {0};
    size_t rem = in_len - offset;
    for (size_t i = 0; i < rem; i++) temp[i] = input[offset + i];
    // Padding: 0x01 domain byte, then zeros, then 0x80
    temp[rem] = 0x01;
    temp[RATE-1] |= 0x80;
    // XOR into state
    for (size_t i = 0; i < RATE/8; i++) {
        uint64_t lane = 0;
        for (int b = 0; b < 8; b++)
            lane |= (uint64_t)temp[i*8 + b] << (8*b);
        state[i] ^= lane;
    }
    keccakf(state);

    // Squeeze phase: output first HASH_BYTES
    std::vector<unsigned char> out(HASH_BYTES);
    for (size_t i = 0; i < HASH_BYTES; i++) {
        size_t lane_pos = i / 8;
        int byte_pos = i % 8;
        out[i] = (state[lane_pos] >> (8*byte_pos)) & 0xFF;
    }
    return out;
}
