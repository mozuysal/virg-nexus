/**
 * @file nx_fast_detector.c
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
#include "virg/nexus/nx_fast_detector.h"

#include <math.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_math.h"
#include "virg/nexus/nx_mem_block.h"

#define NX_FAST_DETECTOR_WORK_MULTIPLIER 10

extern void fast9_detect(struct NXKeypoint *ret_corners, const unsigned char *im, int xsize, int ysize, int stride, int b, int *ret_num_corners);
extern void fast9_score (const unsigned char *i, int stride, struct NXKeypoint *corners, int num_corners, int b);
extern void fast_nonmax_suppression(int *ret_num_nonmax, struct NXKeypoint *ret_nonmax, int num_corners, const struct NXKeypoint *corners);

int nx_fast_detect_keypoints(int n_keys_max, struct NXKeypoint *keys,
                             const struct NXImage *img, int threshold)
{
        NX_ASSERT(n_keys_max >= 0);
        NX_ASSERT_PTR(keys);
        NX_ASSERT_PTR(img);
        NX_IMAGE_ASSERT_GRAYSCALE_UCHAR(img);

        if (n_keys_max == 0) {
                NX_WARNING(NX_LOG_TAG, "Call to nx_fast_detect_keypoints with 0 as the max. number of keypoints!");
                return 0;
        }

        fast9_detect(keys, img->data.uc, img->width, img->height,
                     img->row_stride, threshold, &n_keys_max);

        for (int i = 0; i < n_keys_max; ++i) {
                keys[i].xs = keys[i].x;
                keys[i].ys = keys[i].y;
                keys[i].level = 0;
                keys[i].scale = 1.0f;
                keys[i].sigma = 0.0f;
                keys[i].score = 0.0f;
                keys[i].ori = 0.0f;
                keys[i].id = i;
        }

        return n_keys_max;
}

void nx_fast_score_keypoints(int n_keys, struct NXKeypoint *keys,
                             const struct NXImage *img, int threshold)
{
        NX_ASSERT(n_keys >= 0);
        NX_ASSERT_PTR(keys);
        NX_ASSERT_PTR(img);
        NX_IMAGE_ASSERT_GRAYSCALE_UCHAR(img);

        fast9_score(img->data.uc, img->row_stride, keys, n_keys, threshold);
}

int nx_fast_detect_keypoints_pyr(int n_keys_supp_max, struct NXKeypoint *keys_supp,
                                 int n_keys_max, struct NXKeypoint *keys,
                                 const struct NXImagePyr *pyr, int threshold,
                                 int n_pyr_key_levels)
{
        NX_ASSERT(n_keys_supp_max >= 0);
        NX_ASSERT(n_keys_max >= 0);
        NX_ASSERT_PTR(keys);
        NX_ASSERT_PTR(keys_supp);
        NX_ASSERT_PTR(pyr);

        if (n_keys_supp_max == 0 || n_keys_max == 0) {
                NX_WARNING(NX_LOG_TAG, "Call to nx_fast_detect_keypoints_pyr with 0 as the maximum number of keypoints!");
                return 0;
        }

        if (n_pyr_key_levels <= 0 || n_pyr_key_levels > pyr->n_levels) {
                n_pyr_key_levels = pyr->n_levels;
        }

        int n_keys_supp = 0;
        int n_level_keys_max = n_keys_supp_max;
        struct NXKeypoint *level_keys = keys_supp;
        for (int i = n_pyr_key_levels-1; i >= 0 ; --i) {
                int n_level_keys = nx_fast_detect_keypoints(n_keys_max,
                                                            keys,
                                                            pyr->levels[i].img,
                                                            threshold);

                nx_fast_score_keypoints(n_level_keys, keys,
                                        pyr->levels[i].img, threshold);


                n_level_keys = nx_fast_suppress_keypoints(n_level_keys_max, level_keys,
                                                          n_level_keys, keys);

                // Fill scales/sigmas, fix keypoint ids
                for (int j = 0; j < n_level_keys; ++j) {
                        level_keys[j].level = i;
                        level_keys[j].sigma = pyr->levels[i].sigma;
                        level_keys[j].scale = pyr->levels[i].scale;
                        level_keys[j].id = n_keys_supp + j;
                }

                n_level_keys_max -= n_level_keys;
                level_keys += n_level_keys;
                n_keys_supp += n_level_keys;

                if (n_level_keys_max <= 0)
                        break;
        }

        return n_keys_supp;
}

int nx_fast_suppress_keypoints(int n_keys_supp_max, struct NXKeypoint *keys_supp,
                               int n_keys, const struct NXKeypoint *keys)
{
        NX_ASSERT_PTR(keys);
        NX_ASSERT_PTR(keys_supp);

        fast_nonmax_suppression(&n_keys_supp_max, keys_supp, n_keys, keys);
        for (int i = 0; i < n_keys_supp_max; ++i) {
                keys_supp[i].id = i;
        }

        return n_keys_supp_max;
}

struct NXFastDetectorICData {
        int radius;
        int n_offsets;
        int *offset_table;
};

static struct NXFastDetectorICData *nx_fast_detector_ic_data_new(int radius)
{
        struct NXFastDetectorICData *data = NX_NEW(1, struct NXFastDetectorICData);

        data->radius = radius;
        data->n_offsets = 0;
        data->offset_table = NULL;

        int r2 = radius*radius;
        for (int y = -radius; y < radius; ++y) {
                for (int x = -radius; x < radius; ++x) {
                        int r2xy = x*x+y*y;
                        if (r2xy < r2)
                                ++data->n_offsets;
                }
        }

        int table_length = data->n_offsets * 2;
        data->offset_table = NX_NEW_I(table_length);
        int *tp = data->offset_table;
        for (int y = -radius; y < radius; ++y) {
                for (int x = -radius; x < radius; ++x) {
                        int r2xy = x*x+y*y;
                        if (r2xy < r2) {
                                tp[0] = x;
                                tp[1] = y;
                                tp += 2;
                        }
                }
        }

        return data;
}

static void nx_fast_detector_ic_data_free(struct NXFastDetectorICData *data)
{
        if (data) {
                nx_free(data->offset_table);
                nx_free(data);
        }
}

struct NXFastDetector *nx_fast_detector_alloc()
{
        struct NXFastDetector *detector = NX_NEW(1, struct NXFastDetector);

        detector->threshold = 15;

        detector->keys_work = nx_keypoint_vector_alloc();
        detector->work_multiplier = NX_FAST_DETECTOR_WORK_MULTIPLIER;

        detector->compute_ori = NX_FALSE;
        detector->ic_data = NULL;

        return detector;
}

void nx_fast_detector_free(struct NXFastDetector *detector)
{
        if (detector) {
                nx_keypoint_vector_free(detector->keys_work);
                nx_fast_detector_ic_data_free(detector->ic_data);
                nx_free(detector);
        }
}

static float nx_keypoint_ori_ic(const uchar *center, int n_offsets, const int *offsets, const int *offset_table)
{
        int m01 = 0;
        int m10 = 0;

        for (int i = 0; i < n_offsets; ++i, offset_table += 2) {
                uchar p = center[offsets[i]];
                m10 += offset_table[0] * p; // x * I
                m01 += offset_table[1] * p; // y * I
        }

        return (float)atan2((double)m01, (double)m10);
}

static void nx_fast_detector_compute_keypoint_ori_ic(int n_keys, struct NXKeypoint* keys, const struct NXImage *img,
                                                     const struct NXFastDetectorICData *data)
{
        NX_ASSERT_PTR(keys);
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(data);
        NX_IMAGE_ASSERT_GRAYSCALE_UCHAR(img);

        const int r = data->radius;
        const int xe = img->width - r;
        const int ye = img->height - r;

        int *img_offsets = NX_NEW_I(data->n_offsets);
        for (int i = 0; i < data->n_offsets; ++i) {
                img_offsets[i] = data->offset_table[2*i]
                        + img->row_stride * data->offset_table[2*i+1];
        }

        for (int i = 0; i < n_keys; ++i) {
                struct NXKeypoint *k = keys + i;

                if (k->x >= r && k->x < xe && k->y >= r && k->y < ye) {
                        const uchar *center = img->data.uc + k->y * img->row_stride + k->x;
                        k->ori = nx_keypoint_ori_ic(center, data->n_offsets, img_offsets, data->offset_table);
                }
        }

        nx_free(img_offsets);
}

int nx_fast_detector_detect(struct NXFastDetector *detector, int max_n_keys, struct NXKeypoint* keys, const struct NXImage *img)
{
        NX_ASSERT_PTR(detector);
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(keys);
        NX_IMAGE_ASSERT_GRAYSCALE_UCHAR(img);

        size_t work_size = max_n_keys * detector->work_multiplier;
        nx_keypoint_vector_resize(detector->keys_work, work_size);

        int n_keys = nx_fast_detect_keypoints(detector->keys_work->size,
                                              detector->keys_work->data,
                                              img, detector->threshold);

        nx_fast_score_keypoints(n_keys, detector->keys_work->data,
                                img, detector->threshold);


        n_keys = nx_fast_suppress_keypoints(max_n_keys, keys,
                                            n_keys, detector->keys_work->data);

        if (detector->compute_ori)
                nx_fast_detector_compute_keypoint_ori_ic(n_keys, keys,
                                                         img, detector->ic_data);

        return n_keys;
}

int nx_fast_detector_detect_pyr(struct NXFastDetector *detector, int max_n_keys, struct NXKeypoint* keys, const struct NXImagePyr *pyr, int n_pyr_key_levels)
{
        NX_ASSERT_PTR(detector);
        NX_ASSERT_PTR(pyr);
        NX_ASSERT_PTR(keys);

        size_t work_size = max_n_keys * detector->work_multiplier;
        nx_keypoint_vector_resize(detector->keys_work, work_size);

        if (n_pyr_key_levels <= 0 || n_pyr_key_levels > pyr->n_levels) {
                n_pyr_key_levels = pyr->n_levels;
        }

        int n_keys_supp = 0;
        int n_level_keys_max = max_n_keys;
        struct NXKeypoint *level_keys = keys;
        for (int i = n_pyr_key_levels-1; i >= 0 ; --i) {
                int n_level_keys = nx_fast_detect_keypoints(detector->keys_work->size,
                                                            detector->keys_work->data,
                                                            pyr->levels[i].img,
                                                            detector->threshold);

                nx_fast_score_keypoints(n_level_keys, detector->keys_work->data,
                                        pyr->levels[i].img, detector->threshold);


                n_level_keys = nx_fast_suppress_keypoints(n_level_keys_max, level_keys,
                                                          n_level_keys, detector->keys_work->data);

                if (detector->compute_ori)
                        nx_fast_detector_compute_keypoint_ori_ic(n_level_keys, level_keys,
                                                                 pyr->levels[i].img, detector->ic_data);

                // Fill scales/sigmas, fix keypoint ids
                for (int j = 0; j < n_level_keys; ++j) {
                        level_keys[j].level = i;
                        level_keys[j].sigma = pyr->levels[i].sigma;
                        level_keys[j].scale = pyr->levels[i].scale;
                        level_keys[j].id = n_keys_supp + j;
                }

                n_level_keys_max -= n_level_keys;
                level_keys += n_level_keys;
                n_keys_supp += n_level_keys;

                if (n_level_keys_max <= 0)
                        break;
        }

        return n_keys_supp;
}

void nx_fast_detector_adapt_threshold(struct NXFastDetector *detector, int n_keys, int max_n_keys)
{
        NX_ASSERT_PTR(detector);

        int lim_high      = (1014 * max_n_keys) >> 10; // n > ~0.99*max
        int lim_low       = ( 921 * max_n_keys) >> 10; // n > ~0.90*max
        int lim_very_low  = ( 716 * max_n_keys) >> 10; // n > ~0.70*max

        if( n_keys >= lim_high ) {
            detector->threshold = nx_min_i(250, detector->threshold + 3);
        } else if( n_keys <= lim_very_low ) {
            detector->threshold = nx_max_i(5, detector->threshold - 3);
        } else if( n_keys <= lim_low ) {
            detector->threshold = nx_max_i(5, detector->threshold - 1);
        }

        NX_ASSERT(detector->threshold >= 5);
        NX_ASSERT(detector->threshold <= 250);
}

void nx_fast_detector_set_ori_param(struct NXFastDetector *detector, NXBool compute_ori_p, int patch_radius)
{
        NX_ASSERT_PTR(detector);
        NX_ASSERT(patch_radius > 0);

        detector->compute_ori = compute_ori_p;
        if (compute_ori_p) {
                nx_fast_detector_ic_data_free(detector->ic_data);
                detector->ic_data = nx_fast_detector_ic_data_new(patch_radius);
        }
}
