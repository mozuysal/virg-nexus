/**
 * @file nx_vector_gen.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2014 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 * --------------------------------------------------------------------------------
 * Generic vector implementation
 *
 * Define the following macros and include this file in the specific vector header:
 *
 * NX_VECTOR_NAME     -- name of the vector data structure, e.g. NXKeypointVector
 * NX_VECTOR_FUNC(fn) -- generate func. name for vector functions, e.g. nx_keypoint_vector_##fn
 * NX_VECTOR_ITEM     -- type of the vector elements, e.g. struct NXKeypoint
 *
 */
#ifndef VIRG_NEXUS_NX_VECTOR_GEN_H
#define VIRG_NEXUS_NX_VECTOR_GEN_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_mem_block.h"

__NX_BEGIN_DECL

struct NX_VECTOR_NAME {
        size_t size;
        size_t capacity;
        NX_VECTOR_ITEM *data;

        struct NXMemBlock *mem;
};

struct NX_VECTOR_NAME *NX_VECTOR_FUNC(alloc)    ();
struct NX_VECTOR_NAME *NX_VECTOR_FUNC(new)      (int initial_capacity);
void                   NX_VECTOR_FUNC(free)     (struct NX_VECTOR_NAME *v);
void                   NX_VECTOR_FUNC(release)  (struct NX_VECTOR_NAME *v);
void                   NX_VECTOR_FUNC(reserve)  (struct NX_VECTOR_NAME *v, size_t new_capacity);
void                   NX_VECTOR_FUNC(resize)   (struct NX_VECTOR_NAME *v, size_t new_size);
void                   NX_VECTOR_FUNC(clear)    (struct NX_VECTOR_NAME *v);
void                   NX_VECTOR_FUNC(wrap)     (struct NX_VECTOR_NAME *v, NX_VECTOR_ITEM *items, size_t sz, size_t capacity, NXBool own_memory);
struct NX_VECTOR_NAME *NX_VECTOR_FUNC(copy0)    (const struct NX_VECTOR_NAME *v);
void                   NX_VECTOR_FUNC(copy)     (struct NX_VECTOR_NAME *dest, const struct NX_VECTOR_NAME *src);
NX_VECTOR_ITEM        *NX_VECTOR_FUNC(push_back)(struct NX_VECTOR_NAME *v);

__NX_END_DECL

#endif
