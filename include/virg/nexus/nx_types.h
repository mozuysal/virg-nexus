/**
 * @file nx_types.h
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
