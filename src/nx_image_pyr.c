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
#include <virg/nexus/nx_mem_block.h>

static void nx_image_pyr_free_levels(struct NXImagePyr *pyr);
static void nx_image_pyr_ensure_n_levels(struct NXImagePyr *pyr, int n_levels);

struct NXImagePyr *nx_image_pyr_alloc()
{
        struct NXImagePyr *pyr = nx_new(1, struct NXImagePyr);

        pyr->n_levels = 0;
        pyr->levels = NULL;
        pyr->type = NX_IMAGE_PYR_FAST;
        memset(&pyr->info, 0, sizeof(union NXImagePyrInfo));

        pyr->work_mem = nx_mem_block_alloc();

        return pyr;
}

struct NXImagePyr *nx_image_pyr_new_fast(int width0, int height0, int n_levels)
{
        NX_ASSERT(width0 > 0);
        NX_ASSERT(height0 > 0);
        NX_ASSERT(n_levels > 0);

        struct NXImagePyr *pyr = nx_image_pyr_alloc();
        nx_image_pyr_resize_fast(pyr, width0, height0, n_levels);

        return pyr;
}

struct NXImagePyr *nx_image_pyr_new_fine(int width0, int height0, int n_octaves, int n_octave_steps, float sigma0)
{
        NX_ASSERT(width0 > 0);
        NX_ASSERT(height0 > 0);
        NX_ASSERT(n_octaves > 0);
        NX_ASSERT(n_octave_steps > 0);

        struct NXImagePyr *pyr = nx_image_pyr_alloc();
        nx_image_pyr_resize_fine(pyr, width0, height0, n_octaves, n_octave_steps, sigma0);

        return pyr;
}

struct NXImagePyr *nx_image_pyr_new_scaled(int width0, int height0, int n_levels, float scale_factor, float sigma0)
{
        NX_ASSERT(width0 > 0);
        NX_ASSERT(height0 > 0);
        NX_ASSERT(n_levels > 0);

        struct NXImagePyr *pyr = nx_image_pyr_alloc();
        nx_image_pyr_resize_scaled(pyr, width0, height0, n_levels, scale_factor, sigma0);

        return pyr;
}

void nx_image_pyr_free(struct NXImagePyr *pyr)
{
        if (pyr) {
                nx_image_pyr_free_levels(pyr);
                nx_mem_block_free(pyr->work_mem);
                nx_free(pyr);
        }
}

void nx_image_pyr_resize_fast(struct NXImagePyr *pyr, int width0, int height0, int n_levels)
{
        NX_ASSERT_PTR(pyr);
        NX_ASSERT(width0 > 0);
        NX_ASSERT(height0 > 0);
        NX_ASSERT(n_levels > 0);

        nx_image_pyr_ensure_n_levels(pyr, n_levels);

        int wi = width0;
        int hi = height0;
        float sij = 1.0f;
        for (int i = 0; i < n_levels; ++i) {
                struct NXImagePyrLevel *level = pyr->levels + i;

                nx_image_resize(level->img, wi, hi, 0, NX_IMAGE_GRAYSCALE);
                level->sigma = NX_IMAGE_PYR_INIT_SIGMA;
                level->scale = sij;

                wi /= 2;
                hi /= 2;
                sij *= 2.0f;
        }

        pyr->type = NX_IMAGE_PYR_FAST;

        memset(&pyr->info, 0, sizeof(union NXImagePyrInfo));
        pyr->info.fast.width0 = width0;
        pyr->info.fast.height0 = height0;
}

void nx_image_pyr_resize_fine(struct NXImagePyr *pyr, int width0, int height0, int n_octaves, int n_octave_steps, float sigma0)
{
        NX_ASSERT_PTR(pyr);
        NX_ASSERT(width0 > 0);
        NX_ASSERT(height0 > 0);
        NX_ASSERT(n_octaves > 0);
        NX_ASSERT(n_octave_steps > 0);

        int n_levels = n_octaves * n_octave_steps;
        nx_image_pyr_ensure_n_levels(pyr, n_levels);

        int wi = width0;
        int hi = height0;
        float si = 1.0f;
        struct NXImagePyrLevel *level = pyr->levels;
        for (int i = 0; i < n_octaves; ++i) {
                for (int j = 0; j < n_octave_steps; ++j) {
                        nx_image_resize(level->img, wi, hi, 0, NX_IMAGE_GRAYSCALE);
                        level->sigma = sigma0 * pow(2.0, i + (j / (double)n_octave_steps));
                        level->scale = si;

                        level++;
                }

                wi /= 2;
                hi /= 2;
                si *= 2.0f;
        }

        pyr->type = NX_IMAGE_PYR_FINE;

        memset(&pyr->info, 0, sizeof(union NXImagePyrInfo));
        pyr->info.fine.width0 = width0;
        pyr->info.fine.height0 = height0;
        pyr->info.fine.n_octaves = n_octaves;
        pyr->info.fine.n_octave_steps = n_octave_steps;
        pyr->info.fine.sigma0 = sigma0;
}

