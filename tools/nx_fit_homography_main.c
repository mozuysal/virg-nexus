/**
 * @file nx_fit_homography_main.c
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "virg/nexus/nx_options.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_timing.h"
#include "virg/nexus/nx_statistics.h"
#include "virg/nexus/nx_image_io.h"
#include "virg/nexus/nx_sift_detector.h"
#include "virg/nexus/nx_homography.h"
#include "virg/nexus/nx_point_match_2d_stats.h"

struct NXKeyStore {
        int n_keys;
        int max_n_keys;
        struct NXKeypoint *keys;
        uchar *desc;
};

struct NXImage **load_images(struct NXOptions *opt);
struct NXKeyStore *compute_keypoints_and_descriptors(struct NXOptions *opt,
                                                     struct NXImage **imgs);
struct NXPointMatch2D *match_images(struct NXOptions *opt,
                                    struct NXImage **imgs,
                                    struct NXKeyStore *stores, int *n_matches);
int fit_ransac(double * h, struct NXOptions *opt, struct NXImage **imgs,
               struct NXKeyStore *stores,
               int n_matches, struct NXPointMatch2D *matches);

void ransac_benchmark(struct NXOptions *opt, struct NXImage **imgs,
                      struct NXKeyStore *stores,
                      int n_matches, struct NXPointMatch2D *matches);

int main(int argc, char **argv)
{
        struct NXOptions *opt = nx_options_alloc();
        nx_options_set_usage_header(opt, "Fits a homography to the given pair of images. "
                                    "Specify the input pair of images using -l/-r.\n\n");
        nx_options_add(opt, "ss",
                       "-l|--left",  "first image in the pair to match", NULL,
                       "-r|--right", "second image in the pair to match", NULL);

        nx_options_add(opt, "bi",
                       "--benchmark-ransac", "run RANSAC benchmark instead of fitting a homography once", NX_FALSE,
                       "--benchmark-steps", "number of runs per benchmark", 10000);

        nx_sift_parameters_add_to_options(opt);
        nx_options_add(opt, "d", "--snn-threshold",
                       "second nearest neighbor threshold to filter matches", -1.0);
        nx_options_add(opt, "id",
                       "--ransac-max-n-iterations", "RANSAC terminates after this many iterations", 10000,
                       "--ransac-inlier-threshold", "maximum forward transfer error for inliers", 3.0);
        nx_options_add(opt, "b",
                       "-v|--verbose", "log more information", NX_FALSE);
        nx_options_add_help(opt);
        nx_options_set_from_args(opt, argc, argv);

        NXBool is_verbose = nx_options_get_bool(opt, "-v");
        NXBool run_ransac_benchmark = nx_options_get_bool(opt, "--benchmark-ransac");
        if (is_verbose)
                nx_options_print_values(opt, stderr);

        struct NXImage **imgs = load_images(opt);
        struct NXKeyStore *stores = compute_keypoints_and_descriptors(opt,
                                                                      imgs);
        int n_matches = 0;
        struct NXPointMatch2D *matches = match_images(opt, imgs, stores,
                                                      &n_matches);


        if (run_ransac_benchmark) {
                ransac_benchmark(opt, imgs, stores, n_matches, matches);
        } else {
                double h[9];
                int n_inliers = fit_ransac(&h[0], opt, imgs, stores,
                                           n_matches, matches);
        }

        nx_free(matches);
        nx_free(stores[0].keys);
        nx_free(stores[0].desc);
        nx_free(stores[1].keys);
        nx_free(stores[1].desc);
        nx_free(stores);
        nx_image_free(imgs[0]);
        nx_image_free(imgs[1]);
        nx_free(imgs);
        nx_options_free(opt);

        return EXIT_SUCCESS;
}

void ransac_benchmark(struct NXOptions *opt, struct NXImage **imgs,
                      struct NXKeyStore *stores,
                      int n_matches, struct NXPointMatch2D *matches)
{
        const int N_BENCHMARK_STEPS = nx_options_get_int(opt, "--benchmark-steps");
        NXBool is_verbose = nx_options_get_bool(opt, "-v");
        double inlier_tolerance = nx_options_get_double(opt, "--ransac-inlier-threshold");
        int max_n_ransac_iter = nx_options_get_int(opt, "--ransac-max-n-iterations");

        struct NXPointMatch2DStats *stats = nx_point_match_2d_stats_new();
        nx_point_match_2d_stats_normalize_matches(stats, n_matches, matches);
        double norm_tol = inlier_tolerance / stats->dp;

        int *n_inliers = NX_NEW_I(N_BENCHMARK_STEPS);
        double *runtime = NX_NEW_D(N_BENCHMARK_STEPS);
        double **h = NX_NEW(N_BENCHMARK_STEPS, double *);
        for (int i = 0; i < N_BENCHMARK_STEPS; ++i) {
                h[i] = NX_NEW_D(9);
        }

        struct NXTimer timer;
        for (int i = 0; i < N_BENCHMARK_STEPS; ++i) {
                nx_timer_start(&timer);
                n_inliers[i] = nx_homography_estimate_ransac(h[i], n_matches,
                                                             matches, norm_tol,
                                                             max_n_ransac_iter);
                nx_timer_stop(&timer);
                runtime[i] = nx_timer_measure_in_msec(&timer);
        }

        nx_point_match_2d_stats_denormalize_matches(stats, n_matches, matches);
        for (int i = 0; i < N_BENCHMARK_STEPS; ++i) {
                nx_point_match_2d_stats_denormalize_homography(stats, h[i]);
        }

        struct NXStatistics1D inlier_stats;
        nx_istatistics_1d(&inlier_stats, N_BENCHMARK_STEPS, n_inliers);

        struct NXStatistics1D runtime_stats;
        nx_dstatistics_1d(&runtime_stats, N_BENCHMARK_STEPS, runtime);

        printf("%6d,%6.1f,%6.1f,%6.1f,%6.1f,%6.1f,%6.1f,%6.1f,",
               inlier_stats.n,
               inlier_stats.mean, inlier_stats.stdev,
               inlier_stats.min, inlier_stats.lower_quartile,
               inlier_stats.median,
               inlier_stats.upper_quartile, inlier_stats.max);
        printf("%8.2f,%8.2f,%8.2f,%8.2f,%8.2f,%8.2f,%8.2f\n",
               runtime_stats.mean, runtime_stats.stdev,
               runtime_stats.min, runtime_stats.lower_quartile,
               runtime_stats.median,
               runtime_stats.upper_quartile, runtime_stats.max);

        /*
         * for (int i = 0; i < N_BENCHMARK_STEPS; ++i) {
         *         printf("%4d %5.2f\n", n_inliers[i], runtime[i]);
         * }
         */

        nx_point_match_2d_stats_free(stats);
}

