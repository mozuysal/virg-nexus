/**
 * @file nx_mem_block.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Izmir Institute of Technology. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_MEM_BLOCK_H
#define VIRG_NEXUS_NX_MEM_BLOCK_H

#include <stdlib.h>

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

struct NXMemBlock
{
        uchar *ptr;
        size_t size;
        size_t capacity;
        NXBool own_memory;
};

struct NXMemBlock *nx_mem_block_alloc();

struct NXMemBlock *nx_mem_block_new(size_t init_sz);

void nx_mem_block_free(struct NXMemBlock *mem);

void nx_mem_block_resize(struct NXMemBlock *mem, size_t new_sz);

void nx_mem_block_release(struct NXMemBlock *mem);

void nx_mem_block_wrap(struct NXMemBlock *mem, uchar* ptr, size_t sz, NXBool own_memory);

void nx_mem_block_swap(struct NXMemBlock *mem0, struct NXMemBlock *mem1);

struct NXMemBlock *nx_mem_block_copy0(struct NXMemBlock *mem);

void nx_mem_block_copy(struct NXMemBlock *dest, const struct NXMemBlock *src);

void nx_mem_block_set_zero(struct NXMemBlock *mem);

__NX_END_DECL

#endif
