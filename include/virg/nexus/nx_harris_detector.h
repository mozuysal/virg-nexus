/**
 * @file nx_harris_detector.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2018 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_HARRIS_DETECTOR_H
#define VIRG_NEXUS_NX_HARRIS_DETECTOR_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_keypoint.h"

__NX_BEGIN_DECL


/**
 * Calculates the derivative images I_x^2, I_y^2, and I_xI_y and convolves them
 * with a Gaussian filter with standard deviation sigma_win.
 *
 * @param dimg      NXImage array of length 3, storing I_x^2, I_y^2, and I_xI_y consecutively. Images are created if necessary.
 * @param img       Input image
 * @param sigma_win Standard deviation of the Gaussian window
 */
void nx_harris_deriv_images(struct NXImage **dimg, const struct NXImage *img, float sigma_win);

/**
 * Calculates the Harris cornerness score, s=det-k*trace^2 from the given
 * derivative images.
 *
 * @param simg Output Harris cornerness score image
 * @param dimg NXImage array of length 3, storing I_x^2, I_y^2, and I_xI_y consecutively.
 * @param k    Cornerness score parameter, around 0.04-0.06
 */
void nx_harris_score_image(struct NXImage *simg, const struct NXImage **dimg, float k);

/**
 *
 *
 * @param n_keys_max Size of the keypoint buffer
 * @param keys       Output keypoint buffer
 * @param simg       Harris cornerness score image
 * @param threshold  Score threshold below which no keypoints are extracted
 *
 * @return Number of output corners
 */
int nx_harris_detect_keypoints(int n_keys_max, struct NXKeypoint *keys,
                               const struct NXImage *simg, float threshold);



__NX_END_DECL

#endif
