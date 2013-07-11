/**
 * @file nx_mem_block.c
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
#include "virg/nexus/nx_mem_block.h"

#include <string.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"

struct NXMemBlock *nx_mem_block_alloc(size_t init_sz)
{
        struct NXMemBlock *mem = nx_new(1, struct NXMemBlock);

        mem->ptr = NULL;
        mem->size = 0;
        mem->capacity = 0;
        mem->own_memory = NX_FALSE;

        if (init_sz > 0)
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

void nx_mem_block_resize(struct NXMemBlock *mem, size_t new_sz)
{
        NX_ASSERT(mem != NULL);

        if (new_sz > mem->capacity) {
                mem->ptr = (uchar *)nx_frealloc(mem->ptr, new_sz * sizeof(uchar));
                mem->capacity = new_sz;

                mem->own_memory = NX_TRUE;
        }

       mem->size = new_sz;
}

void nx_mem_block_release(struct NXMemBlock *mem)
{
        NX_ASSERT(mem != NULL);

        if (mem->ptr && mem->own_memory) {
                nx_free(mem->ptr);
        }

        mem->ptr = NULL;
        mem->size = 0;
        mem->capacity = 0;
        mem->own_memory = NX_FALSE;
}

void nx_mem_block_wrap(struct NXMemBlock *mem, uchar* ptr, size_t sz, NXBool own_memory)
{
        NX_ASSERT(mem != NULL);
        NX_ASSERT(ptr != NULL);

        nx_mem_block_release(mem);

        mem->ptr = ptr;
        mem->size = sz;
        mem->capacity = sz;
        mem->own_memory = own_memory;
}

void nx_mem_block_swap(struct NXMemBlock *mem0, struct NXMemBlock *mem1)
{
        NX_ASSERT(mem0 != NULL);
        NX_ASSERT(mem1 != NULL);

        uchar *t_ptr;
        size_t t_sz;
        NXBool t_om;

        t_ptr = mem0->ptr; mem0->ptr = mem1->ptr; mem1->ptr = t_ptr;
        t_sz = mem0->size; mem0->size = mem1->size; mem1->size = t_sz;
        t_sz = mem0->capacity; mem0->capacity = mem1->capacity; mem1->capacity = t_sz;
        t_om = mem0->own_memory; mem0->own_memory = mem1->own_memory; mem1->own_memory = t_om;
}

void nx_mem_block_copy(struct NXMemBlock *dest, const struct NXMemBlock *src)
{
        NX_ASSERT(src != NULL);
        NX_ASSERT(dest != NULL);

        nx_mem_block_resize(dest, src->size);

        if (src->ptr) {
                memcpy(dest->ptr, src->ptr, dest->size * sizeof(uchar));
        }
}

void nx_mem_block_set_zero(struct NXMemBlock *mem)
{
        NX_ASSERT(mem != NULL);

        if (mem->ptr) {
                memset(mem->ptr, 0, mem->size * sizeof(uchar));
        }
}

