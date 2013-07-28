/**
 * @file nx_fast_detector.c
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
#include "virg/nexus/nx_fast_detector.h"

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_math.h"
#include "virg/nexus/nx_mem_block.h"

#define NX_FAST_DETECTOR_WORK_MULTIPLIER 5

extern void fast9_detect(struct NXKeypoint *ret_corners, const unsigned char *im, int xsize, int ysize, int stride, int b, int *ret_num_corners);
extern void fast9_score (const unsigned char *i, int stride, struct NXKeypoint *corners, int num_corners, int b);
extern void fast_nonmax_suppression(int *ret_num_nonmax, struct NXKeypoint *ret_nonmax, int num_corners, const struct NXKeypoint *corners);

int nx_fast_detect_keypoints(int n_keys_max, struct NXKeypoint *keys,
                             const struct NXImage *img, int threshold)
{
        NX_ASSERT(n_keys_max >= 0);
        NX_ASSERT_PTR(keys);
        NX_ASSERT_PTR(img);

        if (n_keys_max == 0) {
                nx_warning(NX_LOG_TAG, "Call to nx_fast_detect_keypoints with 0 as the max. number of keypoints!");
                return 0;
        }

        fast9_detect(keys, img->data, img->width, img->height,
                     img->row_stride, threshold, &n_keys_max);

        for (int i = 0; i < n_keys_max; ++i) {
                keys[i].xs = keys[i].x;
                keys[i].ys = keys[i].y;
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

        fast9_score(img->data, img->row_stride, keys, n_keys, threshold);
}

int nx_fast_detect_keypoints_pyr(int n_keys_supp_max, struct NXKeypoint *keys_supp,
                                 int n_keys_max, struct NXKeypoint *keys,
                                 const struct NXImagePyr *pyr, int threshold)
{
        NX_ASSERT(n_keys_supp_max >= 0);
        NX_ASSERT(n_keys_max >= 0);
        NX_ASSERT_PTR(keys);
        NX_ASSERT_PTR(keys_supp);
        NX_ASSERT_PTR(pyr);

        if (n_keys_supp_max == 0 || n_keys_max == 0) {
                nx_warning(NX_LOG_TAG, "Call to nx_fast_detect_keypoints_pyr with 0 as the maximum number of keypoints!");
                return 0;
        }

        int n_keys_supp = 0;
        int n_level_keys_max = n_keys_supp_max;
        struct NXKeypoint *level_keys = keys_supp;
        for (int i = pyr->n_levels-1; i >= 0 ; --i) {
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

struct NXFastDetector *nx_fast_detector_alloc()
{
        struct NXFastDetector *detector = nx_new(1, struct NXFastDetector);

        detector->max_n_keys = 0;
        detector->threshold = 255;

        detector->n_keys = 0;
        detector->keys = NULL;

        detector->n_work = 0;
        detector->keys_work = NULL;
        detector->mem = nx_mem_block_alloc();

        return detector;
}

struct NXFastDetector *nx_fast_detector_new(int max_n_keys, int initial_work_size)
{
        NX_ASSERT(max_n_keys > 0);

        struct NXFastDetector *detector = nx_fast_detector_alloc();
        nx_fast_detector_resize(detector, max_n_keys, initial_work_size);
        return detector;
}

void nx_fast_detector_free(struct NXFastDetector *detector)
{
        if (detector) {
                nx_free(detector->keys);
                nx_mem_block_free(detector->mem);
                nx_free(detector);
        }
}

void nx_fast_detector_resize(struct NXFastDetector *detector, int max_n_keys, int initial_work_size)
{
        NX_ASSERT_PTR(detector);
        NX_ASSERT(max_n_keys > 0);

        detector->max_n_keys = max_n_keys;
        detector->threshold = 15;

        detector->n_keys = 0;
        detector->keys = nx_new(max_n_keys, struct NXKeypoint);

        if (initial_work_size <= 0)
                initial_work_size = max_n_keys * NX_FAST_DETECTOR_WORK_MULTIPLIER;

        detector->n_work = initial_work_size;
        nx_mem_block_resize(detector->mem, detector->n_work * sizeof(struct NXKeypoint));
        detector->keys_work = (struct NXKeypoint *)detector->mem->ptr;
}

void nx_fast_detector_detect(struct NXFastDetector *detector, const struct NXImage *img)
{
        NX_ASSERT_PTR(detector);
        NX_ASSERT_PTR(img);

        int n_keys = nx_fast_detect_keypoints(detector->n_work,
                                              detector->keys_work,
                                              img, detector->threshold);

        nx_fast_score_keypoints(n_keys, detector->keys_work,
                                img, detector->threshold);


        detector->n_keys = nx_fast_suppress_keypoints(detector->max_n_keys,
                                                      detector->keys,
                                                      n_keys, detector->keys_work);
}

void nx_fast_detector_detect_pyr(struct NXFastDetector *detector, const struct NXImagePyr *pyr)
{
        NX_ASSERT_PTR(detector);
        NX_ASSERT_PTR(pyr);

        detector->n_keys = nx_fast_detect_keypoints_pyr(detector->max_n_keys,
                                                        detector->keys,
                                                        detector->n_work,
                                                        detector->keys_work,
                                                        pyr,
                                                        detector->threshold);
}

void nx_fast_detector_adapt_threshold(struct NXFastDetector *detector)
{
        NX_ASSERT_PTR(detector);

        int lim_high      = (1014 * detector->max_n_keys) >> 10; // n > ~0.99*max
        int lim_low       = ( 921 * detector->max_n_keys) >> 10; // n > ~0.90*max
        int lim_very_low  = ( 716 * detector->max_n_keys) >> 10; // n > ~0.70*max

        if( detector->n_keys >= lim_high ) {
            detector->threshold = nx_min_i(250, detector->threshold + 3);
        } else if( detector->n_keys <= lim_very_low ) {
            detector->threshold = nx_max_i(5, detector->threshold - 3);
        } else if( detector->n_keys <= lim_low ) {
            detector->threshold = nx_max_i(5, detector->threshold - 1);
        }

        NX_ASSERT(detector->threshold >= 5);
        NX_ASSERT(detector->threshold <= 250);
}
