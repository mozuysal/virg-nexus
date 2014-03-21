/**
 * @file nx_image_pyr_builder.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2014 Izmir Institute of Technology. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_IMAGE_PYR_BUILDER_H
#define VIRG_NEXUS_NX_IMAGE_PYR_BUILDER_H

#include "virg/nexus/nx_config.h"
#include <virg/nexus/nx_image.h>
#include "virg/nexus/nx_image_pyr.h"

__NX_BEGIN_DECL

#define NX_IMAGE_PYR_BUILDER_INITIAL_SIGMA 0.5f

enum NXImagePyrBuilderType {
        NX_IMAGE_PYR_BUILDER_NONE = -1,
        NX_IMAGE_PYR_BUILDER_FAST = 0,
        NX_IMAGE_PYR_BUILDER_FINE,
        NX_IMAGE_PYR_BUILDER_SCALED
};

struct NXImagePyrInfo {
        int n_levels;

        int n_octaves;
        int n_octave_steps;

        float scale_factor;

        float sigma0;
};

struct NXImagePyrBuilder
{
        enum NXImagePyrBuilderType type;
        struct NXImagePyrInfo pyr_info;
        struct NXImage *work_img;
};

struct NXImagePyrBuilder *nx_image_pyr_builder_alloc();

struct NXImagePyrBuilder *nx_image_pyr_builder_new_fast(int n_levels, float sigma0);

struct NXImagePyrBuilder *nx_image_pyr_builder_new_fine(int n_octaves, int n_octave_steps, float sigma0);

struct NXImagePyrBuilder *nx_image_pyr_builder_new_scaled(int n_levels, float scale_factor, float sigma0);

void nx_image_pyr_builder_free(struct NXImagePyrBuilder *builder);

void nx_image_pyr_builder_set_fast(struct NXImagePyrBuilder *builder, int n_levels, float sigma0);

void nx_image_pyr_builder_set_fine(struct NXImagePyrBuilder *builder, int n_octaves, int n_octave_steps, float sigma0);

void nx_image_pyr_builder_set_scaled(struct NXImagePyrBuilder *builder, int n_levels, float scale_factor, float sigma0);

struct NXImagePyr *nx_image_pyr_builder_build0(struct NXImagePyrBuilder *builder, const struct NXImage *img);

void nx_image_pyr_builder_build(struct NXImagePyrBuilder *builder, struct NXImagePyr *pyr, const struct NXImage *img);

void nx_image_pyr_builder_update(struct NXImagePyrBuilder *builder, struct NXImagePyr *pyr);

void nx_image_pyr_builder_init_levels(struct NXImagePyrBuilder *builder, struct NXImagePyr *pyr, int width0, int height0);

__NX_END_DECL

#endif
