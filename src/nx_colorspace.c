/**
 * @file nx_colorspace.c
 *
 * Copyright (C) 2019,2020 Mustafa Ozuysal. All rights reserved.
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
#include "virg/nexus/nx_colorspace.h"

#include "virg/nexus/nx_assert.h"

#include "nx_color_map_data.h"

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

static void interpolate_color_map_data(float *rgb, float value,
                                       int n, float color_map_data[][3])
{
        int k = (n-1)*value;
        float alpha = (n-1)*value - k;

        if (k == (n-1)) {
                rgb[0] = color_map_data[k][0];
                rgb[1] = color_map_data[k][1];
                rgb[2] = color_map_data[k][2];
                return;
        }

        rgb[0] = (1.0f-alpha)*color_map_data[k][0] + alpha*color_map_data[k+1][0];
        rgb[1] = (1.0f-alpha)*color_map_data[k][1] + alpha*color_map_data[k+1][1];
        rgb[2] = (1.0f-alpha)*color_map_data[k][2] + alpha*color_map_data[k+1][2];
}

void nx_color_map_apply(uchar* red, uchar* green, uchar* blue,
                        float value, enum NXColorMap map)
{
        float rgb[3];
        switch (map) {
        case NX_COLOR_MAP_VIRIDIS:
                interpolate_color_map_data(&rgb[0], value,
                                           nx_color_map_data_viridis_size,
                                           nx_color_map_data_viridis);
                *red   = rgb[0]*255;
                *green = rgb[1]*255;
                *blue  = rgb[2]*255;
                break;
        case NX_COLOR_MAP_PLASMA:
                interpolate_color_map_data(&rgb[0], value,
                                           nx_color_map_data_plasma_size,
                                           nx_color_map_data_plasma);
                *red   = rgb[0]*255;
                *green = rgb[1]*255;
                *blue  = rgb[2]*255;
                break;
        case NX_COLOR_MAP_MAGMA:
                interpolate_color_map_data(&rgb[0], value,
                                           nx_color_map_data_magma_size,
                                           nx_color_map_data_magma);
                *red   = rgb[0]*255;
                *green = rgb[1]*255;
                *blue  = rgb[2]*255;
                break;
        case NX_COLOR_MAP_CUBEHELIX:
                interpolate_color_map_data(&rgb[0], value,
                                           nx_color_map_data_cubehelix_size,
                                           nx_color_map_data_cubehelix);
                *red   = rgb[0]*255;
                *green = rgb[1]*255;
                *blue  = rgb[2]*255;
                break;
        case NX_COLOR_MAP_GRAY:
        default:
                *red   = value*255;
                *green = value*255;
                *blue  = value*255;
                break;
        }
}
