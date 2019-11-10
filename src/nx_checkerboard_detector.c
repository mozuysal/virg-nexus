/**
 * @file nx_checkerboard_detector.c
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
#include "virg/nexus/nx_checkerboard_detector.h"

#include <stdlib.h>
#include <math.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_image_warp.h"
#include "virg/nexus/nx_filter.h"
#include "virg/nexus/nx_math.h"

void nx_checkers_local_radon_images(struct NXImage **lr_images,
                                    struct NXImage *img, int sum_radius)
{
        for (int i = 0; i < 4; ++i) {
                if (!lr_images[i])
                        lr_images[i] = nx_image_alloc();
                nx_image_resize(lr_images[i], img->width, img->height,
                                NX_IMAGE_STRIDE_DEFAULT,
                                NX_IMAGE_GRAYSCALE, NX_IMAGE_UCHAR);
        }

        int temp_dim = (img->width + img->height + 2) / sqrt(2.0);
        int dim = nx_max_i(temp_dim, nx_max_i(img->width, img->height));
        float *buffer = nx_filter_buffer_alloc(dim, sum_radius);

        nx_image_filter_box_x(lr_images[0], img, sum_radius, buffer);
        nx_image_filter_box_y(lr_images[1], img, sum_radius, buffer);

        struct NXImage *temp = nx_image_alloc();
        nx_image_resize(temp, temp_dim, temp_dim, NX_IMAGE_STRIDE_DEFAULT,
                        NX_IMAGE_GRAYSCALE, NX_IMAGE_UCHAR);

        nx_image_rotate45(temp, img, NX_TRUE);
        nx_image_filter_box_x(temp, temp, sum_radius, buffer);
        nx_image_rotate45(lr_images[2], temp, NX_FALSE);

        nx_image_rotate45(temp, img, NX_FALSE);
        nx_image_filter_box_x(temp, temp, sum_radius, buffer);
        nx_image_rotate45(lr_images[3], temp, NX_TRUE);

        nx_free(buffer);
        nx_image_free(temp);
}

static inline int min4_array_uc(uchar **arr, int x)
{
        return nx_min_i(nx_min_i(arr[0][x], arr[1][x]),
                        nx_min_i(arr[2][x], arr[3][x]));
}

static inline int max4_array_uc(uchar **arr, int x)
{

        return nx_max_i(nx_max_i(arr[0][x], arr[1][x]),
                        nx_max_i(arr[2][x], arr[3][x]));
}

void nx_checkers_score_image(struct NXImage *score_img,
                             struct NXImage **lr_images)
{
        int w = lr_images[0]->width;
        int h = lr_images[0]->height;
        nx_image_resize(score_img, w, h, NX_IMAGE_STRIDE_DEFAULT,
                        NX_IMAGE_GRAYSCALE, NX_IMAGE_FLOAT32);
        for (int y = 0; y < h; ++y) {
                float *s_row = score_img->data.f32 + y*score_img->row_stride;
                uchar *lr_row[4];
                for (int i = 0; i < 4; ++i)
                        lr_row[i] = lr_images[i]->data.uc
                                + y*lr_images[i]->row_stride;
                for (int x = 0; x < w; ++x) {
                        float lr_min = min4_array_uc(&lr_row[0], x)/255.0;
                        float lr_max = max4_array_uc(&lr_row[0], x)/255.0;
                        float d = lr_max - lr_min;
                        s_row[x] = d*d;
                }
        }
        nx_image_smooth(score_img, score_img, 1.3f, 1.3f, 3.0f, NULL);
}
