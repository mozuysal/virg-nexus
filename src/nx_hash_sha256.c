/**
 * @file nx_hash.c
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
#include "virg/nexus/nx_hash.h"

#include <string.h>
#include <stdio.h>

#include "virg/nexus/nx_bit_ops.h"

#define CHUNK_SIZE 64
#define TOTAL_LEN_LEN 8

// Public Domain SHA-256 implementation from https://github.com/amosnier/sha-2
// Not optimized enough but good enough

/*
 * Initialize array of round constants:
 * (first 32 bits of the fractional parts of the cube roots of the first 64 primes 2..311):
 */
static const uint32_t k[] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

struct buffer_state {
	const uint8_t * p;
	size_t len;
	size_t total_len;
	int single_one_delivered; /* bool */
	int total_len_delivered; /* bool */
};

static inline uint32_t right_rot(uint32_t value, unsigned int count)
{
	/*
	 * Defined behaviour in standard C for all count where 0 < count < 32,
	 * which is what we need here.
	 */
	return value >> count | value << (32 - count);
}

static void init_buf_state(struct buffer_state * state, const void * input, size_t len)
{
	state->p = input;
	state->len = len;
	state->total_len = len;
	state->single_one_delivered = 0;
	state->total_len_delivered = 0;
}

/* Return value: bool */
static int calc_chunk(uint8_t chunk[CHUNK_SIZE], struct buffer_state * state)
{
	size_t space_in_chunk;

	if (state->total_len_delivered) {
		return 0;
	}

	if (state->len >= CHUNK_SIZE) {
		memcpy(chunk, state->p, CHUNK_SIZE);
		state->p += CHUNK_SIZE;
		state->len -= CHUNK_SIZE;
		return 1;
	}

	memcpy(chunk, state->p, state->len);
	chunk += state->len;
	space_in_chunk = CHUNK_SIZE - state->len;
	state->p += state->len;
	state->len = 0;

	/* If we are here, space_in_chunk is one at minimum. */
	if (!state->single_one_delivered) {
		*chunk++ = 0x80;
		space_in_chunk -= 1;
		state->single_one_delivered = 1;
	}

	/*
	 * Now:
	 * - either there is enough space left for the total length, and we can conclude,
	 * - or there is too little space left, and we have to pad the rest of this chunk with zeroes.
	 * In the latter case, we will conclude at the next invokation of this function.
	 */
	if (space_in_chunk >= TOTAL_LEN_LEN) {
		const size_t left = space_in_chunk - TOTAL_LEN_LEN;
		size_t len = state->total_len;
		int i;
		memset(chunk, 0x00, left);
		chunk += left;

		/* Storing of len * 8 as a big endian 64-bit without overflow. */
		chunk[7] = (uint8_t) (len << 3);
		len >>= 5;
		for (i = 6; i >= 0; i--) {
			chunk[i] = (uint8_t) len;
			len >>= 8;
		}
		state->total_len_delivered = 1;
	} else {
		memset(chunk, 0x00, space_in_chunk);
	}

	return 1;
}

