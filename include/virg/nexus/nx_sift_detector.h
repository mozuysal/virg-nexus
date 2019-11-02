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

struct NXSIFTDetectorParams nx_sift_default_parameters();

struct NXSIFTDetector;

struct NXSIFTDetector *nx_sift_detector_new(struct NXSIFTDetectorParams sift_param);

void nx_sift_detector_free(struct NXSIFTDetector *detector);

int nx_sift_detector_compute(struct NXSIFTDetector *detector,
                             struct NXImage *image,
                             int *max_n_keys,
                             struct NXKeypoint **keys,
                             uchar **desc);

__NX_END_DECL

#endif
