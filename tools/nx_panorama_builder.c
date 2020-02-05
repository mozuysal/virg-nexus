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

#include <float.h>
#include <string.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_string_array.h"
#include "virg/nexus/nx_image_io.h"
#include "virg/nexus/nx_math.h"
#include "virg/nexus/nx_vec234.h"
#include "virg/nexus/nx_vec.h"
#include "virg/nexus/nx_mat234.h"
#include "virg/nexus/nx_point_match_2d.h"
#include "virg/nexus/nx_sift_detector.h"
#include "virg/nexus/nx_homography.h"

#define SIFT_DISTANCE_RATIO_THR 0.6f
#define INIT_MIN_N_INLIERS 20
#define INIT_HOMOGRAPHY_INLIER_THRESHOLD 3.0f
#define INIT_HOMOGRAPHY_MAX_N_RANSAC_ITERS 10000
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
        int reference_image;

        // per image data
        int n_images; // N
        struct NXStringArray *image_names;
        struct NXImage **images;
        int *n_keys;
        int *max_n_keys;
        struct NXKeypoint **keys;
        uchar **desc;
        int *total_n_corr;
        double **H_to_ref;
        double **H_from_ref;

        // per image-pair data
        int *n_matches; // NxN matrix of int
        struct NXPointMatch2D**matches; // NxN matrix of struct NXPointMatch2D *
        double **H; // NxN matrix of double *
        double *affinity; // NxN matrix of double *

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
        builder->total_n_corr = NX_NEW_I(N);
        builder->H_to_ref = NX_NEW(N, double *);
        builder->H_from_ref = NX_NEW(N, double *);
        for (int i = 0; i < N;  ++i) {
                builder->images[i] = nx_image_alloc();
                nx_image_xload(builder->images[i],
                               nx_string_array_get(builder->image_names, i),
                               NX_IMAGE_LOAD_GRAYSCALE);
                builder->n_keys[i] = 0;
                builder->max_n_keys[i] = N_INITIAL_KEY_BUFFER_SIZE;
                builder->keys[i] = NX_NEW(N_INITIAL_KEY_BUFFER_SIZE, struct NXKeypoint);
                builder->desc[i] = NX_NEW_UC(N_INITIAL_KEY_BUFFER_SIZE * NX_SIFT_DESC_DIM);
                builder->total_n_corr[i] = 0;
                builder->H_to_ref[i] = NX_NEW_D(9);
                builder->H_from_ref[i] = NX_NEW_D(9);
        }

        // Allocate per image-pair data
        builder->n_matches = NX_NEW_I(N*N);
        memset(builder->n_matches, 0, N*N*sizeof(builder->n_matches[0]));

        builder->matches = NX_NEW(N*N, struct NXPointMatch2D *);
        memset(builder->matches, 0, N*N*sizeof(builder->matches[0]));

        builder->H = NX_NEW(N*N, double *);
        memset(builder->H, 0, N*N*sizeof(builder->H[0]));

        builder->affinity = NX_NEW_D(N*N);
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
                        nx_free(builder->H_to_ref[i]);
                        nx_free(builder->H_from_ref[i]);
                }
                nx_free(builder->images);
                nx_free(builder->n_keys);
                nx_free(builder->max_n_keys);
                nx_free(builder->keys);
                nx_free(builder->desc);
                nx_free(builder->total_n_corr);
                nx_free(builder->H_to_ref);
                nx_free(builder->H_from_ref);

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
        NX_ASSERT(INIT_MIN_N_INLIERS >= 4);

        double h[9];
        int n_inliers = 0;
        if (n_corr >= INIT_MIN_N_INLIERS) {
                n_inliers = nx_homography_estimate_norm_ransac(&h[0],
                                                               n_corr, corr,
                                                               INIT_HOMOGRAPHY_INLIER_THRESHOLD,
                                                               INIT_HOMOGRAPHY_MAX_N_RANSAC_ITERS);
        }

        if (n_inliers >= INIT_MIN_N_INLIERS) {
                if (!*hp)
                        *hp = NX_NEW_D(9);
                nx_dmat3_copy(*hp, &h[0]);
        } else {
                n_inliers = 0;
                if (*hp) {
                        nx_free(*hp);
                        *hp = NULL;
                }
        }

        return n_inliers;
}

