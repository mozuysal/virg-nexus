/**
 * @file nx_alloc.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Mustafa Ozuysal. All rights reserved.
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

void nx_free(void *ptr);

#define NX_NEW(n,type) ((type *)nx_fmalloc((n) * sizeof(type)))
#define NX_NEW_C(n)    NX_NEW((n),char)
#define NX_NEW_UC(n)   NX_NEW((n),unsigned char)
#define NX_NEW_SI(n)   NX_NEW((n),short)
#define NX_NEW_I(n)    NX_NEW((n),int)
#define NX_NEW_UI(n)   NX_NEW((n),unsigned int)
#define NX_NEW_L(n)    NX_NEW((n),long)
#define NX_NEW_UL(n)   NX_NEW((n),unsigned long)
#define NX_NEW_Z(n)    NX_NEW((n),size_t)
#define NX_NEW_S(n)    NX_NEW((n),float)
#define NX_NEW_D(n)    NX_NEW((n),double)

__NX_END_DECL

#endif
