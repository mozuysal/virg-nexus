/**
 * @file nx_image_warp.h
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
#ifndef VIRG_NEXUS_NX_IMAGE_WARP_H
#define VIRG_NEXUS_NX_IMAGE_WARP_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

enum NXImageWarpBackgroundMode {
        NX_IMAGE_WARP_BLACK = 0,
        NX_IMAGE_WARP_WHITE,
        NX_IMAGE_WARP_NOISE,
        NX_IMAGE_WARP_REPEAT,
        NX_IMAGE_WARP_MIRROR
};

void nx_image_warp_affine_bilinear(int dest_w, int dest_h, uchar *dest, int dest_stride,
                                   int src_w, int src_h, const uchar *src, int src_stride,
                                   const float *t_dest2src, enum NXImageWarpBackgroundMode bg_mode);

__NX_END_DECL

#endif
