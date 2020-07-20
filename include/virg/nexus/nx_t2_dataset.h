/**
 * @file nx_t2_dataset.h
 *
 * Copyright (C) 2020 Mustafa Ozuysal. All rights reserved.
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
#ifndef VIRG_NEXUS_NX_T2_DATASET_H
#define VIRG_NEXUS_NX_T2_DATASET_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

struct NXT2Point {
        double p[3];
        uchar color[3];
};

struct NXT2PointCloud {
        int n_points;
        struct NXT2Point *points;
};

struct NXT2SFMData {
        double **P;
        double *Tpc;
};

enum NXT2ImageSetType { NX_T2_Training, NX_T2_Intermediate, NX_T2_Advanced };

struct NXT2ImageSet {
        char *base_dir;
        char *name;
        enum NXT2ImageSetType type;
        int n_images;
        struct NXT2PointCloud *pc;
        struct NXT2SFMData *sfm;
};

#define NX_T2_N_IMAGE_SETS 21

static const char *NX_T2_IMAGE_SET_NAMES[NX_T2_N_IMAGE_SETS] = {
        "Auditorium", "Ballroom", "Barn", "Caterpillar",
        "Church", "Courthouse", "Courtroom", "Family",
        "Francis", "Horse", "Ignatius", "Lighthouse", "M60",
        "Meetingroom", "Museum", "Palace", "Panther",
        "Playground", "Temple", "Train", "Truck"
};

static enum NXT2ImageSetType NX_T2_IMAGE_SET_TYPES[NX_T2_N_IMAGE_SETS] = {
    NX_T2_Advanced,     NX_T2_Advanced,     NX_T2_Training, NX_T2_Training,
    NX_T2_Training,     NX_T2_Training,     NX_T2_Advanced, NX_T2_Intermediate,
    NX_T2_Intermediate, NX_T2_Intermediate, NX_T2_Training, NX_T2_Intermediate,
    NX_T2_Intermediate, NX_T2_Training,     NX_T2_Advanced, NX_T2_Advanced,
    NX_T2_Intermediate, NX_T2_Intermediate, NX_T2_Advanced, NX_T2_Intermediate,
    NX_T2_Training};

static int NX_T2_IMAGE_SET_LENGTHS[NX_T2_N_IMAGE_SETS] = {
        302, 324, 410, 383, 507, 1106, 301,
        152, 302, 151, 263, 309, 313, 371,
        301, 509, 314, 307, 302, 301, 251
};

struct NXT2ImageSet *nx_t2_sequence_new(const char *base_dir,
                                        const char *image_set_name,
                                        NXBool load_point_cloud);

void nx_t2_sequence_free(struct NXT2ImageSet *image_set);

void nx_t2_sequence_print(struct NXT2ImageSet *image_set);

void nx_t2_sequence_xload_frame(const struct NXT2ImageSet *image_set,
                                int frame_id,
                                struct NXImage *img,
                                enum NXImageLoadMode mode);


__NX_END_DECL

#endif
