/**
 * @file nx_kitti_slam.c
 *
 * Copyright (C) 2019,2020 Mustafa Ozuysal. All rights reserved.
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
#include "virg/nexus/nx_kitti_slam.h"

#include <string.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_string.h"
#include "virg/nexus/nx_io.h"
#include "virg/nexus/nx_filesystem.h"
#include "virg/nexus/nx_image_io.h"

#define NX_KITTI_SLAM_SEQUENCE_DIR_FORMAT "%s/sequences/%02d"
#define NX_KITTI_SLAM_POSE_FILE_FORMAT "%s/poses/%02d.txt"
#define NX_KITTI_SLAM_SEQ_FILE_FORMAT "%s/sequences/%02d/%s"
#define NX_KITTI_SLAM_CAM_DIR_FORMAT "%s/sequences/%02d/image_%d"
#define NX_KITTI_SLAM_LIDAR_DIR_FORMAT "%s/sequences/%02d/velodyne"
#define NX_KITTI_SLAM_FRAME_FORMAT "%s/sequences/%02d/image_%d/%06d.png"

struct NXKittiSLAM *nx_kitti_slam_new(const char *base_dir)
{
        NX_ASSERT_PTR(base_dir);

        if (!nx_check_dir(base_dir))
                NX_FATAL(NX_LOG_TAG, "Kitti SLAM base directory %s does not exist", base_dir);

        struct NXKittiSLAM *dataset = NX_NEW(1, struct NXKittiSLAM);
        dataset->base_dir = nx_strdup(base_dir);

        for (int i = 0; i < NX_KITTI_SLAM_MAX_SEQ_ID; ++i) {
                char *sequence_dir = nx_fstr(NX_KITTI_SLAM_SEQUENCE_DIR_FORMAT,
                                             dataset->base_dir, i);
                dataset->is_seq_available[i] = nx_check_dir(sequence_dir);
                nx_free(sequence_dir);
        }

        return dataset;
}

void nx_kitti_slam_free(struct NXKittiSLAM *dataset)
{
        if (dataset) {
                nx_free(dataset->base_dir);
                nx_free(dataset);
        }
}

static double **nx_read_poses_from_pose_file(FILE *fp, int *length)
{
        const int START_CAP = 300;
        size_t cap = START_CAP;
        double **poses = NX_NEW(cap, double *);
        size_t n = 0;

        double P[12];
        while (12 == fscanf(fp, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                            &P[0], &P[3], &P[6], &P[9],
                            &P[1], &P[4], &P[7], &P[10],
                            &P[2], &P[5], &P[8], &P[11])) {
                if (n >= cap) {
                        size_t new_cap = cap + 1000;
                        poses = nx_xrealloc(poses, new_cap*sizeof(*poses));
                        cap = new_cap;
                }

                poses[n] = NX_NEW_D(12);
                memcpy(poses[n], &P[0], 12*sizeof(P[0]));
                n++;
        }

        *length = n;
        return poses;
}

static double *nx_read_times_from_times_file(FILE *fp, int *length)
{
        const int START_CAP = 300;
        size_t cap = START_CAP;
        double *times = NX_NEW_D(cap);
        size_t n = 0;

        double t;
        while (1 == fscanf(fp, "%lf", &t)) {
                if (n >= cap) {
                        size_t new_cap = cap + 1000;
                        times = nx_xrealloc(times, new_cap*sizeof(*times));
                        cap = new_cap;
                }

                times[n] = t;
                n++;
        }

        *length = n;
        return times;
}

struct NXKittiSLAMSequence *nx_kitti_slam_sequence_new(struct NXKittiSLAM *dataset,
                                                       int seq_id)
{
        NX_ASSERT_PTR(dataset);
        NX_ASSERT_INDEX(seq_id, NX_KITTI_SLAM_MAX_SEQ_ID);

        if (!dataset->is_seq_available[seq_id]) {
                NX_FATAL(NX_LOG_TAG, "Kitti SLAM sequence %d is missing"
                         " in base directory \"%s\"",
                         seq_id, dataset->base_dir);
        }

        struct NXKittiSLAMSequence *seq = NX_NEW(1, struct NXKittiSLAMSequence);
        seq->dataset = dataset;
        seq->seq_id = seq_id;
        seq->length = 0;

        char *pose_file = nx_fstr(NX_KITTI_SLAM_POSE_FILE_FORMAT,
                                  dataset->base_dir, seq_id);
        FILE *pose_in = nx_fopen(pose_file, "r");
        seq->is_gt_available = (pose_file != NULL);
        if (seq->is_gt_available) {
                seq->poses = nx_read_poses_from_pose_file(pose_in, &seq->length);
                nx_fclose(pose_in, "pose_in");
        } else {
                seq->poses = NULL;
        }
        nx_free(pose_file);

        char *times_file = nx_fstr(NX_KITTI_SLAM_SEQ_FILE_FORMAT,
                                   dataset->base_dir, seq_id, "times.txt");
        FILE *times_in = nx_xfopen(times_file, "r");
        int n = 0;
        seq->times = nx_read_times_from_times_file(times_in, &n);
        if (seq->is_gt_available && n != seq->length) {
                NX_FATAL(NX_LOG_TAG, "Number of lines in poses (%d) and times (%d) files differ",
                         seq->length, n);
        } else {
                seq->length = n;
        }
        nx_xfclose(times_in, "times_in");
        nx_free(times_file);

        char *calib_file = nx_fstr(NX_KITTI_SLAM_SEQ_FILE_FORMAT,
                                   dataset->base_dir, seq_id, "calib.txt");
        FILE *calib_in = nx_xfopen(calib_file, "r");
        if (12 != fscanf(calib_in, "P0: %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
                         &seq->calib[0][0], &seq->calib[0][3], &seq->calib[0][6], &seq->calib[0][9],
                         &seq->calib[0][1], &seq->calib[0][4], &seq->calib[0][7], &seq->calib[0][10],
                         &seq->calib[0][2], &seq->calib[0][5], &seq->calib[0][8], &seq->calib[0][11])) {
                NX_FATAL(NX_LOG_TAG, "Error reading calibration for camera 0 from %s", calib_file);
        }
        if (12 != fscanf(calib_in, "P1: %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
                         &seq->calib[1][0], &seq->calib[1][3], &seq->calib[1][6], &seq->calib[1][9],
                         &seq->calib[1][1], &seq->calib[1][4], &seq->calib[1][7], &seq->calib[1][10],
                         &seq->calib[1][2], &seq->calib[1][5], &seq->calib[1][8], &seq->calib[1][11])) {
                NX_FATAL(NX_LOG_TAG, "Error reading calibration for camera 1 from %s", calib_file);
        }
        if (12 != fscanf(calib_in, "P2: %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
                         &seq->calib[2][0], &seq->calib[2][3], &seq->calib[2][6], &seq->calib[2][9],
                         &seq->calib[2][1], &seq->calib[2][4], &seq->calib[2][7], &seq->calib[2][10],
                         &seq->calib[2][2], &seq->calib[2][5], &seq->calib[2][8], &seq->calib[2][11])) {
                NX_FATAL(NX_LOG_TAG, "Error reading calibration for camera 2 from %s", calib_file);
        }
        if (12 != fscanf(calib_in, "P3: %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
                         &seq->calib[3][0], &seq->calib[3][3], &seq->calib[3][6], &seq->calib[3][9],
                         &seq->calib[3][1], &seq->calib[3][4], &seq->calib[3][7], &seq->calib[3][10],
                         &seq->calib[3][2], &seq->calib[3][5], &seq->calib[3][8], &seq->calib[3][11])) {
                NX_FATAL(NX_LOG_TAG, "Error reading calibration for camera 3 from %s", calib_file);
        }
        nx_xfclose(calib_in, "calib_in");
        nx_free(calib_file);

        for (int i = 0; i < 4; ++i) {
                char *cam_folder = nx_fstr(NX_KITTI_SLAM_CAM_DIR_FORMAT,
                                           dataset->base_dir,
                                           seq_id, i);
                seq->is_cam_available[i] = nx_check_dir(cam_folder);
                nx_free(cam_folder);
        }
        char *lidar_dir = nx_fstr(NX_KITTI_SLAM_LIDAR_DIR_FORMAT,
                                  dataset->base_dir, seq_id);
        seq->is_lidar_available = nx_check_dir(lidar_dir);
        nx_free(lidar_dir);

        return seq;
}

void nx_kitti_slam_sequence_free(struct NXKittiSLAMSequence *seq)
{
        if (seq) {
                if (seq->poses) {
                        for (int i = 0; i < seq->length; ++i)
                                nx_free(seq->poses[i]);
                        nx_free(seq->poses);
                }
                nx_free(seq->times);
                nx_free(seq);
        }
}

void nx_kitti_slam_sequence_xload_frame(const struct NXKittiSLAMSequence *seq,
                                        int cam_id, int frame_id,
                                        struct NXImage *img)
{
        NX_ASSERT_PTR(seq);
        NX_ASSERT_INDEX(cam_id, 4);
        NX_ASSERT_INDEX(frame_id, seq->length);
        NX_ASSERT_PTR(img);

        if (!seq->is_cam_available[cam_id]) {
                NX_FATAL(NX_LOG_TAG, "Kitti SLAM sequence %d is missing"
                         " camera %d in base directory \"%s\"",
                         seq->seq_id, cam_id, seq->dataset->base_dir);
        }

        char *filename = nx_fstr(NX_KITTI_SLAM_FRAME_FORMAT,
                                 seq->dataset->base_dir,
                                 seq->seq_id,
                                 cam_id, frame_id);
        nx_image_xload(img, filename, NX_IMAGE_LOAD_AS_IS);
        nx_free(filename);
}
