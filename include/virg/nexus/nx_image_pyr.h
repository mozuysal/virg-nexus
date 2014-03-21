/**
 * @file nx_image_pyr.h
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
#ifndef VIRG_NEXUS_NX_IMAGE_PYR_H
#define VIRG_NEXUS_NX_IMAGE_PYR_H

#include "virg/nexus/nx_config.h"

#include "virg/nexus/nx_image.h"

__NX_BEGIN_DECL

struct NXImagePyrLevel
{
        struct NXImage *img;
        float sigma;
        float scale;
};

struct NXImagePyr
{
        int n_levels;
        struct NXImagePyrLevel *levels;
};

struct NXImagePyr *nx_image_pyr_alloc();

void nx_image_pyr_free(struct NXImagePyr *pyr);

void nx_image_pyr_alloc_levels(struct NXImagePyr *pyr, int n_levels);

void nx_image_pyr_free_levels(struct NXImagePyr *pyr);

void nx_image_pyr_create_level(struct NXImagePyr *pyr, int level_id, int width, int height, float scale, float sigma);

void nx_image_pyr_copy_to_level0(struct NXImagePyr *pyr, const struct NXImage *image, float initial_sigma);

__NX_END_DECL

#endif