int *nx_panorama_builder_find_shortest_paths(int n, double* weights, int source)
{
        int *next_node = NX_NEW_I(n);
        for (int i = 0; i < n; ++i)
                next_node[i] = -1;
        next_node[source] = source;

        double *node_val = NX_NEW_D(n);
        nx_dvec_set_zero(n, node_val);
        node_val[source] = DBL_MAX;

        NXBool *visited = NX_NEW_B(n);
        for (int i = 0; i < n; ++i)
                visited[i] = NX_FALSE;
        int n_not_visited = n;

        while (n_not_visited > 0) {
                int current = -1;
                double current_val = 0.0;
                for (int i = 0; i < n; ++i) {
                        if (!visited[i] && node_val[i] > current_val) {
                                current = i;
                                current_val = node_val[i];
                        }
                }

                visited[current] = NX_TRUE;
                n_not_visited--;

                for (int i = 0; i < n; ++i) {
                        if (i == current || visited[i])
                                continue;

                        double w = weights[current * n + i];
                        double val = node_val[i] + w;
                        if (val > node_val[i]) {
                                node_val[i] = val;
                                next_node[i] = current;
                        }
                }
        }

        nx_free(node_val);

        return next_node;
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
        nx_ivec_set_zero(N, builder->total_n_corr);
        nx_dvec_set_zero(N*N, builder->affinity);

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
                        builder->total_n_corr[i] += ni;
                        builder->total_n_corr[j] += ni;
                        builder->affinity[j*N + i] = ni;
                        NX_LOG(NX_LOG_TAG, "%2d <-> %2d : %d inliers --> %s",
                               i, j, ni, *hp ? "OK" : "FAILED");
                }
        }

        fprintf(stderr, "------- Affinity -------\n");
        for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                        if (i == j)
                                fprintf(stderr, "%4s ", "-");
                        else
                                fprintf(stderr, "%4.0f ",
                                        builder->affinity[j*N + i]);
                }
                fprintf(stderr, "\n");
        }
        fprintf(stderr, "-------\n");

        // Pick central node
        builder->reference_image = nx_ivec_max_idx(N, builder->total_n_corr);
        NX_LOG(NX_LOG_TAG, "Selecting image %d as the reference",
               builder->reference_image);

        int *next_node = nx_panorama_builder_find_shortest_paths(N,
                                                                 builder->affinity,
                                                                 builder->reference_image);
        for (int i = 0; i < N; ++i) {
                NX_LOG(NX_LOG_TAG, "%d --> %d", i, next_node[i]);
        }

        for (int i = 0; i < N; ++i) {
                nx_dmat3_eye(builder->H_to_ref[i]);
                int node = i;
                int next = next_node[node];
                fprintf(stderr, "%d", node);
                while (node != next) {
                        fprintf(stderr, " .. %d", next);
                        const double *H_to_next = builder->H[next * N + node];
                        NX_ASSERT_PTR(H_to_next);

                        double H[9];
                        nx_dmat3_mul(&H[0], H_to_next, builder->H_to_ref[i]);
                        nx_dmat3_copy(builder->H_to_ref[i], &H[0]);

                        node = next;
                        next = next_node[node];
                }
                fprintf(stderr, "\n");
        }
        nx_free(next_node);
}

void nx_panorama_builder_refine_geometric(struct NXPanoramaBuilder *builder)
{
        NX_ASSERT_PTR(builder);
}

void nx_panorama_builder_refine_photometric(struct NXPanoramaBuilder *builder)
{
        NX_ASSERT_PTR(builder);
}

