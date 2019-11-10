/**
 * @file nx_harris_detector.c
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
#include "virg/nexus/nx_harris_detector.h"

#include "virg/nexus/nx_mem_block.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"

#define NX_HARRIS_KERNEL_TRUNCATION_FACTOR 3.0f

void nx_harris_deriv_images(struct NXImage **dimg,
                            const struct NXImage *img,
                            float sigma_win)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(dimg);
        NX_IMAGE_ASSERT_GRAYSCALE_UCHAR(img);

        for (int i = 0; i < 3; ++i)
                if (!dimg[i])
                        dimg[i] = nx_image_alloc();

        nx_image_deriv_x(dimg[0], img);
        nx_image_deriv_y(dimg[1], img);
        nx_image_resize_like(dimg[2], dimg[0]);

        for (int y = 0; y < img->height; ++y) {
                float *x2_row = dimg[0]->data.f32 + y*dimg[0]->row_stride;
                float *y2_row = dimg[1]->data.f32 + y*dimg[1]->row_stride;
                float *xy_row = dimg[2]->data.f32 + y*dimg[2]->row_stride;
                for (int x = 0; x < img->width; ++x) {
                        xy_row[x] = x2_row[x]*y2_row[x];
                        x2_row[x] *= x2_row[x];
                        y2_row[x] *= y2_row[x];
                }
        }

        if (sigma_win > 0.0f) {
                int nkx;
                int nky;
                float * buffer = nx_image_filter_buffer_alloc(img->width, img->height,
                                                              sigma_win, sigma_win,
                                                              NX_HARRIS_KERNEL_TRUNCATION_FACTOR,
                                                              &nkx, &nky);
                for (int i = 0; i < 3; ++i)
                        nx_image_smooth(dimg[i], dimg[i], sigma_win, sigma_win,
                                        NX_HARRIS_KERNEL_TRUNCATION_FACTOR, buffer);
                nx_free(buffer);
        }
}

void nx_harris_score_image(struct NXImage *simg, struct NXImage **dimg, float k)
{
        NX_ASSERT_PTR(simg);
        NX_ASSERT_PTR(dimg);
        NX_ASSERT_PTR(dimg[0]);
        NX_ASSERT_PTR(dimg[1]);
        NX_ASSERT_PTR(dimg[2]);

        int w = dimg[0]->width;
        int h = dimg[0]->height;
        nx_image_resize(simg, w, h, NX_IMAGE_STRIDE_DEFAULT,
                        NX_IMAGE_GRAYSCALE, NX_IMAGE_FLOAT32);

        for (int y = 0; y < h; ++y) {
                float* s_row = simg->data.f32 + y*simg->row_stride;
                const float *x2_row = dimg[0]->data.f32 + y*dimg[0]->row_stride;
                const float *y2_row = dimg[1]->data.f32 + y*dimg[1]->row_stride;
                const float *xy_row = dimg[2]->data.f32 + y*dimg[2]->row_stride;
                for (int x = 0; x < w; ++x) {
                        float det = x2_row[x]*y2_row[x] - xy_row[x]*xy_row[x];
                        float tr = x2_row[x] + y2_row[x];
                        s_row[x] = det - k*tr*tr;
                }
        }
}

int nx_harris_detect_keypoints(int n_keys_max, struct NXKeypoint *keys,
                               const struct NXImage *simg, float threshold)
{
        NX_ASSERT_PTR(keys);
        NX_ASSERT_PTR(simg);
        NX_IMAGE_ASSERT_GRAYSCALE(simg);
        NX_IMAGE_ASSERT_FLOAT32(simg);

        if (n_keys_max < 1)
                return 0;

        const int BORDER = 2; // no keypoints in the border
        NX_ASSERT(BORDER >= 1);

        int w = simg->width;
        int h = simg->height;
        int n = 0;
        struct NXKeypoint* key = keys;
        for (int y = BORDER; y < h - BORDER; ++y) {
                const float* s_rowm = simg->data.f32 + (y-1)*simg->row_stride;
                const float* s_row = simg->data.f32 + y*simg->row_stride;
                const float* s_rowp = simg->data.f32 + (y+1)*simg->row_stride;
                for (int x = BORDER; x < w - BORDER; ++x) {
                        if (s_row[x] > threshold &&
                            s_row[x] > s_row[x-1] && s_row[x] > s_row[x+1]
                            && s_row[x] > s_rowm[x-1] && s_row[x] > s_rowm[x]
                            && s_row[x] > s_rowm[x+1]
                            && s_row[x] > s_rowp[x-1] && s_row[x] > s_rowp[x]
                            && s_row[x] > s_rowp[x+1]) {
                                key->x = x;
                                key->y = y;
                                key->xs = x;
                                key->ys = y;
                                key->level = 0;
                                key->scale = 1.0f;
                                key->sigma = 1.0f;
                                key->score = s_row[x];
                                key->ori = 0.0f;
                                key->id = n;

                                ++n;
                                ++key;
                                if (n >= n_keys_max)
                                        return n;
                        }
                }
        }

        return n;
}
