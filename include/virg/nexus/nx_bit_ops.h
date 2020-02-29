/**
 * @file nx_bit_ops.h
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
 * You should have received a copy of the GNU General Public License along with
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_BIT_OPS_H
#define VIRG_NEXUS_NX_BIT_OPS_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

int nx_bit_count32(uint32_t x);
int nx_bit_count64(uint64_t x);

static inline uint32_t nx_rotr32(uint32_t x, uint8_t n)
{
        return (x >> n) | (x << (32 - n));
}

static inline uint64_t nx_rotr64(uint64_t x, uint8_t n)
{
        return (x >> n) | (x << (64 - n));
}

static inline void nx_split_big_endian32(uint8_t *bytep, uint32_t x) {
        bytep[0] = (uint8_t) (x >> 24);
        bytep[1] = (uint8_t) (x >> 16);
        bytep[2] = (uint8_t) (x >> 8);
        bytep[3] = (uint8_t) (x);
}

static inline void nx_split_big_endian64(uint8_t *bytep, uint64_t x) {
        bytep[0] = (uint8_t) (x >> 56);
        bytep[1] = (uint8_t) (x >> 48);
        bytep[2] = (uint8_t) (x >> 40);
        bytep[3] = (uint8_t) (x >> 32);
        bytep[4] = (uint8_t) (x >> 24);
        bytep[5] = (uint8_t) (x >> 16);
        bytep[6] = (uint8_t) (x >> 8);
        bytep[7] = (uint8_t) (x);
}

static inline uint32_t nx_compose_big_endian32(const uint8_t *bytep) {
        return ((uint32_t)bytep[0]) << 24
                | ((uint32_t)bytep[1]) << 16
                | ((uint32_t)bytep[2]) << 8
                | ((uint32_t)bytep[3]);
}


__NX_END_DECL

#endif
