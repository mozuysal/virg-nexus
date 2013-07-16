/**
 * @file nx_config.h
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
#ifndef VIRG_NEXUS_NX_CONFIG_H
#define VIRG_NEXUS_NX_CONFIG_H

#ifdef  __cplusplus
#  define __NX_BEGIN_DECL extern "C" {
#  define __NX_END_DECL   }
#else
#  define __NX_BEGIN_DECL
#  define __NX_END_DECL
#endif

#if defined(__GNUC__)
#  define __NX_NO_RETURN __attribute__((noreturn))
#  define __NX_NO_RETURN_PTR __attribute__((__noreturn__))
#elif defined(_MSC_VER)
#  define __NX_NO_RETURN __declspec(noreturn)
#  define __NX_NO_RETURN_PTR
#else
#  define __NX_NO_RETURN
#  define __NX_NO_RETURN_PTR
#  ifndef __attribute__
#    define __attribute__(x)
#  endif
#endif

#define NX_EXIT_FATAL -99

#define NX_SIMD_ALIGNMENT 32

#define NX_LOG_TAG "VIRGNEXUS"

#endif
