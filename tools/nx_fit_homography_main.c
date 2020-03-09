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
#include <zlib.h>

#include "virg/nexus/nx_options.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_timing.h"
#include "virg/nexus/nx_mat.h"
#include "virg/nexus/nx_statistics.h"
#include "virg/nexus/nx_io.h"
#include "virg/nexus/nx_image_io.h"
#include "virg/nexus/nx_sift_detector.h"
#include "virg/nexus/nx_homography.h"
#include "virg/nexus/nx_point_match_2d_stats.h"
#include "virg/nexus/nx_usac_homography.h"

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
                      struct NXKeyStore *stores, int n_matches,
                      struct NXPointMatch2D *matches, int n_vpts,
                      struct NXPointMatch2D *vpts, const double *h_l2r,
                      const double *h_r2l);

struct NXPointMatch2D *load_validation_points(struct NXOptions *opt,
                                              int *n_corr);
double *load_ground_truth_from_options(struct NXOptions *opt,
                                       const char *opt_name);

int main(int argc, char **argv)
{
        struct NXOptions *opt = nx_options_alloc();
        nx_options_set_usage_header(opt, "Fits a homography to the given pair of images. "
                                    "Specify the input pair of images using -l/-r.\n\n");
        nx_options_add(opt, "sssss",
                       "-l|--left",  "first image in the pair to match", NULL,
                       "-r|--right", "second image in the pair to match", NULL,
                       "--validation-points", "name of a file containing K validation points as a 6-by-K matrix, each column containing left and right homogeneous coordinates.", NULL,
                       "--ground-truth-l2r", "name of a file containing ground truth homography from left to right", NULL,
                       "--ground-truth-r2l", "name of a file containing ground truth homography from right to left", NULL);

        nx_options_add(opt, "bi",
                       "--benchmark-ransac", "run RANSAC benchmark instead of fitting a homography once", NX_FALSE,
                       "--benchmark-steps", "number of runs per benchmark", 100);

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


        int n_vpts = 0;
        struct NXPointMatch2D *vpts = load_validation_points(opt, &n_vpts);

        double *h_l2r = load_ground_truth_from_options(opt, "--ground-truth-l2r");
        double *h_r2l = load_ground_truth_from_options(opt, "--ground-truth-r2l");

        if (run_ransac_benchmark) {
                ransac_benchmark(opt, imgs, stores,
                                 n_matches, matches,
                                 n_vpts, vpts,
                                 h_l2r, h_r2l);
        } else {
                double h[9];
                int n_inliers = fit_ransac(&h[0], opt, imgs, stores,
                                           n_matches, matches);

                if (vpts && n_vpts > 0) {
                        double E_fwd = nx_homography_transfer_error_fwd(&h[0],
                                                                        n_vpts,
                                                                        vpts);
                        if (is_verbose) {
                                NX_LOG(NX_LOG_TAG, "Forward Transfer Error: %g", E_fwd);
                        }
                }
        }

        nx_free(h_l2r);
        nx_free(h_r2l);
        nx_free(vpts);
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
                      int n_matches, struct NXPointMatch2D *matches,
                      int n_vpts, struct NXPointMatch2D *vpts,
                      const double *h_l2r, const double *h_r2l)
{
        const int N_BENCHMARK_STEPS = nx_options_get_int(opt, "--benchmark-steps");
        NXBool is_verbose = nx_options_get_bool(opt, "-v");
        double inlier_tolerance = nx_options_get_double(opt, "--ransac-inlier-threshold");
        int max_n_ransac_iter = nx_options_get_int(opt, "--ransac-max-n-iterations");

        NXBool has_ground_truth = h_l2r || h_r2l;
        int n_inliers_gt = 0;
        if (has_ground_truth) {
                if (h_l2r)
                        n_inliers_gt = nx_homography_mark_inliers(h_l2r, n_matches,
                                                                  matches, inlier_tolerance);
                else
                        n_inliers_gt = nx_homography_mark_inliers_inv(h_r2l, n_matches,
                                                                      matches, inlier_tolerance);
        }

        struct NXPointMatch2DStats *stats = nx_point_match_2d_stats_new();
        nx_point_match_2d_stats_normalize_matches(stats, n_matches, matches);
        double norm_tol = inlier_tolerance / stats->dp;

        int *n_inliers = NX_NEW_I(N_BENCHMARK_STEPS);
        double *E_fwd = NX_NEW_D(N_BENCHMARK_STEPS);
        double *runtime = NX_NEW_D(N_BENCHMARK_STEPS);
        double **h = NX_NEW(N_BENCHMARK_STEPS, double *);
        for (int i = 0; i < N_BENCHMARK_STEPS; ++i) {
                h[i] = NX_NEW_D(9);
        }

        struct NXTimer timer;
        for (int i = 0; i < N_BENCHMARK_STEPS; ++i) {
                nx_timer_start(&timer);
                n_inliers[i] = nx_usac_estimate_homography(h[i], n_matches,
                                                           matches, norm_tol,
                                                           max_n_ransac_iter);
                nx_timer_stop(&timer);
                runtime[i] = nx_timer_measure_in_msec(&timer);
        }

        nx_point_match_2d_stats_denormalize_matches(stats, n_matches, matches);
        for (int i = 0; i < N_BENCHMARK_STEPS; ++i) {
                nx_point_match_2d_stats_denormalize_homography(stats, h[i]);
        }

        struct NXStatistics1D e_fwd_stats;
        if (n_vpts > 0 && vpts) {
                for (int i = 0; i < N_BENCHMARK_STEPS; ++i) {
                        E_fwd[i] = nx_homography_transfer_error_fwd(h[i], n_vpts, vpts);
                }
                nx_dstatistics_1d(&e_fwd_stats, N_BENCHMARK_STEPS, E_fwd);
        }

        struct NXStatistics1D inlier_stats;
        nx_istatistics_1d(&inlier_stats, N_BENCHMARK_STEPS, n_inliers);

        struct NXStatistics1D runtime_stats;
        nx_dstatistics_1d(&runtime_stats, N_BENCHMARK_STEPS, runtime);

        if (is_verbose) {
                fprintf(stderr, "n_corr,");
                if (has_ground_truth)
                        fprintf(stderr, " ni_gt, ir_gt,");
                fprintf(stderr, "     n,"
                        "ni_avg,ni_std,ni_min,ni_lwq,ni_med,ni_upq,ni_max,"
                        "  rt_avg,  rt_std,  rt_min,  rt_lwq,  rt_med,  rt_upq,  rt_max");
                if (n_vpts > 0 && vpts)
                        fprintf(stderr, ",Efwd_avg,Efwd_std,Efwd_min,Efwd_lwq,Efwd_med,Efwd_upq,Efwd_max");
                fprintf(stderr, "\n");
        }

        printf("%6d",
               n_matches);

        if (has_ground_truth)
                printf(",%6d,%6.1f", n_inliers_gt, n_inliers_gt * 100.0 / n_matches);

        printf(",%6d,%6.1f,%6.1f,%6.1f,%6.1f,%6.1f,%6.1f,%6.1f,",
               inlier_stats.n,
               inlier_stats.mean, inlier_stats.stdev,
               inlier_stats.min, inlier_stats.lower_quartile,
               inlier_stats.median,
               inlier_stats.upper_quartile, inlier_stats.max);
        printf("%8.2f,%8.2f,%8.2f,%8.2f,%8.2f,%8.2f,%8.2f",
               runtime_stats.mean, runtime_stats.stdev,
               runtime_stats.min, runtime_stats.lower_quartile,
               runtime_stats.median,
               runtime_stats.upper_quartile, runtime_stats.max);

        if (n_vpts > 0 && vpts) {
                printf(",%8.2f,%8.2f,%8.2f,%8.2f,%8.2f,%8.2f,%8.2f",
                       e_fwd_stats.mean, e_fwd_stats.stdev,
                       e_fwd_stats.min, e_fwd_stats.lower_quartile,
                       e_fwd_stats.median,
                       e_fwd_stats.upper_quartile, e_fwd_stats.max);
        }

        printf("\n");

        nx_point_match_2d_stats_free(stats);
        nx_free(n_inliers);
        nx_free(E_fwd);
        nx_free(runtime);
        for (int i = 0; i < N_BENCHMARK_STEPS; ++i) {
                nx_free(h[i]);
        }
        nx_free(h);
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
        n_inliers = nx_usac_estimate_homography(h, n_matches,
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

struct NXPointMatch2D *load_validation_points(struct NXOptions *opt, int *n_corr)
{
        NXBool is_verbose = nx_options_get_bool(opt, "-v");
        const char *validation_filename = nx_options_get_string(opt, "--validation-points");

        *n_corr = 0;
        if (!validation_filename) {
                if (is_verbose) {
                        NX_LOG(NX_LOG_TAG, "Validation file not specified, "
                               "skipping validation transfer error calculation");
                }
                return NULL;
        }

        int m = 0;
        FILE *stream = nx_xfopen(validation_filename, "r");
        double *pts = nx_dmat_xread(stream, &m, n_corr);
        nx_xfclose(stream, validation_filename);

        if (m != 6) {
                NX_FATAL(NX_LOG_TAG, "Validation file \"%s\"must contain six rows not %d",
                         validation_filename, m);
        }

        if (is_verbose) {
                NX_LOG(NX_LOG_TAG, "Loaded %d validation points from %s",
                       *n_corr, validation_filename);
        }

        if (pts) {
                struct NXPointMatch2D *corr = NX_NEW(*n_corr, struct NXPointMatch2D);
                memset(corr, 0, *n_corr * sizeof(struct NXPointMatch2D));
                for (int i = 0; i < *n_corr; ++i) {
                        corr[i].x[0] = pts[i * 6 + 0];
                        corr[i].x[1] = pts[i * 6 + 1];
                        corr[i].xp[0] = pts[i * 6 + 3];
                        corr[i].xp[1] = pts[i * 6 + 4];
                        corr[i].match_cost = 0;
                        corr[i].sigma_x  = 0.1;
                        corr[i].sigma_xp = 0.1;
                        corr[i].is_inlier = NX_TRUE;
                }
                nx_free(pts);

                return corr;
        } else {
                return NULL;
        }
}

double *load_ground_truth_from_options(struct NXOptions *opt,
                                       const char *opt_name)
{
        NXBool is_verbose = nx_options_get_bool(opt, "-v");

        const char *gt_filename = nx_options_get_string(opt, opt_name);
        if (!gt_filename)
                return NULL;

        FILE *stream = nx_xfopen(gt_filename, "r");
        int m, n;
        double *h_gt = nx_dmat_xread(stream, &m, &n);
        if (m != 3 || n != 3)
                NX_FATAL(NX_LOG_TAG, "Ground truth matrix must be 3-by-3, loaded %d-by-%d from %s",
                         m, n, gt_filename);

        if (is_verbose)
                NX_LOG(NX_LOG_TAG, "Loaded ground truth homograhy from %s provided to %s",
                       gt_filename, opt_name);

        return h_gt;
}
