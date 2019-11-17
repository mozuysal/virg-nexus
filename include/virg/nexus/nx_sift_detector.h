/**
 * @file nx_sift_detector.h
 *
 * Copyright (C) 2019 Mustafa Ozuysal. All rights reserved.
 *
 * This file is part of the VIRG-Nexus Library
 *
 * Note that SIFT is covered by a US Patent: "Method and apparatus for
 * identifying scale invariant features in an image and use of same for locating
 * an object in an image," David G. Lowe, US Patent 6,711,293 (March 23,
 * 2004). Provisional application filed March 8, 1999. Asignee: The University
 * of British Columbia.
 *
 * For further details, contact David Lowe (lowe@cs.ubc.ca) or the
 * University-Industry Liaison Office of the University of British Columbia.
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
 * You should have received a copy of the GNU General Public License
 * along with VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_SIFT_DETECTOR_H
#define VIRG_NEXUS_NX_SIFT_DETECTOR_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_keypoint.h"
#include "virg/nexus/nx_point_match_2d.h"

__NX_BEGIN_DECL

#define NX_SIFT_DESC_DIM 128

struct NXSIFTDetectorParams {
        NXBool double_image;
        int n_scales_per_octave;
        float sigma0;
        float kernel_truncation_factor;
        int border_distance;
        float peak_threshold;
        float edge_threshold;
        int magnification_factor;
};

static inline struct NXSIFTDetectorParams nx_sift_default_parameters()
{
        struct NXSIFTDetectorParams params;
        params.double_image = NX_TRUE;
        params.n_scales_per_octave = 3;
        params.sigma0 = 1.6f;
        params.kernel_truncation_factor = 4;
        params.border_distance = 5;
        params.peak_threshold = 0.08f;
        params.edge_threshold = 10.0f;
        params.magnification_factor = 3;

        return params;
}

struct NXSIFTDetector;

struct NXSIFTDetector *nx_sift_detector_new(struct NXSIFTDetectorParams sift_param);

void nx_sift_detector_free(struct NXSIFTDetector *detector);

int nx_sift_detector_compute(struct NXSIFTDetector *detector,
                             struct NXImage *image,
                             int *max_n_keys,
                             struct NXKeypoint **keys,
                             uchar **desc);

int nx_sift_detector_compute_with_cache(struct NXSIFTDetector *detector,
                                        struct NXImage *image,
                                        int *max_n_keys,
                                        struct NXKeypoint **keys,
                                        uchar **desc,
                                        const char *cache_dir);

/**
   Match SIFT descriptors brute force from desc to descp and return the number
   of matches. Applies distance ratio check if distance ratio threshold > 0 and
   create a point match structure. corr must have at least n elements.
 */
int nx_sift_match_brute_force(int n,  const struct NXKeypoint *keys,
                              const uchar *desc,
                              int np, const struct NXKeypoint *keyps,
                              const uchar *descp,
                              struct NXPointMatch2D *corr,
                              float dist_ratio_thr);

int nx_sift_match_brute_force_with_cache(int n,  const struct NXKeypoint *keys,
                                         const uchar *desc,
                                         int np, const struct NXKeypoint *keyps,
                                         const uchar *descp,
                                         struct NXPointMatch2D *corr,
                                         float dist_ratio_thr,
                                         const char *cache_dir);

__NX_END_DECL

#endif
