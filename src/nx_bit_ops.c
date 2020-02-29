/**
 * @file nx_bit_ops.c
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
#include "virg/nexus/nx_bit_ops.h"

/*
 * From http://gurmeetsingh.wordpress.com/2008/08/05/fast-bit-counting-routines
 */
#define TWO32(c)     ((uint32_t)0x1 << (c))
#define MASK32(c)    (((uint32_t)(-1)) / (TWO32(TWO32(c)) + (uint32_t)1))
#define COUNT32(x,c) ((x) & MASK32(c)) + (((x) >> (TWO32(c))) & MASK32(c))

#define TWO64(c)     ((uint64_t)0x1 << (c))
#define MASK64(c)    (((uint64_t)(-1)) / (TWO64(TWO64(c)) + (uint64_t)1))
#define COUNT64(x,c) ((x) & MASK64(c)) + (((x) >> (TWO64(c))) & MASK64(c))

int nx_bit_count32(uint32_t x)
{
        x = COUNT32(x, 0);
        x = COUNT32(x, 1);
        x = COUNT32(x, 2);
        x = COUNT32(x, 3);
        x = COUNT32(x, 4);
        return x;
}

int nx_bit_count64(uint64_t x)
{
        x = COUNT64(x, 0);
        x = COUNT64(x, 1);
        x = COUNT64(x, 2);
        x = COUNT64(x, 3);
        x = COUNT64(x, 4);
        x = COUNT64(x, 5);
        return x;
}
