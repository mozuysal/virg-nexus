/**
 * @file nx_alloc.c
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
#include "virg/nexus/nx_alloc.h"

#include <string.h>
#include <stdalign.h>

#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_assert.h"

void *nx_aligned_alloc(size_t alignment, size_t sz)
{
        NX_ASSERT(sz % alignment == 0);

        void *ptr = aligned_alloc(alignment, sz);

        if (!ptr) {
                NX_ERROR(NX_LOG_TAG, "Error allocating %zd bytes, out of memory!", sz);
                return NULL;
        } else {
                return ptr;
        }
}

void *nx_xaligned_alloc(size_t alignment, size_t sz)
{
        NX_ASSERT(sz % alignment == 0);

        void *ptr = aligned_alloc(alignment, sz);

        if (!ptr) {
                NX_FATAL(NX_LOG_TAG, "Error allocating %zd bytes, out of memory!", sz);
        } else {
                return ptr;
        }
}

void *nx_malloc(size_t sz)
{
        void *ptr = malloc(sz);

        if (sz != 0 && !ptr) {
                NX_ERROR(NX_LOG_TAG, "Error allocating %zd bytes, out of memory!", sz);
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

void *nx_xmalloc(size_t sz)
{
        void *ptr = malloc(sz);

        if (sz != 0 && !ptr) {
                NX_FATAL(NX_LOG_TAG, "Error allocating %zd bytes, out of memory!", sz);
                return NULL;
        } else {
                return ptr;
        }
}

void *nx_xcalloc(size_t n_memb, size_t sz)
{
        void *ptr = nx_calloc(n_memb, sz);
        if (n_memb != 0 && sz != 0 && !ptr)
                NX_FATAL(NX_LOG_TAG, "Error allocating %zd bytes, out of memory!", sz);

        return ptr;
}

void *nx_xrealloc(void *ptr, size_t sz)
{
        void *res_ptr = realloc(ptr, sz);
        if (sz != 0 && !res_ptr)
                NX_FATAL(NX_LOG_TAG, "Error reallocating %zd bytes, out of memory!", sz);

        return res_ptr;
}

void nx_free(void *ptr)
{
        free(ptr);
}
