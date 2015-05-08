/**
 * @file nx_brief_extractor.c
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_brief_extractor.h"

#include <limits.h>
#include <time.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_math.h"
#include "virg/nexus/nx_uniform_sampler.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_image_pyr.h"

#define NX_BRIEF_TEST_PAIR_MIN_DISTANCE2 (3*3)

struct NXBriefExtractor *nx_brief_extractor_alloc()
{
        struct NXBriefExtractor *be = NX_NEW(1, struct NXBriefExtractor);

        be->radius = 0;
        be->n_octets = 0;

        be->offsets = NULL;

        be->offset_limits[0] = 0;
        be->offset_limits[1] = 0;
        be->offset_limits[2] = 0;
        be->offset_limits[3] = 0;

        be->pyr_level_offset = 2;

        return be;
}

struct NXBriefExtractor *nx_brief_extractor_new(int n_octets, int radius)
{
        struct NXBriefExtractor *be = nx_brief_extractor_alloc();
        nx_brief_extractor_resize(be, n_octets, radius);
        return be;
}

struct NXBriefExtractor *nx_brief_extractor_new_with_seed(int n_octets, int radius, uint32_t seed)
{
        struct NXBriefExtractor *be = nx_brief_extractor_new(n_octets, radius);
        nx_brief_extractor_randomize_with_seed(be, seed);
        return be;
}

void nx_brief_extractor_free(struct NXBriefExtractor *be)
{
        if (be) {
                nx_free(be->offsets);
                nx_free(be);
        }
}

void nx_brief_extractor_resize(struct NXBriefExtractor *be, int n_octets, int radius)
{
        NX_ASSERT_PTR(be);
        NX_ASSERT(n_octets > 0);
        NX_ASSERT(radius > 0);

        if (be->n_octets < n_octets) {
                nx_free(be->offsets);
                be->offsets = NULL;
        }

        if (!be->offsets) {
                int length = n_octets * 8 * 4;
                be->offsets = NX_NEW_I(length);
        }

        be->n_octets = n_octets;
        be->radius = radius;

        nx_brief_extractor_randomize(be);
}

void nx_brief_extractor_update_limits(struct NXBriefExtractor *be)
{
        be->offset_limits[0] = INT_MAX; // x_min
        be->offset_limits[1] = -INT_MAX; // x_max
        be->offset_limits[2] = INT_MAX; // y_min
        be->offset_limits[3] = -INT_MAX; // y_max

        int length = be->n_octets * 8;
        for( int i = 0; i < length; ++i ) {

                int x0 = be->offsets[4*i];
                int y0 = be->offsets[4*i+1];
                int x1 = be->offsets[4*i+2];
                int y1 = be->offsets[4*i+3];

                be->offset_limits[0] = nx_min_i(be->offset_limits[0], x0);
                be->offset_limits[1] = nx_max_i(be->offset_limits[1], x0);

                be->offset_limits[0] = nx_min_i(be->offset_limits[0], x1);
                be->offset_limits[1] = nx_max_i(be->offset_limits[1], x1);

                be->offset_limits[2] = nx_min_i(be->offset_limits[2], y0);
                be->offset_limits[3] = nx_max_i(be->offset_limits[3], y0);

                be->offset_limits[2] = nx_min_i(be->offset_limits[2], y1);
                be->offset_limits[3] = nx_max_i(be->offset_limits[3], y1);
        }
}

void nx_brief_extractor_randomize(struct NXBriefExtractor *be)
{
        NX_ASSERT_PTR(be);
        NX_ASSERT_PTR(be->offsets);

        nx_brief_extractor_randomize_with_seed(be, time(NULL));
}

void nx_brief_extractor_randomize_with_seed(struct NXBriefExtractor *be, uint32_t seed)
{
        NX_ASSERT_PTR(be);
        NX_ASSERT_PTR(be->offsets);

        struct NXUniformSampler *sampler = nx_uniform_sampler_new();

        int length = be->n_octets * 8;
        int radius = be->radius;
        for (int i = 0; i < length; ++i) {
                int x0, y0;
                x0 = 2*radius*(nx_uniform_sampler_sample_s(sampler) - 0.5f);
                y0 = 2*radius*(nx_uniform_sampler_sample_s(sampler) - 0.5f);

                int x1, y1, d2;
                do {
                        x1 = 2*radius*(nx_uniform_sampler_sample_s(sampler) - 0.5f);
                        y1 = 2*radius*(nx_uniform_sampler_sample_s(sampler) - 0.5f);

                        int dx = x1 - x0;
                        int dy = y1 - y0;
                        d2 = dx*dx + dy*dy;
                } while (d2 < NX_BRIEF_TEST_PAIR_MIN_DISTANCE2);

                be->offsets[4*i]   = x0;
                be->offsets[4*i+1] = y0;
                be->offsets[4*i+2] = x1;
                be->offsets[4*i+3] = y1;
        }

        nx_brief_extractor_update_limits(be);

        nx_uniform_sampler_free(sampler);
}

NXBool nx_brief_extractor_check_point_pyr(struct NXBriefExtractor *be, const struct NXImagePyr *pyr, int x, int y, int level)
{
        NX_ASSERT_PTR(be);
        NX_ASSERT_PTR(pyr);

        int sample_level = level + be->pyr_level_offset;
        if (sample_level >= pyr->n_levels)
                return NX_FALSE;

        int width  = pyr->levels[sample_level].img->width;
        int height = pyr->levels[sample_level].img->height;

        float key_scale = pyr->levels[level].scale;
        float sample_scale = pyr->levels[sample_level].scale;
        float scale_f = key_scale / sample_scale;

        int x_min = (x + be->offset_limits[0]) * scale_f;
        int x_max = (x + be->offset_limits[1]) * scale_f;
        int y_min = (y + be->offset_limits[2]) * scale_f;
        int y_max = (y + be->offset_limits[3]) * scale_f;

        return (x_min >= 0) && (x_max < width) && (y_min >= 0) && (y_max < height);
}

void nx_brief_extractor_compute_pyr(struct NXBriefExtractor *be, const struct NXImagePyr *pyr, int x, int y, int level, uchar *desc)
{
        NX_ASSERT_PTR(be);
        NX_ASSERT_PTR(pyr);
        NX_ASSERT_PTR(desc);

        int sample_level = level + be->pyr_level_offset;

        const struct NXImage *img = pyr->levels[sample_level].img;

        float key_scale = pyr->levels[level].scale;
        float sample_scale = pyr->levels[sample_level].scale;
        float scale_f = key_scale / sample_scale;

        const int *offsets = be->offsets;
        for (int i = 0; i < be->n_octets; ++i, ++desc) {
                int sample_x0 = (x + offsets[0]) * scale_f;
                int sample_y0 = (y + offsets[1]) * scale_f;
                int sample_x1 = (x + offsets[2]) * scale_f;
                int sample_y1 = (y + offsets[3]) * scale_f;

                uchar I0 = img->data[img->row_stride * sample_y0 + sample_x0];
                uchar I1 = img->data[img->row_stride * sample_y1 + sample_x1];
                //printf("Test %2d: I0 = %4d I1 = %4d\n", i*8, ((int)I0)-15, ((int)I1)-15);

                if (I0 > I1)
                        *desc = 1;
                else
                        *desc = 0;

                for (int j = 1; j < 8; ++j) {
                        offsets += 4;

                        *desc <<= 1;

                        sample_x0 = (x + offsets[0]) * scale_f;
                        sample_y0 = (y + offsets[1]) * scale_f;
                        sample_x1 = (x + offsets[2]) * scale_f;
                        sample_y1 = (y + offsets[3]) * scale_f;

                        I0 = img->data[img->row_stride * sample_y0 + sample_x0];
                        I1 = img->data[img->row_stride * sample_y1 + sample_x1];
                        //printf("Test %2d: I0 = %4d I1 = %4d\n", i*8+j, ((int)I0)-15, ((int)I1)-15);

                        if (I0 > I1)
                                *desc |= 1;
                }

                offsets += 4;
        }
}

// static look-up table for bit counts of all possible values of a byte
static const uchar OCTET_BIT_COUNT_TABLE[256] = {
        0, 1, 1, 2, 1, 2, 2, 3,
        1, 2, 2, 3, 2, 3, 3, 4,
        1, 2, 2, 3, 2, 3, 3, 4,
        2, 3, 3, 4, 3, 4, 4, 5,
        1, 2, 2, 3, 2, 3, 3, 4,
        2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5,
        3, 4, 4, 5, 4, 5, 5, 6,
        1, 2, 2, 3, 2, 3, 3, 4,
        2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5,
        3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5,
        3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6,
        4, 5, 5, 6, 5, 6, 6, 7,
        1, 2, 2, 3, 2, 3, 3, 4,
        2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5,
        3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5,
        3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6,
        4, 5, 5, 6, 5, 6, 6, 7,
        2, 3, 3, 4, 3, 4, 4, 5,
        3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6,
        4, 5, 5, 6, 5, 6, 6, 7,
        3, 4, 4, 5, 4, 5, 5, 6,
        4, 5, 5, 6, 5, 6, 6, 7,
        4, 5, 5, 6, 5, 6, 6, 7,
        5, 6, 6, 7, 6, 7, 7, 8
};

int nx_brief_extractor_descriptor_distance(int n_octets, const uchar *desc0, const uchar *desc1)
{
        int dist = 0;

        int i = 0;
        for ( ; i < n_octets - 7; i += 8) {
                dist += OCTET_BIT_COUNT_TABLE[desc0[0] ^ desc1[0]]
                        + OCTET_BIT_COUNT_TABLE[desc0[1] ^ desc1[1]]
                        + OCTET_BIT_COUNT_TABLE[desc0[2] ^ desc1[2]]
                        + OCTET_BIT_COUNT_TABLE[desc0[3] ^ desc1[3]]
                        + OCTET_BIT_COUNT_TABLE[desc0[4] ^ desc1[4]]
                        + OCTET_BIT_COUNT_TABLE[desc0[5] ^ desc1[5]]
                        + OCTET_BIT_COUNT_TABLE[desc0[6] ^ desc1[6]]
                        + OCTET_BIT_COUNT_TABLE[desc0[7] ^ desc1[7]];
                desc0 += 8;
                desc1 += 8;
        }

        for ( ; i < n_octets; ++i) {
                dist += OCTET_BIT_COUNT_TABLE[*desc0 ^ *desc1];
                ++desc0;
                ++desc1;
        }

        return dist;
}
