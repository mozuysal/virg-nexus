/**
 * @file nx_checkerboard_detector.h
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
#ifndef VIRG_NEXUS_NX_CHECKERBOARD_DETECTOR_H
#define VIRG_NEXUS_NX_CHECKERBOARD_DETECTOR_H

#include "virg/nexus/nx_config.h"

#include "virg/nexus/nx_image.h"

__NX_BEGIN_DECL

void nx_checkers_local_radon_images(struct NXImage **lr_images,
                                    struct NXImage *img, int sum_radius);

void nx_checkers_score_image(struct NXImage *score_img,
                             struct NXImage **lr_images);

__NX_END_DECL

#endif
