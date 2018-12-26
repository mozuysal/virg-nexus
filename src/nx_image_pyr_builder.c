/**
 * @file nx_image_pyr_builder.c
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2014 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include <virg/nexus/nx_image_pyr_builder.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <virg/nexus/nx_alloc.h>
#include <virg/nexus/nx_log.h>
#include <virg/nexus/nx_assert.h>

static void _nx_image_pyr_builder_update_fast(struct NXImagePyrBuilder *builder, struct NXImagePyr *pyr);
static void _nx_image_pyr_builder_update_fine(struct NXImagePyrBuilder *builder, struct NXImagePyr *pyr);
static void _nx_image_pyr_builder_update_scaled(struct NXImagePyrBuilder *builder, struct NXImagePyr *pyr);

static inline float compute_sigma_g(float sigma_current, float sigma_desired);

struct NXImagePyrBuilder *nx_image_pyr_builder_alloc()
{
        struct NXImagePyrBuilder *builder = NX_NEW(1, struct NXImagePyrBuilder);

        builder->type = NX_IMAGE_PYR_BUILDER_NONE;
        memset(&builder->pyr_info, 0, sizeof(struct NXImagePyrInfo));
        builder->work_img = nx_image_alloc();

        return builder;
}

struct NXImagePyrBuilder *nx_image_pyr_builder_new_fast(int n_levels, float sigma0)
{
        NX_ASSERT(n_levels > 0);

        struct NXImagePyrBuilder *builder = nx_image_pyr_builder_alloc();
        nx_image_pyr_builder_set_fast(builder, n_levels, sigma0);

        return builder;
}

struct NXImagePyrBuilder *nx_image_pyr_builder_new_fine(int n_octaves, int n_octave_steps, float sigma0)
{
        NX_ASSERT(n_octaves > 0);
        NX_ASSERT(n_octave_steps > 0);

        struct NXImagePyrBuilder *builder = nx_image_pyr_builder_alloc();
        nx_image_pyr_builder_set_fine(builder, n_octaves, n_octave_steps, sigma0);

        return builder;
}

struct NXImagePyrBuilder *nx_image_pyr_builder_new_scaled(int n_levels, float scale_factor, float sigma0)
{
        NX_ASSERT(n_levels > 0);

        struct NXImagePyrBuilder *builder = nx_image_pyr_builder_alloc();
        nx_image_pyr_builder_set_scaled(builder, n_levels, scale_factor, sigma0);

        return builder;
}

void nx_image_pyr_builder_free(struct NXImagePyrBuilder *builder)
{
        if (builder) {
                nx_image_free(builder->work_img);
                nx_free(builder);
        }
}

void nx_image_pyr_builder_set_fast(struct NXImagePyrBuilder *builder, int n_levels, float sigma0)
{
        NX_ASSERT_PTR(builder);
        NX_ASSERT(n_levels > 0);

        if (sigma0 < NX_IMAGE_PYR_BUILDER_INITIAL_SIGMA)
                sigma0 = NX_IMAGE_PYR_BUILDER_INITIAL_SIGMA;

        builder->type = NX_IMAGE_PYR_BUILDER_FAST;

        builder->pyr_info.n_levels = n_levels;
        builder->pyr_info.n_octaves = n_levels;
        builder->pyr_info.n_octave_steps = 1;
        builder->pyr_info.scale_factor = 2.0f;
        builder->pyr_info.sigma0 = sigma0;
}

void nx_image_pyr_builder_set_fine(struct NXImagePyrBuilder *builder, int n_octaves, int n_octave_steps, float sigma0)
{
        NX_ASSERT_PTR(builder);
        NX_ASSERT(n_octaves > 0);
        NX_ASSERT(n_octave_steps > 0);

        if (sigma0 < NX_IMAGE_PYR_BUILDER_INITIAL_SIGMA)
                sigma0 = NX_IMAGE_PYR_BUILDER_INITIAL_SIGMA;

        builder->type = NX_IMAGE_PYR_BUILDER_FINE;

        builder->pyr_info.n_levels = n_octaves * n_octave_steps;
        builder->pyr_info.n_octaves = n_octaves;
        builder->pyr_info.n_octave_steps = n_octave_steps;
        builder->pyr_info.scale_factor = 2.0f;
        builder->pyr_info.sigma0 = sigma0;
}

void nx_image_pyr_builder_set_scaled(struct NXImagePyrBuilder *builder, int n_levels, float scale_factor, float sigma0)
{
        NX_ASSERT_PTR(builder);
        NX_ASSERT(n_levels > 0);

        if (sigma0 < NX_IMAGE_PYR_BUILDER_INITIAL_SIGMA)
                sigma0 = NX_IMAGE_PYR_BUILDER_INITIAL_SIGMA;

        builder->type = NX_IMAGE_PYR_BUILDER_SCALED;

        builder->pyr_info.n_levels = n_levels;
        builder->pyr_info.n_octaves = -1;
        builder->pyr_info.n_octave_steps = 1;
        builder->pyr_info.scale_factor = scale_factor;
        builder->pyr_info.sigma0 = sigma0;
}

struct NXImagePyr *nx_image_pyr_builder_build0(struct NXImagePyrBuilder *builder, const struct NXImage *img)
{
        NX_ASSERT_PTR(builder);
        NX_ASSERT(builder->type != NX_IMAGE_PYR_BUILDER_NONE);
        NX_ASSERT_PTR(img);

        struct NXImagePyr *pyr = nx_image_pyr_alloc();
        nx_image_pyr_builder_build(builder, pyr, img);

        return pyr;
}

void nx_image_pyr_builder_build(struct NXImagePyrBuilder *builder, struct NXImagePyr *pyr, const struct NXImage *img)
{
        NX_ASSERT_PTR(builder);
        NX_ASSERT(builder->type != NX_IMAGE_PYR_BUILDER_NONE);
        NX_ASSERT_PTR(pyr);
        NX_ASSERT_PTR(img);
        NX_IMAGE_ASSERT_GRAYSCALE(img);

        nx_image_pyr_alloc_levels(pyr, builder->pyr_info.n_levels);
        nx_image_pyr_copy_to_level0(pyr, img, builder->pyr_info.sigma0);
        nx_image_pyr_builder_update(builder, pyr);
}

void nx_image_pyr_builder_update(struct NXImagePyrBuilder *builder, struct NXImagePyr *pyr)
{
        NX_ASSERT_PTR(builder);
        NX_ASSERT(builder->type != NX_IMAGE_PYR_BUILDER_NONE);
        NX_ASSERT_PTR(pyr);
        NX_ASSERT_PTR(pyr->levels);
        NX_ASSERT(pyr->n_levels == builder->pyr_info.n_levels);
        NX_ASSERT_PTR(pyr->levels[0].img);

        int w0 = pyr->levels[0].img->width;
        int h0 = pyr->levels[0].img->height;
        nx_image_pyr_builder_init_levels(builder, pyr, w0, h0);

        switch (builder->type) {
        case NX_IMAGE_PYR_BUILDER_FAST:
                _nx_image_pyr_builder_update_fast(builder, pyr);
                break;
        case NX_IMAGE_PYR_BUILDER_FINE:
                _nx_image_pyr_builder_update_fine(builder, pyr);
                break;
        case NX_IMAGE_PYR_BUILDER_SCALED:
                _nx_image_pyr_builder_update_scaled(builder, pyr);
                break;
        default:
                NX_FATAL(NX_LOG_TAG, "Can not update image pyramid with unknown builder type %d!",
                         (int)builder->type);
        }
}

void nx_image_pyr_builder_init_levels(struct NXImagePyrBuilder *builder, struct NXImagePyr *pyr, int width0, int height0)
{
        NX_ASSERT_PTR(builder);
        NX_ASSERT_PTR(pyr);
        NX_ASSERT(builder->type != NX_IMAGE_PYR_BUILDER_NONE);
        NX_ASSERT(width0 > 0);
        NX_ASSERT(height0 > 0);

        nx_image_pyr_alloc_levels(pyr, builder->pyr_info.n_levels);

        for (int i = 0; i < pyr->n_levels; ++i) {
                double scale_exp = i / (double)builder->pyr_info.n_octave_steps;
                float sigma_i = builder->pyr_info.sigma0 * pow(2.0, scale_exp);

                float si = pow(builder->pyr_info.scale_factor, (int)scale_exp);

                int wi = width0 / si;
                int hi = height0 / si;

                nx_image_pyr_create_level(pyr, i, wi, hi, si, sigma_i);
        }

        NX_ASSERT_PTR(pyr->n_levels == builder->pyr_info.n_levels);
        NX_ASSERT_PTR(pyr->levels);
        NX_ASSERT_PTR(pyr->levels[0].img);
        NX_ASSERT(pyr->levels[0].img->width == width0);
        NX_ASSERT(pyr->levels[0].img->height == height0);
}


void _nx_image_pyr_builder_update_fast(struct NXImagePyrBuilder *builder, struct NXImagePyr *pyr)
{
        float sigma_g = compute_sigma_g(NX_IMAGE_PYR_BUILDER_INITIAL_SIGMA, pyr->levels[0].sigma);
        if (sigma_g > 0.0f)
                nx_image_smooth(pyr->levels[0].img, pyr->levels[0].img, sigma_g, sigma_g, NULL);

        // Downsample each layer with AA filter to yield the next one
        int n_levels = pyr->n_levels;
        for (int i = 1; i < n_levels; ++i) {
                nx_image_downsample_aa_x(builder->work_img, pyr->levels[i-1].img);
                nx_image_downsample_aa_y(pyr->levels[i].img, builder->work_img);
        }
}

void _nx_image_pyr_builder_update_fine(struct NXImagePyrBuilder *builder, struct NXImagePyr *pyr)
{
        int n_oct = builder->pyr_info.n_octaves;
        int n_steps = builder->pyr_info.n_octave_steps;

        float sigma_current = NX_IMAGE_PYR_BUILDER_INITIAL_SIGMA;
        float sigma_multiplier = 1.0f;
        for (int i = 0; i < n_oct; ++i) {
                struct NXImagePyrLevel *oct_levels = pyr->levels + i * n_steps;

                // On first octave smooth the original image to sigma0.
                if (i == 0) {
                        float sigma_g = compute_sigma_g(sigma_current, oct_levels[0].sigma);
                        if (sigma_g > 0.0f) {
                                nx_image_smooth(oct_levels[0].img, oct_levels[0].img, sigma_g, sigma_g, NULL);
                                sigma_current = oct_levels[0].sigma;
                        }
                }

                // Within the octave each image (other than the first) is the
                // smoothed version of the previous.
                for (int j = 1; j < n_steps; ++j) {
                        float sigma_desired = oct_levels[j].sigma * sigma_multiplier;
                        float sigma_g = compute_sigma_g(sigma_current, sigma_desired);
                        nx_image_smooth(oct_levels[j].img, oct_levels[j-1].img, sigma_g, sigma_g, NULL);
                        sigma_current = sigma_desired;
                }

                // If we are not at the last octave, smooth and downsample the
                // last image to prepare the first image of the next octave.
                if (i != n_oct-1) {
                        struct NXImagePyrLevel *next_oct_level0 = oct_levels + n_steps;
                        float sigma_desired = next_oct_level0->sigma * sigma_multiplier;
                        float sigma_g = compute_sigma_g(sigma_current, sigma_desired);
                        nx_image_smooth(builder->work_img, oct_levels[n_steps-1].img, sigma_g, sigma_g, NULL);
                        nx_image_downsample(next_oct_level0->img, builder->work_img);
                        sigma_current = sigma_desired;

                        sigma_current *= 0.5;
                        sigma_multiplier *= 0.5f;
                }
        }
}

void _nx_image_pyr_builder_update_scaled(struct NXImagePyrBuilder *builder, struct NXImagePyr *pyr)
{
        float sigma_current = NX_IMAGE_PYR_BUILDER_INITIAL_SIGMA;
        float sigma_g = compute_sigma_g(sigma_current, pyr->levels[0].sigma);

        if (sigma_g > 0.0f) {
                nx_image_smooth(pyr->levels[0].img, pyr->levels[0].img, sigma_g, sigma_g, NULL);
                sigma_current = pyr->levels[0].sigma;
        }

        // Each other level is a smoothed and scaled version of the previous level
        nx_image_copy(builder->work_img, pyr->levels[0].img);
        int n_levels = pyr->n_levels;
        for (int i = 1; i < n_levels; ++i) {
                float sigma_desired = pyr->levels[i].sigma;
                float sigma_g = compute_sigma_g(sigma_current, sigma_desired);
                nx_image_smooth(builder->work_img, builder->work_img, sigma_g, sigma_g, NULL);
                sigma_current = sigma_desired;

                nx_image_scale(pyr->levels[i].img, builder->work_img, 1.0f / pyr->levels[i].scale);
        }
}

float compute_sigma_g(float sigma_current, float sigma_desired)
{
        float sigma_g = sqrt(sigma_desired*sigma_desired - sigma_current*sigma_current);

        return sigma_g;
}
