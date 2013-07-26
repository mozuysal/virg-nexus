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

extern void fast9_detect(struct NXKeypoint *ret_corners, const unsigned char *im, int xsize, int ysize, int stride, int b, int *ret_num_corners);
extern void fast9_score (const unsigned char *i, int stride, struct NXKeypoint *corners, int num_corners, int b);
extern void fast_nonmax_suppression(int *ret_num_nonmax, struct NXKeypoint *ret_nonmax, int num_corners, const struct NXKeypoint *corners);

int nx_fast_detect_keypoints(int n_keys_max, struct NXKeypoint *keys,
                             const struct NXImage *img, int threshold)
{
        NX_ASSERT_PTR(keys);
        NX_ASSERT_PTR(img);

        fast9_detect(keys, img->data, img->width, img->height,
                     img->row_stride, threshold, &n_keys_max);
        return n_keys_max;
}

void nx_fast_score_keypoints(int n_keys, struct NXKeypoint *keys,
                            const struct NXImage *img, int threshold)
{
        NX_ASSERT_PTR(keys);
        NX_ASSERT_PTR(img);

        fast9_score(img->data, img->row_stride, keys, n_keys, threshold);
}

int nx_fast_detect_keypoints_pyr(int n_keys_supp_max, struct NXKeypoint *keys_supp,
                                 int n_keys_max, struct NXKeypoint *keys,
                                 const struct NXImagePyr *pyr, int threshold)
{
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
                // Fix keypoint ids
                for (int j = 0; j < n_level_keys; ++j) {
                        level_keys[j].id = n_keys_supp + j;
                }

                n_level_keys_max -= n_level_keys;
                level_keys += n_level_keys;
                n_keys_supp += n_level_keys;
        }

        return n_keys_supp;
}

int nx_fast_suppress_keypoints(int n_keys_supp_max, struct NXKeypoint *keys_supp,
                               int n_keys, const struct NXKeypoint *keys)
{
        NX_ASSERT_PTR(keys);
        NX_ASSERT_PTR(keys_supp);

        fast_nonmax_suppression(&n_keys_supp_max, keys_supp, n_keys, keys);
        return n_keys_supp_max;
}
