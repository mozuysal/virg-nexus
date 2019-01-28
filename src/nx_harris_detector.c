/**
 * @file nx_harris_detector.c
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2018 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_harris_detector.h"

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"

void nx_harris_deriv_images(struct NXImage **dimg,
                            const struct NXImage *img,
                            float sigma_win)
{
        NX_ASSERT_PTR(img);
        NX_IMAGE_ASSERT_GRAYSCALE_UCHAR(img);

        if (!dimg)
                dimg = NX_NEW(3, struct NXImage *);

        nx_image_deriv_x(dimg[0], img);
        nx_image_deriv_y(dimg[1], img);

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

        float * buffer = nx_image_filter_buffer_alloc(img->width, img->height,
                                                      sigma_win, sigma_win);
        for (int i = 0; i < 3; ++i)
                nx_image_smooth(dimg[i], dimg[i], sigma_win, sigma_win, buffer);
        nx_free(buffer);
}
