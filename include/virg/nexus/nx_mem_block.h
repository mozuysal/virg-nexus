/**
 * @file nx_mem_block.h
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
#ifndef VIRG_NEXUS_NX_MEM_BLOCK_H
#define VIRG_NEXUS_NX_MEM_BLOCK_H

#include <stdlib.h>

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

struct NXMemBlock
{
        void *ptr;
        size_t size;
        size_t capacity;
        NXBool own_memory;
};

struct NXMemBlock *nx_mem_block_alloc();

struct NXMemBlock *nx_mem_block_new(size_t init_sz);

void nx_mem_block_free(struct NXMemBlock *mem);

void *nx_mem_block_reserve(struct NXMemBlock *mem, size_t new_capacity);

void *nx_mem_block_resize(struct NXMemBlock *mem, size_t new_sz);

void nx_mem_block_release(struct NXMemBlock *mem);

void *nx_mem_block_wrap(struct NXMemBlock *mem, void *ptr, size_t sz, size_t capacity, NXBool own_memory);

void nx_mem_block_swap(struct NXMemBlock *mem0, struct NXMemBlock *mem1);

struct NXMemBlock *nx_mem_block_copy0(struct NXMemBlock *mem);

void *nx_mem_block_copy(struct NXMemBlock *dest, const struct NXMemBlock *src);

void nx_mem_block_set_zero(struct NXMemBlock *mem);

__NX_END_DECL

#endif
