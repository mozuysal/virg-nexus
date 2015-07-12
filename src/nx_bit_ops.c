/**
 * @file nx_bit_ops.c
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2015 Mustafa Ozuysal. All rights reserved.
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
