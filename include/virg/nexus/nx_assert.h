/**
 * @file nx_assert.h
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
#ifndef VIRG_NEXUS_NX_ASSERT_H
#define VIRG_NEXUS_NX_ASSERT_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_log.h"

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
