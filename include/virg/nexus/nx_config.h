/**
 * @file nx_config.h
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
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
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
#  define __NX_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#  define __NX_NO_RETURN __declspec(noreturn)
#  define __NX_NO_RETURN_PTR
#  define __NX_FUNCTION __func__
#else
#  define __NX_NO_RETURN
#  define __NX_NO_RETURN_PTR
#  ifndef __attribute__
#    define __attribute__(x)
#  endif
#endif

#define NX_EXIT_FATAL -99

#define NX_SIMD_ALIGNMENT 32

#define NX_LOG_TAG "NX"
#define VG_LOG_TAG "VG"

#define NX_VERSION_MAJOR 0
#define NX_VERSION_MINOR 1


#endif