void nx_panorama_builder_create_reference(struct NXPanoramaBuilder *builder)
{
        const int N = builder->n_images;

        struct {
                int min_x;
                int min_y;
                int max_x;
                int max_y;
        } dims = { INT_MAX, INT_MAX, INT_MIN, INT_MIN };

        for (int i = 0; i < N; ++i) {
                const double *H = builder->H_to_ref[i];
                nx_dmat3_print(H, "H_to_ref_i");
                float wi = builder->images[i]->width;
                float hi = builder->images[i]->height;

                float x[4][2] = { {0.0f, 0.0f}, {wi, 0.0f},
                                   {wi, hi},    {0.0f, hi} };
                float xp[2];
                for (int j = 0; j < 4; ++j) {
                        nx_homography_transfer_fwd(H, &xp[0], &x[j][0]);
                        fprintf(stderr, "%.0f, %.0f --> %.2f, %.2f\n",
                                x[j][0], x[j][1], xp[0], xp[1]);
                        dims.min_x = nx_min_i(dims.min_x, (int)(xp[0]));
                        dims.min_y = nx_min_i(dims.min_y, (int)(xp[1]));
                        dims.max_x = nx_max_i(dims.max_x, (int)(xp[0]));
                        dims.max_y = nx_max_i(dims.max_y, (int)(xp[1]));
                }
        }
        fprintf(stderr, "dims: %d %d %d %d\n",
                dims.min_x, dims.min_y, dims.max_x, dims.max_y);

        // resize panorama
        int width  = dims.max_x - dims.min_x;
        int height = dims.max_y - dims.min_y;
        nx_image_resize(builder->panorama, width, height,
                        NX_IMAGE_STRIDE_DEFAULT,
                        NX_IMAGE_GRAYSCALE, NX_IMAGE_UCHAR);

        // shift origin
        for (int i = 0; i < N; ++i) {
                double *H = builder->H_to_ref[i];
                H[0] -= dims.min_x * H[2];
                H[3] -= dims.min_x * H[5];
                H[6] -= dims.min_x * H[8];
                H[1] -= dims.min_y * H[2];
                H[4] -= dims.min_y * H[5];
                H[7] -= dims.min_y * H[8];
        }

        // compute inverses
        for (int i = 0; i < N; ++i) {
                nx_dmat3_inv(builder->H_from_ref[i],
                             builder->H_to_ref[i]);
        }
}

uchar nx_panorama_builder_composite_value(struct NXPanoramaBuilder *builder,
                                          int x, int y)
{
        const int N = builder->n_images;

        uchar v = 0;
        for (int i = 0; i < N; ++i) {
                const double *H = builder->H_from_ref[i];
                float xc[2] = { x, y };
                float xpc[2];
                nx_homography_transfer_fwd(H, &xpc[0], &xc[0]);

                struct NXImage *img = builder->images[i];
                float xp = xpc[0];
                float yp = xpc[1];
                int xpi = xp;
                int ypi = yp;
                if (xpi >= 0 && xpi < img->width
                    && ypi >= 0 && ypi < img->height) {
                        v = img->data.uc[ypi * img->row_stride + xpi];
                }
        }

        return v;
}

void nx_panorama_builder_composite(struct NXPanoramaBuilder *builder)
{
        NX_ASSERT_PTR(builder);

        nx_panorama_builder_create_reference(builder);
        struct NXImage *pan = builder->panorama;
        NX_LOG(NX_LOG_TAG, "Compositing into panorama of size %dx%d",
               pan->width, pan->height);

        nx_image_set_zero(pan);
        for (int y = 0; y < pan->height; ++y) {
                uchar *row = pan->data.uc + y * pan->row_stride;
                for (int x = 0; x < pan->width; ++x) {
                        row[x] = nx_panorama_builder_composite_value(builder,
                                                                     x, y);
                }
        }
}
