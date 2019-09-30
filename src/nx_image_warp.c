/**
 * @file nx_image_warp.c
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
#include "virg/nexus/nx_image_warp.h"

#include <float.h>
#include <math.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_log.h"
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

static void nx_image_rotate45_uc(struct NXImage *dest, struct NXImage *src, NXBool cw)
{
        NX_ASSERT_PTR(dest);
        NX_ASSERT_PTR(src);
        NX_IMAGE_ASSERT_GRAYSCALE_UCHAR(src);
        NX_IMAGE_ASSERT_GRAYSCALE_UCHAR(dest);

        const float CW = cw ? +1.0f : -1.0f;
        const float SQRT2_INV = 1.0f / sqrt(2.0);
        const float TX = (src->width  - SQRT2_INV*(dest->width+CW*dest->height))*0.5f;
        const float TY = (src->height + SQRT2_INV*(CW*dest->width-dest->height))*0.5f;
        const int LAST_X = src->width - 1;
        const int LAST_Y = src->height - 1;

        for (int y = 0; y < dest->height; ++y) {
                uchar *drow = dest->data.uc + y*dest->row_stride;
                float xp = CW*y*SQRT2_INV + TX;
                float yp = y*SQRT2_INV + TY;
                for (int x = 0; x < dest->width;
                     ++x, xp += SQRT2_INV, yp -= CW*SQRT2_INV) {
                        int xpi = xp;
                        int ypi = yp;

                        float u = xp-xpi;
                        float v = yp-ypi;
                        float up = 1.0f - u;
                        float vp = 1.0f - v;

                        int idx[2] = { xpi, xpi + 1};
                        int idy[2] = { ypi, ypi + 1};

                        if (idx[0] < 0 || idx[0] >= LAST_X
                            || idy[0] < 0 || idy[0] >= LAST_Y) {
                                drow[x] = 0;
                                continue;
                        }
                        const uchar *p0 = src->data.uc + idy[0]*src->row_stride;
                        const uchar *p1 = src->data.uc + idy[1]*src->row_stride;
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

static void nx_image_rotate45_f32(struct NXImage *dest, struct NXImage *src, NXBool cw)
{
        NX_ASSERT_PTR(dest);
        NX_ASSERT_PTR(src);
        NX_IMAGE_ASSERT_GRAYSCALE_FLOAT32(dest);
        NX_IMAGE_ASSERT_GRAYSCALE_FLOAT32(src);

        const float CW = cw ? +1.0f : -1.0f;
        const float SQRT2_INV = 1.0f / sqrt(2.0);
        const float TX = (src->width  - SQRT2_INV*(dest->width+CW*dest->height))*0.5f;
        const float TY = (src->height + SQRT2_INV*(CW*dest->width-dest->height))*0.5f;
        const int LAST_X = src->width - 1;
        const int LAST_Y = src->height - 1;

        for (int y = 0; y < dest->height; ++y) {
                float *drow = dest->data.f32 + y*dest->row_stride;
                float xp = CW*y*SQRT2_INV + TX;
                float yp = y*SQRT2_INV + TY;
                for (int x = 0; x < dest->width;
                     ++x, xp += SQRT2_INV, yp -= CW*SQRT2_INV) {
                        int xpi = xp;
                        int ypi = yp;

                        float u = xp-xpi;
                        float v = yp-ypi;
                        float up = 1.0f - u;
                        float vp = 1.0f - v;

                        int idx[2] = { xpi, xpi + 1};
                        int idy[2] = { ypi, ypi + 1};

                        if (idx[0] < 0 || idx[0] >= LAST_X
                            || idy[0] < 0 || idy[0] >= LAST_Y) {
                                drow[x] = 0;
                                continue;
                        }
                        const float *p0 = src->data.f32 + idy[0]*src->row_stride;
                        const float *p1 = src->data.f32 + idy[1]*src->row_stride;
                        int I = vp*(up*p0[idx[0]] + u*p0[idx[1]])
                                + v*(up*p1[idx[0]] + u*p1[idx[1]]);

                        drow[x] = I;
                }
        }
}

void nx_image_rotate45(struct NXImage *dest, struct NXImage *src, NXBool cw)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);
        NX_IMAGE_ASSERT_GRAYSCALE(dest);

        switch (src->dtype) {
        case NX_IMAGE_UCHAR: nx_image_rotate45_uc(dest, src, cw); break;
        case NX_IMAGE_FLOAT32: nx_image_rotate45_f32(dest, src, cw); break;
        default: NX_FATAL(NX_LOG_TAG, "Unhandled switch case for image data type");
        }
}
