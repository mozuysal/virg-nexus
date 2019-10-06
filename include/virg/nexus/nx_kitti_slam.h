/**
 * @file nx_kitti_slam.h
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
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_KITTI_SLAM_H
#define VIRG_NEXUS_NX_KITTI_SLAM_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_image.h"

__NX_BEGIN_DECL

#define NX_KITTI_SLAM_MAX_SEQ_ID 22

struct NXKittiSLAM {
        char *base_dir;
        NXBool is_seq_available[NX_KITTI_SLAM_MAX_SEQ_ID];
};

struct NXKittiSLAMSequence {
        struct NXKittiSLAM *dataset;
        int seq_id;
        int length;
        double **poses;
        double *times;
        double calib[4][12];
        NXBool is_gt_available;
        NXBool is_cam_available[4];
        NXBool is_lidar_available;
};

struct NXKittiSLAM *nx_kitti_slam_new(const char *base_dir);

void nx_kitti_slam_free(struct NXKittiSLAM *dataset);

struct NXKittiSLAMSequence *nx_kitti_slam_sequence_new(struct NXKittiSLAM *dataset,
                                                       int seq_id);

void nx_kitti_slam_sequence_free(struct NXKittiSLAMSequence *seq);

void nx_kitti_slam_sequence_xload_frame(const struct NXKittiSLAMSequence *seq,
                                        int cam_id, int frame_id,
                                        struct NXImage *img);

__NX_END_DECL

#endif
