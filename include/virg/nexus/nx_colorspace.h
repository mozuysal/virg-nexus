/**
 * @file nx_colorspace.h
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
#ifndef VIRG_NEXUS_NX_COLORSPACE_H
#define VIRG_NEXUS_NX_COLORSPACE_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

void nx_convert_rgba_to_gray(int width, int height, uchar *gray, int gray_stride,
                             const uchar *rgba, int rgba_stride);

void nx_convert_gray_to_rgba(int width, int height, uchar *rgba, int rgba_stride,
                             const uchar *gray, int gray_stride);

__NX_END_DECL

#endif
