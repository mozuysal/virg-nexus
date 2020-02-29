/**
 * @file nx_sift_benchmark_main.c
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
 * You should have received a copy of the GNU General Public License
 * along with VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_string.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_options.h"
#include "virg/nexus/nx_timing.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_image_io.h"
#include "virg/nexus/nx_homography.h"
#include "virg/nexus/nx_sift_detector.h"
#include "virg/nexus/nx_vgg_affine_dataset.h"

#define N_TOL 3

const int N_KEYS_INIT = 1000;
static double INLIER_TOL[N_TOL] = { 3.0, 2.0, 1.0 };

struct BenchmarkOptions {
        char *vgg_base;
        float dist_ratio_thr;
        NXBool is_verbose;
};

void add_options(struct NXOptions *opt);
struct BenchmarkOptions *parse_options(struct NXOptions *opt);

void run_vgg_benchmark(struct BenchmarkOptions *bopt,
                       struct NXSIFTDetector *detector);

int main(int argc, char **argv)
{
        struct NXOptions *opt = nx_options_alloc();
        add_options(opt);
        nx_options_set_from_args(opt, argc, argv);
        struct BenchmarkOptions *bopt = parse_options(opt);

        if (bopt->is_verbose) {
                nx_log_verbosity(NX_LOG_INFORMATIVE);
                nx_options_print_values(opt, stderr);
        }

        struct NXSIFTDetector *detector = NULL;
        detector = nx_sift_detector_new(nx_sift_default_parameters());

        run_vgg_benchmark(bopt, detector);

        nx_sift_detector_free(detector);
        nx_free(bopt->vgg_base);
        nx_free(bopt);
        nx_options_free(opt);

        return EXIT_SUCCESS;
}

void add_options(struct NXOptions *opt)
{
        nx_options_add(opt, "sdb",
                       "--vgg-base", "base directory for Oxford affine sequences", "/opt/data/vgg/affine",
                       "--dist-ratio-thr", "near neighbor distance ratio threshold", 0.6,
                       "-v|--verbose", "display more information", NX_FALSE);
        nx_options_add_help(opt);
}

struct BenchmarkOptions *parse_options(struct NXOptions *opt)
{
        struct BenchmarkOptions *bopt = NX_NEW(1, struct BenchmarkOptions);

        bopt->vgg_base = nx_strdup(nx_options_get_string(opt, "--vgg-base"));
        bopt->dist_ratio_thr = nx_options_get_double(opt, "--dist-ratio-thr");
        bopt->is_verbose = nx_options_get_bool(opt, "-v");

        return bopt;
}

void evaluate_vgg_pair(struct BenchmarkOptions *bopt,
                       struct NXSIFTDetector *detector,
                       struct NXVGGAffineSequence *vgg_seq,
                       const struct NXImage *img0,
                       const struct NXKeypoint *keys0,
                       const uchar *desc0,
                       int n_keys0,
                       int pair_id)
{
        struct NXImage *imgi = nx_image_alloc();
        nx_vgg_affine_sequence_xload_frame(vgg_seq, pair_id, imgi,
                                           NX_IMAGE_LOAD_GRAYSCALE);
        NX_INFO(NX_LOG_TAG, "Loaded image %d/%d: %dx%d",
                pair_id, vgg_seq->length - 1, imgi->width, imgi->height);

        // Detect keypoints
        struct NXKeypoint *keysi = NX_NEW(N_KEYS_INIT, struct NXKeypoint);
        uchar *desci = NX_NEW_UC(N_KEYS_INIT * NX_SIFT_DESC_DIM);
        int max_n_keysi = N_KEYS_INIT;
        struct NXTimer timer;
        nx_timer_start(&timer);
        int n_keysi = nx_sift_detector_compute(detector, imgi,
                                               &max_n_keysi,
                                               &keysi, &desci);
        nx_timer_stop(&timer);
        double compute_time = nx_timer_measure_in_msec(&timer);
        NX_INFO(NX_LOG_TAG, "Detected %d keypoints on image %d",
                n_keysi, pair_id);

        // Match keypoints
        struct NXPointMatch2D *pm = NX_NEW(n_keys0,
                                           struct NXPointMatch2D);
        nx_timer_start(&timer);
        int n_pm = nx_sift_match_brute_force(n_keys0, keys0, desc0,
                                             n_keysi, keysi, desci,
                                             pm, bopt->dist_ratio_thr);
        nx_timer_stop(&timer);
        double match_time = nx_timer_measure_in_msec(&timer);
        NX_INFO(NX_LOG_TAG, "0 <-> %d : %d SIFT matches", pair_id, n_pm);

        // Count inliers
        int n_inliers[N_TOL];
        for (int i = 0; i < N_TOL; ++i) {
                n_inliers[i] = nx_homography_mark_inliers(vgg_seq->h[pair_id],
                                                          n_pm, pm,
                                                          INLIER_TOL[i]);
        }

        // Report
        // seq, pair_id, n_keys0, n_keysi, compute_time, match_time, inliers0, ...
        printf("%s,%d,%d,%d,%.3f,%.3f,%d,%d,%d\n",
               vgg_seq->name, pair_id, n_keys0, n_keysi,
               compute_time, match_time,
               n_inliers[0], n_inliers[1], n_inliers[2]);

        nx_free(pm);
        nx_free(desci);
        nx_free(keysi);
        nx_image_free(imgi);
 }

void run_vgg_benchmark(struct BenchmarkOptions *bopt,
                       struct NXSIFTDetector *detector)
{
        struct NXVGGAffineSequence *vgg_seq = NULL;
        const char *seq_name = "graf";
        vgg_seq = nx_vgg_affine_sequence_new(bopt->vgg_base, seq_name);
        NX_INFO(NX_LOG_TAG, "Loaded sequence \"%s\" of length %d",
                vgg_seq->name, vgg_seq->length);

        struct NXImage *img0 = nx_image_alloc();
        nx_vgg_affine_sequence_xload_frame(vgg_seq, 0, img0,
                                           NX_IMAGE_LOAD_GRAYSCALE);
        NX_INFO(NX_LOG_TAG, "Loaded image 0: %dx%d", img0->width, img0->height);

        struct NXKeypoint *keys0 = NX_NEW(N_KEYS_INIT, struct NXKeypoint);
        uchar *desc0 = NX_NEW_UC(N_KEYS_INIT * NX_SIFT_DESC_DIM);
        int max_n_keys0 = N_KEYS_INIT;
        int n_keys0 = nx_sift_detector_compute(detector, img0, &max_n_keys0,
                                               &keys0, &desc0);
        NX_INFO(NX_LOG_TAG, "Detected %d keypoints on image 0", n_keys0);

        for (int i = 1; i < vgg_seq->length; ++i) {
                evaluate_vgg_pair(bopt, detector, vgg_seq, img0,
                                  keys0, desc0, n_keys0, i);
        }

        nx_free(desc0);
        nx_free(keys0);
        nx_image_free(img0);
        nx_vgg_affine_sequence_free(vgg_seq);
}
