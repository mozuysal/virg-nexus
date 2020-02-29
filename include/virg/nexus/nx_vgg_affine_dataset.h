/**
 * @file nx_vgg_affine_dataset.h
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
 * VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_VGG_AFFINE_DATASET_H
#define VIRG_NEXUS_NX_VGG_AFFINE_DATASET_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_image_io.h"

__NX_BEGIN_DECL

#define NX_VGG_AFFINE_N_SEQ 8

extern const char *NX_VGG_AFFINE_SEQ_NAMES[NX_VGG_AFFINE_N_SEQ];

struct NXVGGAffineSequence {
        char *base_dir;
        char *name;
        int length;
        double **h;
};

struct NXVGGAffineSequence *nx_vgg_affine_sequence_new(const char *base_dir,
                                                       const char *seq_name);

void nx_vgg_affine_sequence_free(struct NXVGGAffineSequence *seq);

void nx_vgg_affine_sequence_xload_frame(const struct NXVGGAffineSequence *seq,
                                        int frame_id,
                                        struct NXImage *img,
                                        enum NXImageLoadMode mode);

__NX_END_DECL

#endif
