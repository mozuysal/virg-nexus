/**
 * @file nx_image_warp.c
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
#include "virg/nexus/nx_image_warp.h"

#include <float.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_message.h"
#include "virg/nexus/nx_uniform_sampler.h"

void nx_image_warp_affine_bilinear(int dest_w, int dest_h, uchar *dest, int dest_stride,
                                   int src_w, int src_h, const uchar *src, int src_stride,
                                   const float *t_dest2src, enum NXImageWarpBackgroundMode bg_mode)
{
        NX_ASSERT_PTR(dest);
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(t_dest2src);

        const int LAST_X = src_w - 1;
        const int LAST_Y = src_h - 1;
        const int W2 = src_w * 2 - 2;
        const int H2 = src_h * 2 - 2;

        uchar bg_fixed = 0;
        if (bg_mode == NX_IMAGE_WARP_WHITE)
                bg_fixed = 255;

        const float *t = t_dest2src;
        for (int y = 0; y < dest_h; ++y) {
                uchar *drow = dest + y*dest_stride;

                float xp = y*t[3] + t[6];
                float yp = y*t[4] + t[7];
                for (int x = 0; x < dest_w; ++x, xp += t[0], yp += t[1]) {

                        /* if (x == 256 && y == 256) { */
                                /* nx_error("NXDBG", "(x, y) -> (xp, yp): (%d, %d) -> (%.4f, %.4f)\n", */
                                         /* x, y, xp, yp); */
                        /* } */

                        int xpi = xp;
                        int ypi = yp;

                        float u = xp-xpi;
                        float v = yp-ypi;
                        float up = 1.0f - u;
                        float vp = 1.0f - v;

                        int idx[2] = { xpi, xpi + 1};
                        int idy[2] = { ypi, ypi + 1};

                        switch (bg_mode) {
                        default:
                        case NX_IMAGE_WARP_BLACK:
                        case NX_IMAGE_WARP_WHITE:
                                if (idx[0] < 0 || idx[0] >= LAST_X
                                    || idy[0] < 0 || idy[0] >= LAST_Y) {
                                        drow[x] = bg_fixed;
                                        continue;
                                }
                                break;
                        case NX_IMAGE_WARP_NOISE:
                                if (idx[0] < 0 || idx[0] >= LAST_X
                                    || idy[0] < 0 || idy[0] >= LAST_Y) {
                                        drow[x] = 255.0f * NX_UNIFORM_SAMPLE_S;
                                        continue;
                                }
                                break;
                        case NX_IMAGE_WARP_REPEAT:
                                if (idx[0] < 0) {
                                        idx[0] = 0;
                                        idx[1] = 0;
                                } else if (idx[0] >= LAST_X) {
                                        idx[0] = LAST_X;
                                        idx[1] = LAST_X;
                                }

                                if (idy[0] < 0) {
                                        idy[0] = 0;
                                        idy[1] = 0;
                                } else if (idy[0] >= LAST_Y) {
                                        idy[0] = LAST_Y;
                                        idy[1] = LAST_Y;
                                }
                                break;
                        case NX_IMAGE_WARP_MIRROR:
                                if (idx[0] < 0) {
                                        if (idx[0] < 0) idx[0] =  -idx[0] - W2 * (-idx[0] / W2);
                                        if (idx[1] < 0) idx[1] =  -idx[1] - W2 * (-idx[1] / W2);
                                } else if (idx[0] >= LAST_X) {
                                        if (idx[0] >= src_w) idx[0] = W2 - idx[0] - W2 * (idx[0] / W2);
                                        if (idx[1] >= src_w) idx[1] = W2 - idx[1] - W2 * (idx[1] / W2);
                                }

                                if (idy[0] < 0) {
                                        if (idy[0] < 0) idy[0] =  -idy[0] - H2 * (-idy[0] / H2);
                                        if (idy[1] < 0) idy[1] =  -idy[1] - H2 * (-idy[1] / H2);
                                } else if (idy[0] >= LAST_Y) {
                                        if (idy[0] >= src_h) idy[0] = H2 - idy[0] - H2 * (idy[0] / H2);
                                        if (idy[1] >= src_h) idy[1] = H2 - idy[1] - H2 * (idy[1] / H2);
                                }
                                break;
                        }

                        const uchar *p0 = src + idy[0]*src_stride;
                        const uchar *p1 = src + idy[1]*src_stride;
                        int I = vp*(up*p0[idx[0]] + u*p0[idx[1]])
                                + v*(up*p1[idx[0]] + u*p1[idx[1]]);

                        if (I < 0)
                                I = 0;
                        else if (I > 255)
                                I = 255;

                        drow[x] = I;
                }
        }
}
