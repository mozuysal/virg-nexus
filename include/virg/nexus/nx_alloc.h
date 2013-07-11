/**
 * @file nx_alloc.h
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
#ifndef VIRG_NEXUS_NX_ALLOC_H
#define VIRG_NEXUS_NX_ALLOC_H

#include <stdlib.h>

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

void *nx_malloc(size_t sz);

void *nx_calloc(size_t n_memb, size_t sz);

void *nx_fmalloc(size_t sz);

void *nx_fcalloc(size_t n_memb, size_t sz);

void *nx_frealloc(void *ptr, size_t sz);

void *nx_aligned_alloc(size_t sz, size_t alignment);

void *nx_faligned_alloc(size_t sz, size_t alignment);

#define nx_free(x) free((x))

#define nx_new(n,type) ((type *)nx_fmalloc((n) * sizeof(type)))
#define nx_new_c(n)    nx_new((n),char)
#define nx_new_uc(n)   nx_new((n),unsigned char)
#define nx_new_i(n)    nx_new((n),int)
#define nx_new_ui(n)   nx_new((n),unsigned int)
#define nx_new_l(n)    nx_new((n),long)
#define nx_new_ul(n)   nx_new((n),unsigned long)
#define nx_new_z(n)    nx_new((n),size_t)
#define nx_new_s(n)    nx_new((n),float)
#define nx_new_d(n)    nx_new((n),double)

__NX_END_DECL

#endif