void nx_sha256(uint8_t hash[32], const void *input, size_t len)
{
	uint32_t h[] = { 0x6a09e667,
                         0xbb67ae85,
                         0x3c6ef372,
                         0xa54ff53a,
                         0x510e527f,
                         0x9b05688c,
                         0x1f83d9ab,
                         0x5be0cd19 };
	unsigned i, j;

	/* 512-bit chunks is what we will operate on. */
	uint8_t chunk[64];

	struct buffer_state state;

	init_buf_state(&state, input, len);

	while (calc_chunk(chunk, &state)) {
		uint32_t ah[8];

		const uint8_t *p = chunk;

		/* Initialize working variables to current hash value: */
		for (i = 0; i < 8; i++)
			ah[i] = h[i];

		/* Compression function main loop: */
		for (i = 0; i < 4; i++) {
			/*
			 * The w-array is really w[64], but since we only need
			 * 16 of them at a time, we save stack by calculating
			 * 16 at a time.
			 *
			 * This optimization was not there initially and the
			 * rest of the comments about w[64] are kept in their
			 * initial state.
			 */

			/*
			 * create a 64-entry message schedule array w[0..63] of 32-bit words
			 * (The initial values in w[0..63] don't matter, so many implementations zero them here)
			 * copy chunk into first 16 words w[0..15] of the message schedule array
			 */
			uint32_t w[16];

			for (j = 0; j < 16; j++) {
				if (i == 0) {
					w[j] = (uint32_t) p[0] << 24 | (uint32_t) p[1] << 16 |
						(uint32_t) p[2] << 8 | (uint32_t) p[3];
					p += 4;
				} else {
					/* Extend the first 16 words into the remaining 48 words w[16..63] of the message schedule array: */
					const uint32_t s0 = right_rot(w[(j + 1) & 0xf], 7) ^ right_rot(w[(j + 1) & 0xf], 18) ^ (w[(j + 1) & 0xf] >> 3);
					const uint32_t s1 = right_rot(w[(j + 14) & 0xf], 17) ^ right_rot(w[(j + 14) & 0xf], 19) ^ (w[(j + 14) & 0xf] >> 10);
					w[j] = w[j] + s0 + w[(j + 9) & 0xf] + s1;
				}
				const uint32_t s1 = right_rot(ah[4], 6) ^ right_rot(ah[4], 11) ^ right_rot(ah[4], 25);
				const uint32_t ch = (ah[4] & ah[5]) ^ (~ah[4] & ah[6]);
				const uint32_t temp1 = ah[7] + s1 + ch + k[i << 4 | j] + w[j];
				const uint32_t s0 = right_rot(ah[0], 2) ^ right_rot(ah[0], 13) ^ right_rot(ah[0], 22);
				const uint32_t maj = (ah[0] & ah[1]) ^ (ah[0] & ah[2]) ^ (ah[1] & ah[2]);
				const uint32_t temp2 = s0 + maj;

				ah[7] = ah[6];
				ah[6] = ah[5];
				ah[5] = ah[4];
				ah[4] = ah[3] + temp1;
				ah[3] = ah[2];
				ah[2] = ah[1];
				ah[1] = ah[0];
				ah[0] = temp1 + temp2;
			}
		}

		/* Add the compressed chunk to the current hash value: */
		for (i = 0; i < 8; i++)
			h[i] += ah[i];
	}

	/* Produce the final hash value (big-endian): */
	for (i = 0, j = 0; i < 8; i++)
	{
		hash[j++] = (uint8_t) (h[i] >> 24);
		hash[j++] = (uint8_t) (h[i] >> 16);
		hash[j++] = (uint8_t) (h[i] >> 8);
		hash[j++] = (uint8_t) h[i];
	}
}

#define NX_SHA256_8to32(W)          \
        do {                                    \
                (h0) = (uint8_t) ((H) >> 24);   \
                (h1) = (uint8_t) ((H) >> 16);   \
                (h2) = (uint8_t) ((H) >> 8);    \
                (h3) = (uint8_t) (H);           \
        } while (1)

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

void nx_sha256_fast(uint8_t hash[32], const uint8_t *msg, size_t lmsg)
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

        size_t n_msg_only_blocks = lmsg / 64;
        int l_remaining_msg = (int)(lmsg - n_msg_only_blocks * 64);
        int k; // number of all zero pad bytes
        if (l_remaining_msg <= 55)
                k = 55 - l_remaining_msg;
        else
                k = 119 - l_remaining_msg;

        // process the blocks containing only message data
        uint8_t block[64];
        for (size_t b = 0; b < n_msg_only_blocks; ++b) {
                memcpy(&block[0], msg, 64);
                msg += 64;

                nx_sha256_process_block(&block[0], &H[0]);
        }

        // form the last block by padding
        uint8_t *block_p = &block[0];
        memcpy(block_p, msg, l_remaining_msg);
        block_p += l_remaining_msg;

        *block_p = 0x80;
        block_p++;

        memset(block_p, 0, k);
        block_p += k;

        nx_split_big_endian64(block_p, lmsg * 8);

        // process the last block
        nx_sha256_process_block(&block[0], &H[0]);

        // copy to output hash
        for (int i = 0; i < 8; ++i)
                nx_split_big_endian32(hash + 4*i, H[i]);
}
