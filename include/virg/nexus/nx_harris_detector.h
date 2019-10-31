/**
 * @file nx_harris_detector.h
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
 * You should have received a copy of the GNU General Public License
 * along with VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
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
#include "virg/nexus/nx_image_io.h"
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
void nx_harris_score_image(struct NXImage *simg, struct NXImage **dimg, float k);

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