int fit_ransac(double *h, struct NXOptions *opt, struct NXImage **imgs,
               struct NXKeyStore *stores,
               int n_matches, struct NXPointMatch2D *matches)
{
        NXBool is_verbose = nx_options_get_bool(opt, "-v");
        double inlier_tolerance = nx_options_get_double(opt, "--ransac-inlier-threshold");
        int max_n_ransac_iter = nx_options_get_int(opt, "--ransac-max-n-iterations");
        struct NXPointMatch2DStats *stats = nx_point_match_2d_stats_new();
        nx_point_match_2d_stats_normalize_matches(stats, n_matches, matches);
        double norm_tol = inlier_tolerance / stats->dp;
        int n_inliers = 0;
        n_inliers = nx_homography_estimate_ransac(h, n_matches,
                                                  matches, norm_tol,
                                                  max_n_ransac_iter);

        nx_point_match_2d_stats_denormalize_homography(stats, h);
        nx_point_match_2d_stats_denormalize_matches(stats, n_matches, matches);

        if (is_verbose) {
                NX_LOG(NX_LOG_TAG, "# of inliers is %d", n_inliers);
        }

        nx_point_match_2d_stats_free(stats);

        return n_inliers;
}

struct NXPointMatch2D *match_images(struct NXOptions *opt,
                                    struct NXImage **imgs,
                                    struct NXKeyStore *stores,
                                    int *n_matches)
{
        NXBool is_verbose = nx_options_get_bool(opt, "-v");
        float dist_ratio_thr = nx_options_get_double(opt, "--snn-threshold");

        struct NXPointMatch2D *matches = NX_NEW(stores[0].n_keys, struct NXPointMatch2D);
        *n_matches = nx_sift_match_brute_force(stores[0].n_keys,
                                               stores[0].keys,
                                               stores[0].desc,
                                               stores[1].n_keys,
                                               stores[1].keys,
                                               stores[1].desc,
                                               matches,
                                               dist_ratio_thr);

        if (is_verbose) {
                NX_LOG(NX_LOG_TAG, "# of putative matches is %d", *n_matches);
        }

        return matches;
}


