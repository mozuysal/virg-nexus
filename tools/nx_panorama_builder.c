/**
 * @file nx_panorama_builder.c
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
#include "nx_panorama_builder.h"

#include <string.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_string_array.h"
#include "virg/nexus/nx_image_io.h"
#include "virg/nexus/nx_vec234.h"
#include "virg/nexus/nx_vec.h"
#include "virg/nexus/nx_point_match_2d.h"
#include "virg/nexus/nx_sift_detector.h"
#include "virg/nexus/nx_homography.h"

#define SIFT_DISTANCE_RATIO_THR 0.6f
#define INIT_MIN_N_INLIERS 20
#define INIT_HOMOGRAPHY_INLIER_THRESHOLD 3.0f
#define INIT_HOMOGRAPHY_MAX_N_RANSAC_ITERS 500
#define CACHE_DIR NULL

void nx_panorama_builder_init(struct NXPanoramaBuilder *builder);
void nx_panorama_builder_refine_geometric(struct NXPanoramaBuilder *builder);
void nx_panorama_builder_refine_photometric(struct NXPanoramaBuilder *builder);
void nx_panorama_builder_composite(struct NXPanoramaBuilder *builder);

void nx_panorama_builder_add_options(struct NXOptions *opt)
{
        nx_options_add(opt,
                       "R",
                       "IMAGES");
}

struct NXPanoramaBuilder
{
        // per image data
        int n_images; // N
        struct NXStringArray *image_names;
        struct NXImage **images;
        int *n_keys;
        int *max_n_keys;
        struct NXKeypoint **keys;
        uchar **desc;

        // per image-pair data
        int *n_matches; // NxN matrix of int
        struct NXPointMatch2D**matches; // NxN matrix of struct NXPointMatch2D *
        double **H; // NxN matrix of double *
        float *affinity; // NxN matrix of float

        // output data
        struct NXImage *panorama;
};

struct NXPanoramaBuilder *
nx_panorama_builder_new_from_options(struct NXOptions *opt)
{
        const int N_INITIAL_KEY_BUFFER_SIZE = 1000;
        struct NXPanoramaBuilder *builder = NX_NEW(1, struct NXPanoramaBuilder);

        builder->image_names = nx_string_array_new_from_null_terminated_list(nx_options_get_rest(opt));
        builder->n_images = nx_string_array_size(builder->image_names);
        const int N = builder->n_images;
        if (N < 2) {
                NX_FATAL(NX_LOG_TAG, "Panorama Builder requires at least two images, %d given!",
                         N);
        }

        // Allocate per image data
        builder->images = NX_NEW(N, struct NXImage *);
        builder->n_keys = NX_NEW_I(N);
        builder->max_n_keys = NX_NEW_I(N);
        builder->keys = NX_NEW(N, struct NXKeypoint *);
        builder->desc = NX_NEW(N, uchar *);
        for (int i = 0; i < N; ++i) {
                builder->images[i] = nx_image_alloc();
                nx_image_xload(builder->images[i],
                               nx_string_array_get(builder->image_names, i),
                               NX_IMAGE_LOAD_GRAYSCALE);
                builder->n_keys[i] = 0;
                builder->max_n_keys[i] = N_INITIAL_KEY_BUFFER_SIZE;
                builder->keys[i] = NX_NEW(N_INITIAL_KEY_BUFFER_SIZE, struct NXKeypoint);
                builder->desc[i] = NX_NEW_UC(N_INITIAL_KEY_BUFFER_SIZE * NX_SIFT_DESC_DIM);
        }

        // Allocate per image-pair data
        builder->n_matches = NX_NEW_I(N*N);
        memset(builder->n_matches, 0, N*N*sizeof(builder->n_matches[0]));

        builder->matches = NX_NEW(N*N, struct NXPointMatch2D *);
        memset(builder->matches, 0, N*N*sizeof(builder->matches[0]));

        builder->H = NX_NEW(N*N, double *);
        memset(builder->H, 0, N*N*sizeof(builder->H[0]));

        builder->affinity = NX_NEW_S(N*N);
        memset(builder->affinity, 0, N*N*sizeof(builder->affinity[0]));
        // Allocate output data
        builder->panorama = nx_image_alloc();

        return builder;
}

void nx_panorama_builder_free(struct NXPanoramaBuilder *builder)
{
        if (builder) {
                const int N = builder->n_images;

                // Deallocate per image data
                nx_string_array_free(builder->image_names);
                for (int i = 0; i < N; ++i) {
                        nx_image_free(builder->images[i]);
                        nx_free(builder->keys[i]);
                        nx_free(builder->desc[i]);
                }
                nx_free(builder->images);
                nx_free(builder->n_keys);
                nx_free(builder->max_n_keys);
                nx_free(builder->keys);
                nx_free(builder->desc);

                // Deallocate per image-pair data
                nx_free(builder->n_matches);

                for (int i = 0; i < N*N; ++i) {
                        nx_free(builder->matches[i]);
                        nx_free(builder->H[i]);
                }
                nx_free(builder->matches);
                nx_free(builder->H);
                nx_free(builder->affinity);

                // Deallocate output data
                nx_image_free(builder->panorama);
                nx_free(builder);
        }
}

void nx_panorama_builder_run(struct NXPanoramaBuilder *builder)
{
        NX_ASSERT_PTR(builder);

        nx_panorama_builder_init(builder);
        nx_panorama_builder_refine_geometric(builder);
        nx_panorama_builder_refine_photometric(builder);
        nx_panorama_builder_composite(builder);
}

struct NXImage *
nx_panorama_builder_get_panorama(struct NXPanoramaBuilder *builder)
{
        NX_ASSERT_PTR(builder);

        return builder->panorama;
}

int nx_panorama_builder_fit_initial_homography(double **hp, int n_corr,
                                               struct NXPointMatch2D *corr)
{
        double h[9];
        int n_inliers = 0;
        if (n_corr > 4) {
                n_inliers = nx_homography_estimate_norm_ransac(&h[0],
                                                               n_corr, corr,
                                                               INIT_HOMOGRAPHY_INLIER_THRESHOLD,
                                                               INIT_HOMOGRAPHY_MAX_N_RANSAC_ITERS);
        }

        if (n_inliers >= INIT_MIN_N_INLIERS) {
                if (!*hp)
                        *hp = NX_NEW_D(9);
                nx_dvec3_copy(*hp, &h[0]);
        } else {
                if (*hp) {
                        nx_free(*hp);
                        *hp = NULL;
                }
        }

        return n_inliers;
}


void nx_panorama_builder_init(struct NXPanoramaBuilder *builder)
{
        NX_ASSERT_PTR(builder);
        NX_ASSERT(builder->n_images > 1);
        NX_ASSERT_PTR(builder->images);
        NX_ASSERT_PTR(builder->keys);
        NX_ASSERT_PTR(builder->desc);
        for (int i = 0; i < builder->n_images; ++i) {
                NX_ASSERT_PTR(builder->images[i]);
                NX_ASSERT_PTR(builder->keys[i]);
                NX_ASSERT_PTR(builder->desc[i]);
                NX_ASSERT(builder->max_n_keys[i] > 0);
        }

        NX_ASSERT_PTR(builder->n_matches);
        NX_ASSERT_PTR(builder->matches);

        const int N = builder->n_images;

        // Detect keys & extract descriptors
        struct NXSIFTDetectorParams sift_param = nx_sift_default_parameters();
        struct NXSIFTDetector *detector = nx_sift_detector_new(sift_param);

        for (int i = 0; i < N; ++i) {
                builder->n_keys[i] = nx_sift_detector_compute_with_cache(detector,
                                                                         builder->images[i],
                                                                         builder->max_n_keys + i,
                                                                         builder->keys + i,
                                                                         builder->desc + i,
                                                                         CACHE_DIR);
                NX_LOG(NX_LOG_TAG, "Detected %d keypoints on panorama input %s",
                       builder->n_keys[i],
                       nx_string_array_get(builder->image_names, i));
        }
        nx_sift_detector_free(detector);

        // Create initial similarity network and matches
        for (int i = 0; i < N; ++i) {
                int n_keys_i = builder->n_keys[i];
                const struct NXKeypoint *keys_i = builder->keys[i];
                const uchar *desc_i = builder->desc[i];
                for (int j = 0; j < N; ++j) {
                        // skip matching to self
                        if (i == j)
                                continue;

                        int n_keys_j = builder->n_keys[j];
                        const struct NXKeypoint *keys_j = builder->keys[j];
                        const uchar *desc_j = builder->desc[j];
                        int *n_pm = builder->n_matches + (j*N + i);

                        // create match array from image i to j
                        struct NXPointMatch2D **pm = builder->matches + (j*N + i);
                        *pm = (struct NXPointMatch2D *)nx_xrealloc(*pm,
                                                                   n_keys_i * sizeof(**pm));
                        *n_pm = nx_sift_match_brute_force_with_cache(n_keys_i, keys_i, desc_i,
                                                                     n_keys_j, keys_j, desc_j,
                                                                     *pm,
                                                                     SIFT_DISTANCE_RATIO_THR,
                                                                     CACHE_DIR);
                        NX_LOG(NX_LOG_TAG, "%2d <-> %2d : %d initial matches", i, j, *n_pm);
                }
        }

        // Estimate initial homographies and inlier matches
        for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                        // skip estimation to self
                        if (i == j)
                                continue;

                        double **hp = builder->H + j*N + i;
                        int n_corr = builder->n_matches[j*N + i];
                        struct NXPointMatch2D *corr = builder->matches[j*N + i];
                        int ni = nx_panorama_builder_fit_initial_homography(hp,
                                                                            n_corr,
                                                                            corr);
                        builder->affinity[j*N + i] = ni;
                        NX_LOG(NX_LOG_TAG, "%2d <-> %2d : %d inliers --> %s",
                               i, j, ni, *hp ? "OK" : "FAILED");
                }
        }

        // Pick central node
        float *scores = NX_NEW_S(N);
        for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                        float aff = builder->affinity[j*N + i];
                        fprintf(stderr, "%6.1f ", aff);
                        scores[i] += aff;
                        scores[j] += aff;
                }
                fprintf(stderr, "\n");
        }

        int central_node = nx_svec_max_idx(N, scores);
        nx_free(scores);
        scores = NULL;
        NX_LOG(NX_LOG_TAG, "Selecting image %d as the central node", central_node);

        
}

void nx_panorama_builder_refine_geometric(struct NXPanoramaBuilder *builder)
{
        NX_ASSERT_PTR(builder);
}

void nx_panorama_builder_refine_photometric(struct NXPanoramaBuilder *builder)
{
        NX_ASSERT_PTR(builder);
}

void nx_panorama_builder_composite(struct NXPanoramaBuilder *builder)
{
        NX_ASSERT_PTR(builder);
}
