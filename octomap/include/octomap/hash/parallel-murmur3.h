// -*- mode: c++; c-basic-offset: 4 indent-tabs-mode: nil -*- */
//
// Copyright 2017 Juho Snellman, released under a MIT license:
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// README
// ------
//
// Three AVX2 implementations of the Murmur3 hash functions.
//
// - parallel: Computing the hash values of 8 keys in parallel.
// - parallel_multiseed: Computing N hash values for each of 8 keys
//   in parallel. Each of the N hash values for a key will be computed
//   using a different key.
// - scalar: Computing the hash value of a single key.
//
// You probably don't want to use any of these, look in
// parallel-xxhash.h instead. (It is missing parallel_multiseed, since
// my use case for that disappeared. But it'd be trivial to implement).

#ifndef PARALLEL_MURMUR3_H
#define PARALLEL_MURMUR3_H

#include <cstdint>
#include <cstdio>

template<int SizeWords>
struct murmur3 {

    // Compute a hash value for the key.
    static uint32_t scalar(uint32_t* key, uint32_t seed) {
        const uint32_t c1s = 0xcc9e2d51;
        const uint32_t c2s = 0x1b873593;
        uint32_t h = seed;

        for (int i = 0; i < SizeWords; ++i) {
            uint32_t k = key[i];
            k *= c1s;
            k = rol32<15>(k);
            k *= c2s;

            h ^= k;
            h = rol32<13>(h);
            h = h*5 + 0xe6546b64;
        }

        h ^= SizeWords * 4;

        return fmix32(h);
    }

private:

    static uint32_t fmix32(uint32_t h) {
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;

        return h;
    }

    template<int r>
    static uint32_t rol32(uint32_t x) {
        return (x << r) | (x >> (32 - r));
    }
};

#endif // PARALLEL_MURMUR3_H
