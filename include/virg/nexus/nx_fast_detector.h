/**
 * @file nx_fast_detector.h
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
#ifndef VIRG_NEXUS_NX_FAST_DETECTOR_H
#define VIRG_NEXUS_NX_FAST_DETECTOR_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_keypoint.h"
#include "virg/nexus/nx_keypoint_vector.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_image_pyr.h"

__NX_BEGIN_DECL

int nx_fast_detect_keypoints(int n_keys_max, struct NXKeypoint *keys,
                             const struct NXImage *img, int threshold);

void nx_fast_score_keypoints(int n_keys, struct NXKeypoint *keys,
                             const struct NXImage *img, int threshold);

int nx_fast_detect_keypoints_pyr(int n_keys_supp_max, struct NXKeypoint *keys_supp,
                                 int n_keys_max, struct NXKeypoint *keys,
                                 const struct NXImagePyr *pyr, int threshold,
                                 int n_pyr_key_levels);

int nx_fast_suppress_keypoints(int n_keys_supp_max, struct NXKeypoint *keys_supp,
                               int n_keys, const struct NXKeypoint *keys);

struct NXFastDetectorICData;

struct NXFastDetector
{
        int threshold;

        struct NXKeypointVector *keys_work;
        size_t work_multiplier;

        NXBool compute_ori;
        struct NXFastDetectorICData *ic_data;
};

struct NXFastDetector *nx_fast_detector_alloc();

void nx_fast_detector_free(struct NXFastDetector *detector);

int nx_fast_detector_detect(struct NXFastDetector *detector, int max_n_keys, struct NXKeypoint* keys, const struct NXImage *img);

int nx_fast_detector_detect_pyr(struct NXFastDetector *detector, int max_n_keys, struct NXKeypoint* keys, const struct NXImagePyr *pyr, int n_pyr_key_levels);

void nx_fast_detector_set_ori_param(struct NXFastDetector *detector, NXBool compute_ori_p, int patch_radius);

void nx_fast_detector_adapt_threshold(struct NXFastDetector *detector, int n_keys, int max_n_keys);

__NX_END_DECL

#endif
