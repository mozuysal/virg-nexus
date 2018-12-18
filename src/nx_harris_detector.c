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

void nx_harris_deriv_images(struct NXImage **dimg, const struct NXImage *img, float sigma_win)
{
        NX_ASSERT_PTR(dimg);
        NX_ASSERT_PTR(img);

        for (int i = 0; i < 3; ++i)
                if (!dimg[i])
                        dimg[i] = nx_image_new_like(img);
                else
                        nx_image_resize_like(dimg[i], img);

}
