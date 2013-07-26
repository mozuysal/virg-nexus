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

#define NX_IMAGE_PYR_INIT_SIGMA 0.5f

enum NXImagePyrType {
        NX_IMAGE_PYR_FAST = 0,
        NX_IMAGE_PYR_FINE,
        NX_IMAGE_PYR_SCALED
};

struct NXImagePyrLevel
{
        struct NXImage *img;
        float sigma;
        float scale;
};

struct NXImagePyrFastInfo {
        int width0;
        int height0;
        float sigma0;
};

struct NXImagePyrFineInfo {
        int width0;
        int height0;
        int n_octaves;
        int n_octave_steps;
        float sigma0;
};

struct NXImagePyrScaledInfo {
        int width0;
        int height0;
        float scale_factor;
        float sigma0;
};

union NXImagePyrInfo {
       struct NXImagePyrFastInfo fast;
       struct NXImagePyrFineInfo fine;
       struct NXImagePyrScaledInfo scaled;
};

struct NXImagePyr
{
        int n_levels;
        struct NXImagePyrLevel *levels;

        enum NXImagePyrType type;
        union NXImagePyrInfo info;

        struct NXImage *work_img;
};

struct NXImagePyr *nx_image_pyr_alloc();

struct NXImagePyr *nx_image_pyr_new_fast(int width0, int height0, int n_levels, float sigma0);

struct NXImagePyr *nx_image_pyr_new_fine(int width0, int height0, int n_octaves, int n_octave_steps, float sigma0);

struct NXImagePyr *nx_image_pyr_new_scaled(int width0, int height0, int n_levels, float scale_factor, float sigma0);

void nx_image_pyr_free(struct NXImagePyr *pyr);

void nx_image_pyr_resize_fast(struct NXImagePyr *pyr, int width0, int height0, int n_levels, float sigma0);

void nx_image_pyr_resize_fine(struct NXImagePyr *pyr, int width0, int height0, int n_octaves, int n_octave_steps, float sigma0);

void nx_image_pyr_resize_scaled(struct NXImagePyr *pyr, int width0, int height0, int n_levels, float scale_factor, float sigma0);

void nx_image_pyr_release(struct NXImagePyr *pyr);

void nx_image_pyr_compute(struct NXImagePyr *pyr, const struct NXImage *img);

void nx_image_pyr_update(struct NXImagePyr *pyr);

struct NXImagePyr *nx_image_pyr_copy0(const struct NXImagePyr *pyr);

void nx_image_pyr_copy(struct NXImagePyr *dest, const struct NXImagePyr *src);

__NX_END_DECL

#endif
