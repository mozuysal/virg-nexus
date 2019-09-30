/**
 * @file nx_affine_warp_processor.h
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
#ifndef VIRG_NEXUS_NX_AFFINE_WARP_PROCESSOR_H
#define VIRG_NEXUS_NX_AFFINE_WARP_PROCESSOR_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_image.h"

__NX_BEGIN_DECL

struct NXAffineWarpParam {
        double scale;
        double planar_angle;
        double tilt;
        double tilt_angle;
};

struct NXAffineWarpProcessor;

struct NXAffineWarpProcessor *nx_affine_warp_processor_new();
void nx_affine_warp_processor_free(struct NXAffineWarpProcessor *wp);

void nx_affine_warp_processor_warp(struct NXAffineWarpProcessor *wp, const struct NXImage *img, struct NXAffineWarpParam param);

const struct NXImage *nx_affine_warp_processor_warp_result(struct NXAffineWarpProcessor *wp);
const double *nx_affine_warp_processor_forward_transform(struct NXAffineWarpProcessor *wp);
const double *nx_affine_warp_processor_inverse_transform(struct NXAffineWarpProcessor *wp);

void nx_affine_warp_processor_set_bg_fixed(struct NXAffineWarpProcessor *wp, uchar bg_color);
void nx_affine_warp_processor_set_bg_repeat(struct NXAffineWarpProcessor *wp);
void nx_affine_warp_processor_set_bg_noise(struct NXAffineWarpProcessor *wp);

void nx_affine_warp_processor_set_post_blur_sigma(struct NXAffineWarpProcessor *wp, float sigma);

__NX_END_DECL

#endif
