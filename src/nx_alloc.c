/**
 * @file nx_alloc.c
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
#include "virg/nexus/nx_alloc.h"

#include <string.h>

#include "virg/nexus/nx_message.h"

extern int posix_memalign(void **memptr, size_t alignment, size_t size);

void *nx_aligned_alloc(size_t sz, size_t alignment)
{
        void *ptr = 0;
        int alloc_res = posix_memalign(&ptr, alignment, sz);

        if (alloc_res) {
                nx_error("nx_aligned_alloc", "Error allocating %zd bytes, out of memory!", sz);
                return NULL;
        } else {
                return ptr;
        }
}

void *nx_faligned_alloc(size_t sz, size_t alignment)
{
        void *ptr = 0;
        int alloc_res = posix_memalign(&ptr, alignment, sz);

        if (alloc_res) {
                nx_fatal("nx_aligned_alloc", "Error allocating %zd bytes, out of memory!", sz);
                return NULL;
        } else {
                return ptr;
        }
}

void *nx_malloc(size_t sz)
{
        void *ptr = malloc(sz);

        if (!ptr) {
                nx_error("nx_malloc", "Error allocating %zd bytes, out of memory!", sz);
                return NULL;
        }

        return ptr;
}

void *nx_calloc(size_t n_memb, size_t sz)
{
        const size_t length = n_memb * sz;
        void *ptr = nx_malloc(length);
        if (ptr) {
                memset(ptr, 0, length);
        }

        return ptr;
}

void *nx_fmalloc(size_t sz)
{
        void *ptr = malloc(sz);

        if (!ptr) {
                nx_fatal("nx_fmalloc", "Error allocating %zd bytes, out of memory!", sz);
                return NULL;
        } else {
                return ptr;
        }
}

void *nx_fcalloc(size_t n_memb, size_t sz)
{
        void *ptr = nx_calloc(n_memb, sz);
        if (!ptr)
                nx_fatal("nx_calloc", "Error allocating %zd bytes, out of memory!", sz);

        return ptr;
}

void *nx_frealloc(void *ptr, size_t sz)
{
        void *res_ptr = realloc(ptr, sz);
        if (!res_ptr)
                nx_fatal("nx_realloc", "Error reallocating %zd bytes, out of memory!", sz);

        return res_ptr;
}
