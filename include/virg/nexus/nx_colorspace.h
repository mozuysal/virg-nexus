/**
 * @file nx_colorspace.h
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
#ifndef VIRG_NEXUS_NX_COLORSPACE_H
#define VIRG_NEXUS_NX_COLORSPACE_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

static inline uchar nx_rgb_to_gray_uc(uchar r, uchar g, uchar b)
{
        int gray = r*0.3f + g*0.59f + b*0.11f;

        if (gray < 0)
                return 0;
        else if (gray > 255)
                return 255;
        else
                return gray;
}

static inline float nx_rgb_to_gray_f32(float r, float g, float b)
{
        float gray = r*0.3f + g*0.59f + b*0.11f;

        if (gray < 0.0f)
                return 0.0f;
        else if (gray > 1.0f)
                return 1.0f;
        else
                return gray;
}

void nx_convert_rgba_to_gray_uc(int width, int height, uchar *gray, int gray_stride,
                                const uchar *rgba, int rgba_stride);

void nx_convert_gray_to_rgba_uc(int width, int height, uchar *rgba, int rgba_stride,
                                const uchar *gray, int gray_stride);

void nx_convert_rgba_to_gray_f32(int width, int height, float *gray, int gray_stride,
                                 const float *rgba, int rgba_stride);

void nx_convert_gray_to_rgba_f32(int width, int height, float *rgba, int rgba_stride,
                                 const float *gray, int gray_stride);

enum NXColorMap {
        NX_COLOR_MAP_GRAY = 0,
        NX_COLOR_MAP_VIRIDIS,
        NX_COLOR_MAP_PLASMA,
        NX_COLOR_MAP_MAGMA,
        NX_COLOR_MAP_CUBEHELIX
};

void nx_color_map_apply(uchar* red, uchar* green, uchar* blue,
                        float value, enum NXColorMap map);

__NX_END_DECL

#endif
