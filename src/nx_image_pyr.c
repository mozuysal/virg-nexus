/**
 * @file nx_image_pyr.c
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
#include <virg/nexus/nx_image_pyr.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <virg/nexus/nx_alloc.h>
#include <virg/nexus/nx_message.h>
#include <virg/nexus/nx_assert.h>

struct NXImagePyr *nx_image_pyr_alloc()
{
        struct NXImagePyr *pyr = NX_NEW(1, struct NXImagePyr);

        pyr->n_levels = 0;
        pyr->levels = NULL;

        return pyr;
}

void nx_image_pyr_free(struct NXImagePyr *pyr)
{
        if (pyr) {
                nx_image_pyr_free_levels(pyr);
                nx_free(pyr);
        }
}

void nx_image_pyr_alloc_levels(struct NXImagePyr *pyr, int n_levels) {
        NX_ASSERT_PTR(pyr);
        NX_ASSERT(n_levels >= 1);

        if (pyr->n_levels != n_levels) {
                nx_image_pyr_free_levels(pyr);
        }

        if (pyr->levels == NULL) {
                pyr->levels = NX_NEW(n_levels, struct NXImagePyrLevel);
                for (int i = 0; i < n_levels; ++i) {
                        pyr->levels[i].img = nx_image_alloc();
                }
                pyr->n_levels = n_levels;
        }
}

void nx_image_pyr_free_levels(struct NXImagePyr *pyr)
{
        NX_ASSERT_PTR(pyr);

        for (int i = 0; i < pyr->n_levels; ++i) {
                nx_image_free(pyr->levels[i].img);
        }
        nx_free(pyr->levels);

        pyr->levels = NULL;
        pyr->n_levels = 0;
}

void nx_image_pyr_create_level(struct NXImagePyr *pyr, int level_id, int width, int height, float scale, float sigma)
{
        NX_ASSERT_PTR(pyr);
        NX_ASSERT(level_id < pyr->n_levels);

        struct NXImagePyrLevel *level = pyr->levels + level_id;

        nx_image_resize(level->img, width, height, 0, NX_IMAGE_GRAYSCALE);

        level->scale = scale;
        level->sigma = sigma;
}

void nx_image_pyr_copy_to_level0(struct NXImagePyr *pyr, const struct NXImage *image, float initial_sigma)
{
        NX_ASSERT_PTR(pyr);
        NX_ASSERT_PTR(image);
        NX_ASSERT_PTR(pyr->levels);

        nx_image_pyr_create_level(pyr, 0, image->width, image->height, 1.0f, initial_sigma);
        nx_image_copy(pyr->levels[0].img, image);
}
