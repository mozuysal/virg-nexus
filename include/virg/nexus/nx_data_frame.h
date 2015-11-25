/**
 * @file nx_data_frame.h
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
#ifndef VIRG_NEXUS_NX_DATA_FRAME_H
#define VIRG_NEXUS_NX_DATA_FRAME_H

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

struct NXDataFrame;

struct NXDataFrame *nx_data_frame_alloc();

void nx_data_frame_free(struct NXDataFrame *df);

__NX_END_DECL

#endif
