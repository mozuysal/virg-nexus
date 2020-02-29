/**
 * @file nx_mem_block.c
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
#include "virg/nexus/nx_mem_block.h"

#include <string.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"

struct NXMemBlock *nx_mem_block_alloc()
{
        struct NXMemBlock *mem = NX_NEW(1, struct NXMemBlock);

        mem->ptr = NULL;
        mem->size = 0;
        mem->capacity = 0;
        mem->own_memory = NX_FALSE;

        return mem;
}

struct NXMemBlock *nx_mem_block_new(size_t init_sz)
{
        struct NXMemBlock *mem = nx_mem_block_alloc();
        nx_mem_block_resize(mem, init_sz);

        return mem;
}

void nx_mem_block_free(struct NXMemBlock *mem)
{
        if (mem) {
                if (mem->own_memory) {
                        nx_free(mem->ptr);
                }
                nx_free(mem);
        }
}

void *nx_mem_block_reserve(struct NXMemBlock *mem, size_t new_capacity)
{
        NX_ASSERT_PTR(mem);

        if (new_capacity > mem->capacity) {
                if (mem->own_memory) {
                        mem->ptr = nx_xrealloc(mem->ptr, new_capacity);
                } else {
                        void *new_ptr = nx_xmalloc(new_capacity);
                        if (mem->size > 0)
                                memcpy(new_ptr, mem->ptr, mem->size);
                        mem->ptr = new_ptr;
                        mem->own_memory = NX_TRUE;
                }

                mem->capacity = new_capacity;
        }

        return mem->ptr;
}

void *nx_mem_block_resize(struct NXMemBlock *mem, size_t new_sz)
{
        NX_ASSERT_PTR(mem);

        nx_mem_block_reserve(mem, new_sz);
        mem->size = new_sz;

        return mem->ptr;
}

void nx_mem_block_release(struct NXMemBlock *mem)
{
        NX_ASSERT_PTR(mem);

        if (mem->ptr && mem->own_memory) {
                nx_free(mem->ptr);
        }

        mem->ptr = NULL;
        mem->size = 0;
        mem->capacity = 0;
        mem->own_memory = NX_FALSE;
}

void *nx_mem_block_wrap(struct NXMemBlock *mem, void *ptr, size_t sz, size_t capacity, NXBool own_memory)
{
        NX_ASSERT_PTR(mem);
        NX_ASSERT_PTR(ptr);

        nx_mem_block_release(mem);

        mem->ptr = ptr;
        mem->size = sz;
        mem->capacity = capacity;
        mem->own_memory = own_memory;

        return mem->ptr;
}

void nx_mem_block_swap(struct NXMemBlock *mem0, struct NXMemBlock *mem1)
{
        NX_ASSERT_PTR(mem0);
        NX_ASSERT_PTR(mem1);

        void *t_ptr;
        size_t t_sz;
        NXBool t_om;

        t_ptr = mem0->ptr; mem0->ptr = mem1->ptr; mem1->ptr = t_ptr;
        t_sz = mem0->size; mem0->size = mem1->size; mem1->size = t_sz;
        t_sz = mem0->capacity; mem0->capacity = mem1->capacity; mem1->capacity = t_sz;
        t_om = mem0->own_memory; mem0->own_memory = mem1->own_memory; mem1->own_memory = t_om;
}

struct NXMemBlock *nx_mem_block_copy0(struct NXMemBlock *mem)
{
        NX_ASSERT_PTR(mem);
        struct NXMemBlock *cpy = nx_mem_block_alloc();

        nx_mem_block_copy(cpy, mem);
        return cpy;
}

void *nx_mem_block_copy(struct NXMemBlock *dest, const struct NXMemBlock *src)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);

        nx_mem_block_resize(dest, src->size);

        if (src->ptr) {
                memcpy(dest->ptr, src->ptr, dest->size);
        }

        return dest->ptr;
}

void nx_mem_block_set_zero(struct NXMemBlock *mem)
{
        NX_ASSERT_PTR(mem);

        if (mem->ptr) {
                memset(mem->ptr, 0, mem->size);
        }
}