void nx_image_pyr_resize_scaled(struct NXImagePyr *pyr, int width0, int height0, int n_levels, float scale_factor, float sigma0)
{
        NX_ASSERT_PTR(pyr);
        NX_ASSERT(width0 > 0);
        NX_ASSERT(height0 > 0);
        NX_ASSERT(n_levels > 0);

        nx_image_pyr_ensure_n_levels(pyr, n_levels);

        float si = 1.0f;
        for (int i = 0; i < n_levels; ++i) {
                struct NXImagePyrLevel *level = pyr->levels + i;

                int wi = width0 / si;
                int hi = height0 / si;

                nx_image_resize(level->img, wi, hi, 0, NX_IMAGE_GRAYSCALE);
                level->sigma = NX_IMAGE_PYR_INIT_SIGMA * si;
                level->scale = si;

                si *= scale_factor;
        }

        pyr->type = NX_IMAGE_PYR_SCALED;

        memset(&pyr->info, 0, sizeof(union NXImagePyrInfo));
        pyr->info.scaled.width0 = width0;
        pyr->info.scaled.height0 = height0;
        pyr->info.scaled.scale_factor = scale_factor;
        pyr->info.scaled.sigma0 = sigma0;
}

void nx_image_pyr_release(struct NXImagePyr *pyr)
{
        NX_ASSERT_PTR(pyr);

        nx_image_pyr_free_levels(pyr);
        pyr->type = NX_IMAGE_PYR_FAST;
        memset(&pyr->info, 0, sizeof(union NXImagePyrInfo));
        nx_mem_block_release(pyr->work_mem);
}

void nx_image_pyr_compute(struct NXImagePyr *pyr, const struct NXImage *img)
{
        NX_ASSERT_PTR(pyr);
        NX_ASSERT_PTR(img);
        NX_ASSERT(pyr->n_levels > 0);
        NX_ASSERT(pyr->levels[0].img->width == img->width);
        NX_ASSERT(pyr->levels[0].img->height == img->height);
        NX_IMAGE_ASSERT_GRAYSCALE(img);

        nx_image_copy(pyr->levels[0].img, img);
        nx_image_pyr_update(pyr);
}

void nx_image_pyr_update(struct NXImagePyr *pyr);

struct NXImagePyr *nx_image_pyr_copy0(const struct NXImagePyr *pyr)
{
        NX_ASSERT_PTR(pyr);

        struct NXImagePyr *cpy = nx_image_pyr_alloc();
        nx_image_pyr_copy(cpy, pyr);

        return cpy;
}

void nx_image_pyr_copy(struct NXImagePyr *dest, const struct NXImagePyr *src)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);

        if (src->n_levels == 0) {
                nx_image_pyr_release(dest);

                dest->type = src->type;
                memcpy(&dest->info, &src->info, sizeof(union NXImagePyrInfo));
        } else {
                switch (src->type) {
                case NX_IMAGE_PYR_FAST:
                        nx_image_pyr_resize_fast(dest, src->info.fast.width0,
                                                 src->info.fast.height0,
                                                 src->n_levels);
                        break;
                case NX_IMAGE_PYR_FINE:
                        nx_image_pyr_resize_fine(dest, src->info.fine.width0,
                                                 src->info.fine.height0,
                                                 src->info.fine.n_octaves,
                                                 src->info.fine.n_octave_steps,
                                                 src->info.fine.sigma0);
                        break;
                case NX_IMAGE_PYR_SCALED:
                        nx_image_pyr_resize_scaled(dest, src->info.scaled.width0,
                                                   src->info.scaled.height0,
                                                   src->n_levels,
                                                   src->info.scaled.scale_factor,
                                                   src->info.scaled.sigma0);
                        break;
                default:
                        nx_fatal(NX_LOG_TAG, "Can not copy image pyramid of unknown type %d!",
                                 (int)src->type);
                }

                for (int i = 0; i < dest->n_levels; ++i) {
                        nx_image_copy(dest->levels[i].img, src->levels[i].img);
                        dest->levels[i].scale = src->levels[i].scale;
                        dest->levels[i].sigma = src->levels[i].sigma;
                }
        }
}

void nx_image_pyr_free_levels(struct NXImagePyr *pyr)
{
        for (int i = 0; i < pyr->n_levels; ++i) {
                nx_image_free(pyr->levels[i].img);
        }
        nx_free(pyr->levels);

        pyr->levels = NULL;
        pyr->n_levels = 0;
}

void nx_image_pyr_ensure_n_levels(struct NXImagePyr *pyr, int n_levels)
{
        NX_ASSERT(n_levels > 0);

        if (pyr->n_levels != n_levels) {
                nx_image_pyr_free_levels(pyr);
        }

        if (pyr->levels == NULL) {
                pyr->levels = nx_new(n_levels, struct NXImagePyrLevel);
                for (int i = 0; i < n_levels; ++i) {
                        pyr->levels[i].img = nx_image_alloc();
                }
                pyr->n_levels = n_levels;
        }
}