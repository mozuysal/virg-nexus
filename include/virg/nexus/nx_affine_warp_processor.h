/**
 * @file nx_affine_warp_processor.h
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
