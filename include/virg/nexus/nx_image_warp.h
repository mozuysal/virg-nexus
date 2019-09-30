/**
 * @file nx_image_warp.h
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
#ifndef VIRG_NEXUS_NX_IMAGE_WARP_H
#define VIRG_NEXUS_NX_IMAGE_WARP_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_image.h"

__NX_BEGIN_DECL

enum NXImageWarpBackgroundMode {
        NX_IMAGE_WARP_BLACK = 0,
        NX_IMAGE_WARP_WHITE,
        NX_IMAGE_WARP_NOISE,
        NX_IMAGE_WARP_REPEAT,
        NX_IMAGE_WARP_MIRROR
};

void nx_image_rotate45(struct NXImage *dest, struct NXImage *src, NXBool cw);

void nx_image_warp_affine_bilinear(int dest_w, int dest_h, uchar *dest, int dest_stride,
                                   int src_w, int src_h, const uchar *src, int src_stride,
                                   const float *t_dest2src, enum NXImageWarpBackgroundMode bg_mode);

__NX_END_DECL

#endif
