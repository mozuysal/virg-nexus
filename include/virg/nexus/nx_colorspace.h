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

inline uchar nx_rgb_to_gray(uchar r, uchar g, uchar b);

void nx_convert_rgba_to_gray(int width, int height, uchar *gray, int gray_stride,
                             const uchar *rgba, int rgba_stride);

void nx_convert_gray_to_rgba(int width, int height, uchar *rgba, int rgba_stride,
                             const uchar *gray, int gray_stride);

uchar nx_rgb_to_gray(uchar r, uchar g, uchar b)
{
        int gray = r*0.3f + g*0.59f + b*0.11f;

        if (gray < 0)
                return 0;
        else if (gray > 255)
                return 255;
        else
                return gray;
}

__NX_END_DECL

#endif
