/**
 * @file nx_checkerboard_detector.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2019 Mustafa Ozuysal. All rights reserved.
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
