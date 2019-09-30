/**
 * @file nx_image_pyr.h
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