struct NXImage **load_images(struct NXOptions *opt)
{
        NX_ASSERT_PTR(opt);

        NXBool is_verbose = nx_options_get_bool(opt, "-v");

        struct NXImage **imgs = NX_NEW(2, struct NXImage *);
        imgs[0] = nx_image_alloc();
        imgs[1] = nx_image_alloc();
        const char *filename_left = nx_options_get_string(opt, "-l");
        const char *filename_right = nx_options_get_string(opt, "-r");
        if (filename_left && filename_right) {
                nx_image_xload(imgs[0], filename_left, NX_IMAGE_LOAD_GRAYSCALE);
                nx_image_xload(imgs[1], filename_right, NX_IMAGE_LOAD_GRAYSCALE);
        } else {
                nx_options_print_usage(opt, stderr);
                NX_FATAL(NX_LOG_TAG, "You must specify the input pair of images!");
        }

        if (is_verbose) {
                NX_LOG(NX_LOG_TAG, "Loaded pair of images:");
                NX_LOG(NX_LOG_TAG, "  (%4d x %4d) from %s", imgs[0]->width, imgs[0]->height, filename_left);
                NX_LOG(NX_LOG_TAG, "  (%4d x %4d) from %s", imgs[1]->width, imgs[1]->height, filename_right);
        }

        return imgs;
}

struct NXKeyStore *
compute_keypoints_and_descriptors(struct NXOptions *opt,
                                  struct NXImage **imgs)
{
        const int INIT_KEY_CAPACITY = 500;

        NXBool is_verbose = nx_options_get_bool(opt, "-v");

        struct NXSIFTDetectorParams sift_param = nx_sift_parameters_from_options(opt);
        struct NXSIFTDetector *detector = nx_sift_detector_new(sift_param);

        struct NXKeyStore *stores = NX_NEW(2, struct NXKeyStore);
        for (int i = 0; i < 2; ++i) {
                stores[i].n_keys = 0;
                stores[i].max_n_keys = INIT_KEY_CAPACITY;
                stores[i].keys = NX_NEW(INIT_KEY_CAPACITY, struct NXKeypoint);
                stores[i].desc = NX_NEW_UC(INIT_KEY_CAPACITY * NX_SIFT_DESC_DIM);

                stores[i].n_keys = nx_sift_detector_compute(detector,
                                                            imgs[i],
                                                            &stores[i].max_n_keys,
                                                            &stores[i].keys,
                                                            &stores[i].desc);

                if (is_verbose)
                        NX_LOG(NX_LOG_TAG,
                               "Detected %d keypoints on the %s image",
                               stores[i].n_keys, i == 0 ? "left" : "right");
        }

        return stores;
}
