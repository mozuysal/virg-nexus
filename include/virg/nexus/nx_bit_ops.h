/**
 * @file nx_bit_ops.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2015 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_BIT_OPS_H
#define VIRG_NEXUS_NX_BIT_OPS_H

#include <inttypes.h>

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

int nx_bit_count32(uint32_t x);
int nx_bit_count64(uint64_t x);

__NX_END_DECL

#endif
