/**
 * @file nx_types.h
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
 * along with VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_TYPES_H
#define VIRG_NEXUS_NX_TYPES_H

#include "virg/nexus/nx_config.h"

#ifdef VIRG_NEXUS_PC_WIN32
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
#  include <inttypes.h>
#endif

__NX_BEGIN_DECL

typedef unsigned char uchar;

typedef int NXBool;

#define NX_FALSE 0
#define NX_TRUE  1

typedef int NXResult;

#define NX_OK 0
#define NX_FAIL -1

__NX_END_DECL

#endif
