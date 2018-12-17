/**
 * @file nx_assert.h
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
                        NX_FATAL("NX_ASSERT","Assertion '%s' failed", #expr); \
                }                                                       \
        } while(0)
#endif

#if defined(NDEBUG)
#  define NX_ASSERT_PTR(ptr) do { (void)sizeof(ptr); } while(0)
#else
#  define NX_ASSERT_PTR(ptr)                                            \
        do {                                                            \
                if (!(ptr)) {                                           \
                        NX_FATAL("NX_ASSERT_PTR","Unexpected NULL pointer"); \
                }                                                       \
        } while(0)
#endif


#if defined(NDEBUG)
#  define NX_ASSERT_CUSTOM(msg,expr) do { (void)sizeof(expr);(void)sizeof(msg); } while(0)
#else
#  define NX_ASSERT_CUSTOM(msg,expr)                                    \
        do {                                                            \
                if (!(expr)) {                                          \
                        NX_FATAL("NX_ASSERT_CUSTOM", "%s", (msg));      \
                }                                                       \
        } while(0)
#endif

#if defined(NDEBUG)
#  define NX_ASSERT_RANGE(expr,min_val,max_val) do { (void)sizeof(expr); } while(0)
#else
#  define NX_ASSERT_RANGE(expr,min_val,max_val)                         \
        do {                                                            \
                if ((expr) < (min_val) || (expr) > (max_val)) {         \
                        NX_FATAL("NX_ASSERT_RANGE",                     \
                                 "Assertion '%s == %d in [%d, %d]' failed", \
                                 #expr,expr,min_val,max_val);           \
                }                                                       \
        } while(0)
#endif

#if defined(NDEBUG)
#  define NX_ASSERT_INDEX(expr,size) do { (void)sizeof(expr); } while(0)
#else
#  define NX_ASSERT_INDEX(expr,size)                                    \
                do {                                                    \
                        NX_ASSERT_RANGE(expr,0,(size-1));               \
                } while(0)
#endif


__NX_END_DECL

#endif
