/**
 * @file nx_assert.h
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
#ifndef VIRG_NEXUS_NX_ASSERT_H
#define VIRG_NEXUS_NX_ASSERT_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_message.h"

__NX_BEGIN_DECL

#if defined(NDEBUG)
#  define NX_ASSERT(expr) do { (void)sizeof(expr); } while(0)
#else
#  define NX_ASSERT(expr)                                               \
        do {                                                            \
                if (!(expr)) {                                          \
                        nx_fatal("NX_ASSERT",                           \
                                 "Assertion %s failed on file %s:%d",   \
                                 #expr, __FILE__, __LINE__);            \
                }                                                       \
        } while(0)
#endif

__NX_END_DECL

#endif
