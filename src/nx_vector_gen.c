/**
 * @file nx_vector_gen.c
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
 * --------------------------------------------------------------------------------
 * Generic vector implementation
 *
 * Define the following macros and include this file in the specific vector implementation:
 *
 * NX_VECTOR_NAME     -- name of the vector data structure, e.g. NXKeypointVector
 * NX_VECTOR_FUNC(fn) -- generate func. name for vector functions, e.g. nx_keypoint_vector_##fn
 * NX_VECTOR_ITEM     -- type of the vector elements, e.g. struct NXKeypoint
 *
 */
#include "virg/nexus/nx_alloc.h"

#define NX_VECTOR_ITEM_SIZE sizeof(NX_VECTOR_ITEM)

static void _update_vector_fields(struct NX_VECTOR_NAME *v)
{
        v->capacity = v->mem->capacity / NX_VECTOR_ITEM_SIZE;
        v->size     = v->mem->size / NX_VECTOR_ITEM_SIZE;
        v->data     = (NX_VECTOR_ITEM *)(v->mem->ptr);
}

struct NX_VECTOR_NAME *NX_VECTOR_FUNC(alloc)()
{
        struct NX_VECTOR_NAME *v = NX_NEW(1, struct NX_VECTOR_NAME);

        v->size = 0;
        v->capacity = 0;
        v->data = NULL;

        v->mem = nx_mem_block_alloc();

        return v;
}

struct NX_VECTOR_NAME *NX_VECTOR_FUNC(new)(int initial_capacity)
{
        struct NX_VECTOR_NAME *v = NX_VECTOR_FUNC(alloc)();
        NX_VECTOR_FUNC(reserve)(v, initial_capacity);
        return v;
}

void NX_VECTOR_FUNC(free)(struct NX_VECTOR_NAME *v)
{
        if (v) {
                nx_mem_block_free(v->mem);
                nx_free(v);
        }
}

void NX_VECTOR_FUNC(release)(struct NX_VECTOR_NAME *v)
{
        nx_mem_block_release(v->mem);

        v->size = 0;
        v->capacity = 0;
        v->data = NULL;
}

void NX_VECTOR_FUNC(reserve)(struct NX_VECTOR_NAME *v, size_t new_capacity)
{
        if (new_capacity > v->capacity) {
                nx_mem_block_reserve(v->mem, new_capacity*NX_VECTOR_ITEM_SIZE);
                _update_vector_fields(v);
        }
}

void NX_VECTOR_FUNC(resize)(struct NX_VECTOR_NAME *v, size_t new_size)
{
        nx_mem_block_resize(v->mem, new_size*NX_VECTOR_ITEM_SIZE);
        _update_vector_fields(v);
}

void NX_VECTOR_FUNC(clear)(struct NX_VECTOR_NAME *v)
{
        NX_VECTOR_FUNC(resize)(v, 0);
}

void NX_VECTOR_FUNC(wrap)(struct NX_VECTOR_NAME *v, NX_VECTOR_ITEM *items, size_t sz, size_t capacity, NXBool own_memory)
{
        nx_mem_block_wrap(v->mem, (char*)items, sz*NX_VECTOR_ITEM_SIZE, capacity*NX_VECTOR_ITEM_SIZE, own_memory);
        _update_vector_fields(v);
}

struct NX_VECTOR_NAME *NX_VECTOR_FUNC(copy0)(const struct NX_VECTOR_NAME *v)
{
        struct NX_VECTOR_NAME *cpy = NX_VECTOR_FUNC(alloc)();
        NX_VECTOR_FUNC(copy)(cpy, v);
        return cpy;
}

void NX_VECTOR_FUNC(copy)(struct NX_VECTOR_NAME *dest, const struct NX_VECTOR_NAME *src)
{
        nx_mem_block_copy(dest->mem, src->mem);
        _update_vector_fields(dest);
}

NX_VECTOR_ITEM *NX_VECTOR_FUNC(push_back)(struct NX_VECTOR_NAME *v)
{
        if (v->size >= v->capacity)
                NX_VECTOR_FUNC(reserve)(v, v->size * 2);
        NX_VECTOR_FUNC(resize)(v, v->size+1);
        return v->data + v->size-1;
}
