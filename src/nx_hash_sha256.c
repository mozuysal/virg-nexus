/**
 * @file nx_hash_sha256.c
 *
 * Copyright (C) 2019 Mustafa Ozuysal. All rights reserved.
 *
 * This file is part of the VIRG-Nexus Library
 *
 * VIRG-Nexus Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VIRG-Nexus Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_hash_sha256.h"

#include <string.h>
#include <stdio.h>

#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_string.h"
#include "virg/nexus/nx_bit_ops.h"

#define NX_SHA256_CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define NX_SHA256_MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define NX_SHA256_SIGMA0(x) (nx_rotr32(x, 2) ^ nx_rotr32(x, 13) ^ nx_rotr32(x, 22))
#define NX_SHA256_SIGMA1(x) (nx_rotr32(x, 6) ^ nx_rotr32(x, 11) ^ nx_rotr32(x, 25))

void nx_sha256_process_block(uint8_t *block, uint32_t *H)
{
        const uint32_t K[] = {
                0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
                0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
                0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
                0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
                0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };

        uint32_t a = H[0];
        uint32_t b = H[1];
        uint32_t c = H[2];
        uint32_t d = H[3];
        uint32_t e = H[4];
        uint32_t f = H[5];
        uint32_t g = H[6];
        uint32_t h = H[7];

        uint32_t W[64];
        for (int i = 0; i < 16; ++i) {
                W[i] = nx_compose_big_endian32(block + i*4);
        }
        for (int i = 16; i < 64; ++i) {
                W[i] = (nx_rotr32(W[i-2], 17) ^ nx_rotr32(W[i-2], 19) ^ (W[i-2] >> 10))
                        + W[i-7]
                        + (nx_rotr32(W[i-15], 7) ^ nx_rotr32(W[i-15], 18) ^ (W[i-15] >> 3))
                        + W[i-16];
        }

        for (int i = 0; i < 64; ++i) {
                uint32_t T1 = h + NX_SHA256_SIGMA1(e) + NX_SHA256_CH(e,f,g) + K[i] + W[i];
                uint32_t T2 = NX_SHA256_SIGMA0(a) + NX_SHA256_MAJ(a,b,c);
                h = g;
                g = f;
                f = e;
                e = d + T1;
                d = c;
                c = b;
                b = a;
                a = T1 + T2;
        }

        H[0] += a;
        H[1] += b;
        H[2] += c;
        H[3] += d;
        H[4] += e;
        H[5] += f;
        H[6] += g;
        H[7] += h;
}

void nx_sha256(uint8_t hash[32], const uint8_t *msg, size_t lmsg)
{
        nx_sha256_multi(hash, 1, &msg, &lmsg);
}

void nx_sha256_multi(uint8_t hash[32], int n_msg,
                     const uint8_t * const *msg, const size_t *lmsg)
{
        // initial hash sequence
        uint32_t H[8] = {
                0x6a09e667,
                0xbb67ae85,
                0x3c6ef372,
                0xa54ff53a,
                0x510e527f,
                0x9b05688c,
                0x1f83d9ab,
                0x5be0cd19 };

        size_t lmsg_total = 0;
        for (int i = 0; i < n_msg; ++i)
                lmsg_total += lmsg[i];

        size_t n_msg_only_blocks = lmsg_total / 64;
        int l_remaining_msg = (int)(lmsg_total - n_msg_only_blocks * 64);
        int k; // number of all zero pad bytes
        if (l_remaining_msg <= 55)
                k = 55 - l_remaining_msg;
        else
                k = 119 - l_remaining_msg;

        // process the blocks containing only message data
        uint8_t block[64];
        int msg_offset = 0;
        size_t offset = 0;
        for (size_t b = 0; b < n_msg_only_blocks; ++b) {
                size_t n_copied = nx_memncpy_multi(&block[0], n_msg,
                                                   (const void * const *)msg,
                                                   lmsg, 64,
                                                   &msg_offset, &offset);
                NX_ASSERT(n_copied == 64U);

                nx_sha256_process_block(&block[0], &H[0]);
        }

        // process the last block containing message data by padding
        uint8_t *block_p = &block[0];
        size_t n_copied = nx_memncpy_multi(&block[0], n_msg,
                                           (const void * const *)msg,
                                           lmsg, l_remaining_msg,
                                           &msg_offset, &offset);
        NX_ASSERT(n_copied == (size_t)l_remaining_msg);
        block_p += l_remaining_msg;

        *block_p = 0x80;
        block_p++;

        if (l_remaining_msg <= 55) {
                // The rest of the message and padding fits in a single block
                memset(block_p, 0, k);
                block_p += k;
                nx_split_big_endian64(block_p, lmsg_total * 8);
                nx_sha256_process_block(&block[0], &H[0]);
        } else {
                // We need to finish this block with k0 zeros and process it
                int k0 = 64 - (l_remaining_msg + 1);
                int k1 = k - k0;
                memset(block_p, 0, k0);
                nx_sha256_process_block(&block[0], &H[0]);

                // There is another block containing the remaining padding zeros
                // and length
                block_p = &block[0];
                memset(block_p, 0, k1);
                block_p += k1;
                nx_split_big_endian64(block_p, lmsg_total * 8);
                nx_sha256_process_block(&block[0], &H[0]);
        }

        // copy to output hash
        for (int i = 0; i < 8; ++i)
                nx_split_big_endian32(hash + 4*i, H[i]);
}
