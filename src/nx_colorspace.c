/**
 * @file nx_colorspace.c
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
#include "virg/nexus/nx_colorspace.h"

#include "virg/nexus/nx_assert.h"

void nx_convert_rgba_to_gray_uc(int width, int height, uchar *gray, int gray_stride,
                                const uchar *rgba, int rgba_stride)
{
        NX_ASSERT_PTR(gray);
        NX_ASSERT_PTR(rgba);

        for (int y = 0; y < height; ++y) {
                const uchar* rgba_row = rgba + y*rgba_stride;
                uchar* gray_row = gray + y*gray_stride;
                for (int x = 0; x < width; ++x) {
                        gray_row[x] = nx_rgb_to_gray_uc(rgba_row[4*x],
                                                        rgba_row[4*x+1],
                                                        rgba_row[4*x+2]);
                }
        }
}

void nx_convert_gray_to_rgba_uc(int width, int height, uchar *rgba, int rgba_stride,
                                const uchar *gray, int gray_stride)
{
        NX_ASSERT_PTR(rgba);
        NX_ASSERT_PTR(gray);

        for (int y = 0; y < height; ++y) {
                const uchar* gray_row = gray + y*gray_stride;
                uchar* rgba_row = rgba + y*rgba_stride;
                for (int x = 0; x < width; ++x) {
                        rgba_row[4*x]   = gray_row[x];
                        rgba_row[4*x+1] = gray_row[x];
                        rgba_row[4*x+2] = gray_row[x];
                        rgba_row[4*x+3] = 255;
                }
        }
}

void nx_convert_rgba_to_gray_f32(int width, int height, float *gray, int gray_stride,
                                 const float *rgba, int rgba_stride)
{
        NX_ASSERT_PTR(gray);
        NX_ASSERT_PTR(rgba);

        for (int y = 0; y < height; ++y) {
                const float* rgba_row = rgba + y*rgba_stride;
                float* gray_row = gray + y*gray_stride;
                for (int x = 0; x < width; ++x) {
                        gray_row[x] = nx_rgb_to_gray_f32(rgba_row[4*x],
                                                         rgba_row[4*x+1],
                                                         rgba_row[4*x+2]);
                }
        }
}

void nx_convert_gray_to_rgba_f32(int width, int height, float *rgba, int rgba_stride,
                                 const float *gray, int gray_stride)
{
        NX_ASSERT_PTR(rgba);
        NX_ASSERT_PTR(gray);

        for (int y = 0; y < height; ++y) {
                const float* gray_row = gray + y*gray_stride;
                float* rgba_row = rgba + y*rgba_stride;
                for (int x = 0; x < width; ++x) {
                        rgba_row[4*x]   = gray_row[x];
                        rgba_row[4*x+1] = gray_row[x];
                        rgba_row[4*x+2] = gray_row[x];
                        rgba_row[4*x+3] = 1.0f;
                }
        }
}
