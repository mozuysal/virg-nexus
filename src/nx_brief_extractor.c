/**
 * @file nx_brief_extractor.c
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
 * You should have received a copy of the GNU General Public License along with
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_brief_extractor.h"

#include <limits.h>
#include <time.h>
#include <math.h>

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

        struct NXUniformSampler *sampler = nx_uniform_sampler_new_with_seed(seed);

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
        NX_IMAGE_ASSERT_GRAYSCALE_UCHAR(img);

        float key_scale = pyr->levels[level].scale;
        float sample_scale = pyr->levels[sample_level].scale;
        float scale_f = key_scale / sample_scale;

        const int *offsets = be->offsets;
        for (int i = 0; i < be->n_octets; ++i, ++desc) {
                *desc = 0;

                for (int j = 0; j < 8; ++j, offsets += 4) {
                        *desc <<= 1;

                        int sample_x0 = (x + offsets[0]) * scale_f;
                        int sample_y0 = (y + offsets[1]) * scale_f;
                        int sample_x1 = (x + offsets[2]) * scale_f;
                        int sample_y1 = (y + offsets[3]) * scale_f;

                        uchar I0 = img->data.uc[img->row_stride * sample_y0 + sample_x0];
                        uchar I1 = img->data.uc[img->row_stride * sample_y1 + sample_x1];

                        if (I0 > I1)
                                *desc |= 1;
                }
        }
}

NXBool nx_brief_extractor_compute_pyr_at_theta(struct NXBriefExtractor *be, const struct NXImagePyr *pyr, int x, int y, int level, float theta, uchar *desc)
{
        NX_ASSERT_PTR(be);
        NX_ASSERT_PTR(pyr);
        NX_ASSERT_PTR(desc);

        int sample_level = level + be->pyr_level_offset;
        const struct NXImage *img = pyr->levels[sample_level].img;
        NX_IMAGE_ASSERT_GRAYSCALE_UCHAR(img);

        float key_scale = pyr->levels[level].scale;
        float sample_scale = pyr->levels[sample_level].scale;
        float scale_f = key_scale / sample_scale;

        float ct = cos(theta);
        float st = sin(theta);

        const int *offsets = be->offsets;
        for (int i = 0; i < be->n_octets; ++i, ++desc) {
                *desc = 0;

                for (int j = 0; j < 8; ++j, offsets += 4) {
                        *desc <<= 1;

                        int sample_x0 = (x + ct*offsets[0] + st*offsets[1]) * scale_f;
                        int sample_y0 = (y - st*offsets[0] + ct*offsets[1]) * scale_f;
                        int sample_x1 = (x + ct*offsets[2] + st*offsets[3]) * scale_f;
                        int sample_y1 = (y - st*offsets[2] + ct*offsets[3]) * scale_f;

                        if (sample_x0 < 0 || sample_x0 >= img->width || sample_x1 < 0 || sample_x1 >= img->width
                            || sample_y0 < 0 || sample_y0 >= img->height || sample_y1 < 0 || sample_y1 >= img->height)
                                return NX_FALSE;

                        uchar I0 = img->data.uc[img->row_stride * sample_y0 + sample_x0];
                        uchar I1 = img->data.uc[img->row_stride * sample_y1 + sample_x1];

                        if (I0 > I1)
                                *desc |= 1;
                }
        }

        return NX_TRUE;
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

NXResult nx_brief_extractor_write(const struct NXBriefExtractor *be, FILE *stream)
{
        NX_ASSERT_PTR(be);
        NX_ASSERT_PTR(stream);

        if (fwrite(&be->radius, sizeof(be->radius), 1, stream) != 1)
                return NX_FAIL;

        if (fwrite(&be->n_octets, sizeof(be->n_octets), 1, stream) != 1)
                return NX_FAIL;

        int n_offsets = be->n_octets*4*8;
        if (fwrite(be->offsets, sizeof(*be->offsets), n_offsets, stream) != (size_t)n_offsets)
                return NX_FAIL;

        if (fwrite(&be->pyr_level_offset, sizeof(be->pyr_level_offset), 1, stream) != 1)
                return NX_FAIL;

        return NX_OK;
}

NXResult nx_brief_extractor_read(struct NXBriefExtractor *be, FILE *stream)
{
        NX_ASSERT_PTR(be);
        NX_ASSERT_PTR(stream);

        int radius;
        if (fread(&radius, sizeof(radius), 1, stream) != 1)
                return NX_FAIL;

        int n_octets;
        if (fread(&n_octets, sizeof(n_octets), 1, stream) != 1)
                return NX_FAIL;

        nx_brief_extractor_resize(be, radius, n_octets);

        int n_offsets = be->n_octets*4*8;
        if (fread(be->offsets, sizeof(*be->offsets), n_offsets, stream) != (size_t)n_offsets)
                return NX_FAIL;
        nx_brief_extractor_update_limits(be);

        if (fread(&be->pyr_level_offset, sizeof(be->pyr_level_offset), 1, stream) != 1)
                return NX_FAIL;

        return NX_OK;
}

void nx_brief_extractor_xwrite(const struct NXBriefExtractor *be, FILE *stream)
{
        if (nx_brief_extractor_write(be, stream) != NX_OK)
                NX_FATAL(NX_LOG_TAG, "Error writing BRIEF extractor to stream!");
}

void nx_brief_extractor_xread(struct NXBriefExtractor *be, FILE *stream)
{
        if (nx_brief_extractor_read(be, stream) != NX_OK)
                NX_FATAL(NX_LOG_TAG, "Error reading BRIEF extractor from stream!");
}
