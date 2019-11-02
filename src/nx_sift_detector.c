/**
 * @file nx_sift_detector.c
 *
 * Copyright (C) 2019 Mustafa Ozuysal. All rights reserved.
 *
 * This file is part of the VIRG-Nexus Library
 *
 * Note that SIFT is covered by a US Patent: "Method and apparatus for
 * identifying scale invariant features in an image and use of same for locating
 * an object in an image," David G. Lowe, US Patent 6,711,293 (March 23,
 * 2004). Provisional application filed March 8, 1999. Asignee: The University
 * of British Columbia.
 *
 * For further details, contact David Lowe (lowe@cs.ubc.ca) or the
 * University-Industry Liaison Office of the University of British Columbia.
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
#include "virg/nexus/nx_sift_detector.h"

#include <math.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_mat234.h"
#include "virg/nexus/nx_vec234.h"

#define NX_SIFT_N_ORI_BINS 36

struct NXSIFTDetectorParams nx_sift_default_parameters()
{
        struct NXSIFTDetectorParams params;
        params.double_image = NX_TRUE;
        params.n_scales_per_octave = 3;
        params.sigma0 = 1.6f;
        params.kernel_truncation_factor = 4;
        params.border_distance = 5;
        params.peak_threshold = 0.08f;
        params.edge_threshold = 10.0f;
        params.magnification_factor = 3;

        return params;
}


struct NXSIFTDetector
{
        struct NXSIFTDetectorParams param;

        struct NXImage *gx;
        struct NXImage *gy;

        struct NXImage **levels;
        struct NXImage **dogs;
};

struct NXSIFTDetector *nx_sift_detector_new(struct NXSIFTDetectorParams sift_param)
{
        const int n_scales = sift_param.n_scales_per_octave;

        struct NXSIFTDetector *det = NX_NEW(1, struct NXSIFTDetector);
        det->param = sift_param;

        det->gx = nx_image_alloc();
        det->gy = nx_image_alloc();

        det->levels = NX_NEW(n_scales + 3, struct NXImage *);
        det->dogs   = NX_NEW(n_scales + 2, struct NXImage *);
        for (int i = 0; i < n_scales + 2; ++i) {
                det->levels[i] = nx_image_alloc();
                det->dogs[i] = nx_image_alloc();
        }
        det->levels[n_scales + 2] = nx_image_alloc();

        return det;
}

void nx_sift_detector_free(struct NXSIFTDetector *det)
{
        if (det) {
                const int n_scales = det->param.n_scales_per_octave;
                for (int i = 0; i < n_scales + 2; ++i) {
                        nx_image_free(det->levels[i]);
                        nx_image_free(det->dogs[i]);
                }
                nx_image_free(det->levels[n_scales + 2]);

                nx_free(det->levels);
                nx_free(det->dogs);

                nx_image_free(det->gx);
                nx_image_free(det->gy);

                nx_free(det);
        }
}

static inline float kernel_sigma(float sigma_current, float sigma_desired)
{
        float sigma_g = sqrt(sigma_desired*sigma_desired
                             - sigma_current*sigma_current);
        return sigma_g;
}

struct NXSIFTKeyStore {
        int n;
        int cap;
        struct NXKeypoint *keys;
        uchar *desc;
};

static int nx_sift_key_store_append(struct NXSIFTKeyStore *store)
{
        if (store->n <= store->cap) {
                int new_cap = (int)(store->cap * 1.6f);
                store->keys = (struct NXKeypoint *)nx_xrealloc(store->keys,
                                                               new_cap * sizeof(struct NXKeypoint));
                store->desc = (uchar *)nx_xrealloc(store->desc,
                                                   new_cap*NX_SIFT_DESC_DIM*sizeof(uchar));
                store->cap = new_cap;
        }

        NX_ASSERT(store->n < store->cap);
        int id = store->n;
        store->n++;
        return id;
}

static void nx_sift_detector_prepare_octave(struct NXSIFTDetector *det,
                                            float sigma_c)
{
        const int n_scales = det->param.n_scales_per_octave;
        float scale_multiplier = pow(2.0, 1.0 / n_scales);
        for (int i = 1; i < n_scales + 3; ++i) {
                nx_image_copy(det->levels[i], det->levels[i-1]);
                float sigma_g = kernel_sigma(sigma_c, scale_multiplier * sigma_c);
                nx_image_smooth(det->levels[i], det->levels[i],
                                sigma_g, sigma_g, NULL);
                sigma_c *= scale_multiplier;
        }

        for (int i = 0; i < n_scales + 2; ++i) {
                nx_image_subtract(det->dogs[i],
                                  det->levels[i], det->levels[i+1]);
        }
}

static float nx_sift_compute_ori_hist(float *hist, const struct NXImage *gx,
                                      const struct NXImage *gy, float x, float y,
                                      float sigma)
{
        memset(hist, 0, NX_SIFT_N_ORI_BINS*sizeof(*hist));

        sigma *= 1.5f; // Grow sigma for orientation estimation
        int r = (int)(sigma * 3.0f);
        int w = gx->width;
        int h = gx->height;
        int xi = (int)(x + 0.5f);
        int yi = (int)(y + 0.5f);

        for (int v = yi - r; v <= yi + r; ++v) {
                if (v <= 0 || v >= h - 1)
                        continue;
                const float *rx = gx->data.f32 + v * gx->row_stride;
                const float *ry = gy->data.f32 + v * gy->row_stride;
                for (int u = xi - r; u <= xi + r; ++u) {
                        float dr_sq = (u - x) * (u - x) + (v - y) * (v - y);
                        if (u <= 0 || u >= w - 1 || dr_sq > (r * r + 0.5))
                                continue;
                        float gx_val = rx[u];
                        float gy_val = ry[u];
                        float gmag = sqrt(gx_val*gx_val + gy_val*gy_val);
                        float gori = atan2(gy_val, gx_val);
                        
                }
        }
}

static void nx_sift_compute_keys(struct NXSIFTDetector *det,
                                 struct NXSIFTKeyStore *store,
                                 int octave, float sigma_c,
                                 float i, float x, float y)
{
        const int n_scales = det->param.n_scales_per_octave;

        float ori_hist[NX_SIFT_N_ORI_BINS];
        float sigma = sigma_c * pow(2.0, (octave + i) / n_scales);
        float hist_peak = nx_sift_compute_ori_hist(&ori_hist[0],
                                                   det->gx, det->gy,
                                                   x, y, sigma);
}

static void nx_sift_interp_peak_location(struct NXSIFTDetector *det,
                                         struct NXSIFTKeyStore *store,
                                         int octave, float sigma_c,
                                         int i, int x, int y)
{
        float dval;
        double b[3];

        const int n_scales = det->param.n_scales_per_octave;
        const float peak_thr = det->param.peak_threshold / n_scales;

        const struct NXImage *dog = det->dogs[i];
        const struct NXImage *dogm = det->dogs[i-1];
        const struct NXImage *dogp = det->dogs[i+1];

        int n_tries = 5;
        while (n_tries-- > 0) {
                const float *dogm_rowm  = dogm->data.f32 + (y-1) * dogm->row_stride;
                const float *dogm_row   = dogm_rowm + dogm->row_stride;
                const float *dogm_rowp  = dogm_row  + dogm->row_stride;
                const float *dog_rowm   = dog->data.f32 + (y-1) * dog->row_stride;
                const float *dog_row    = dog_rowm + dog->row_stride;
                const float *dog_rowp   = dog_row  + dog->row_stride;
                const float *dogp_rowm  = dogp->data.f32 + (y-1) * dogp->row_stride;
                const float *dogp_row   = dogp_rowm + dogp->row_stride;
                const float *dogp_rowp  = dogp_row  + dogp->row_stride;

                double g[3] = {
                        (dog_row[x+1] - dog_row[x-1]) / 2.0,   // dx
                        (dog_rowp[x]  - dog_rowm[x])  / 2.0,   // dy
                        (dogp_row[x]  - dogm_row[x])  / 2.0 }; // ds
                b[0] = -g[0];
                b[1] = -g[1];
                b[2] = -g[2];
                double A[9] = {
                        dog_row[x-1] - 2.0 * dog_row[x] + dog_row[x+1], // Hxx
                        ((dog_rowp[x+1] - dog_rowp[x-1]) -
                         (dog_rowm[x+1] - dog_rowm[x-1])) / 4.0,        // Hxy
                        ((dogp_row[x+1] - dogp_row[x-1]) -
                         (dogm_row[x+1] - dogm_row[x-1])) / 4.0,        // Hxs
                        0.0,
                        dog_rowm[x]  - 2.0 * dog_row[x] + dog_rowp[x],  // Hyy
                        ((dogp_rowp[x] - dogp_rowm[x]) -
                         (dogm_rowp[x] - dogm_rowm[x])) / 4.0,          // Hys
                        0.0,
                        0.0,
                        dogm_row[x]  - 2.0 * dog_row[x] + dogp_row[x] };// Hss

                NXBool sv_res = nx_dmat3_sym_solve_single(&A[0], &b[0]);
                if (!sv_res)
                        return;

                dval = dog_row[x] + 0.5f * (float)nx_dvec3_dot(&g[0], &b[0]);

                int dx = 0;
                int dy = 0;
                if (b[0] > 0.6 && x < dog->width - 3)
                        ++dx;
                else if (b[0] < -0.6 && x > 3)
                        --dx;
                if (b[1] > 0.6 && y < dog->height - 3)
                        ++dy;
                else if (b[1] < -0.6 && y > 3)
                        --dy;
                if (dx == 0 && dy == 0)
                        break;
                x += dx;
                y += dy;
        }

        if (fabs(b[0]) > 1.5 || fabs(b[1]) > 1.5 || fabs(b[2]) > 1.5
            || fabs(dval) < peak_thr)
                return;

        nx_sift_compute_keys(det, store, octave, sigma_c,
                             i + b[2], x + b[0], y + b[1]);
}

static inline NXBool nx_sift_check_edge_threshold(const float *dog_rowm,
                                                  const float *dog_row,
                                                  const float *dog_rowp,
                                                  int x, float edge_thr)
{
        const float edge_thr_p1 = edge_thr + 1.0f;

        float HA = dog_rowm[x] - 2.0 * dog_row[x] + dog_rowp[x];
        float HB = dog_row[x-1] - 2.0 * dog_row[x] + dog_row[x+1];
        float HC = ((dog_rowp[x+1] - dog_rowp[x-1])
                    - (dog_rowm[x+1] - dog_rowm[x-1])) / 4.0f;
        float det = HA * HB - HC * HC;
        float tr  = HA + HB;

        return det * edge_thr_p1 * edge_thr_p1 > edge_thr * tr * tr;
}

static void nx_sift_process_dog(struct NXSIFTDetector *det,
                                struct NXSIFTKeyStore *store,
                                int octave, float sigma_c, int i)
{
        const int n_scales = det->param.n_scales_per_octave;
        const float peak_thr = det->param.peak_threshold / n_scales;
        const float edge_thr = det->param.edge_threshold;

        const struct NXImage *dog = det->dogs[i];
        const struct NXImage *dogm = det->dogs[i-1];
        const struct NXImage *dogp = det->dogs[i+1];

        const int B = det->param.border_distance;
        for (int y = B; y < dog->height - B; ++y) {
                const float *dogm_rowm  = dogm->data.f32 + (y-1) * dogm->row_stride;
                const float *dogm_row   = dogm_rowm + dogm->row_stride;
                const float *dogm_rowp  = dogm_row  + dogm->row_stride;
                const float *dog_rowm   = dog->data.f32 + (y-1) * dog->row_stride;
                const float *dog_row    = dog_rowm + dog->row_stride;
                const float *dog_rowp   = dog_row  + dog->row_stride;
                const float *dogp_rowm  = dogp->data.f32 + (y-1) * dogp->row_stride;
                const float *dogp_row   = dogp_rowm + dogp->row_stride;
                const float *dogp_rowp  = dogp_row  + dogp->row_stride;
                for (int x = B; x < dog->width + B; ++x) {
                        float dval = dog_row[x];
                        if (fabs(dval) > 0.8 * peak_thr
                            && ((dval > 0 && dval > dog_row[x-1]   && dval > dog_row[x+1]
                                 && dval > dog_rowm[x-1]  && dval > dog_rowm[x]  && dval > dog_rowm[x+1]
                                 && dval > dog_rowp[x-1]  && dval > dog_rowp[x]  && dval > dog_rowp[x+1]
                                 && dval > dogm_row[x-1]  && dval > dogm_row[x]  && dval > dogm_row[x+1]
                                 && dval > dogm_rowm[x-1] && dval > dogm_rowm[x] && dval > dogm_rowm[x+1]
                                 && dval > dogm_rowp[x-1] && dval > dogm_rowp[x] && dval > dogm_rowp[x+1]
                                 && dval > dogp_row[x-1]  && dval > dogp_row[x]  && dval > dogp_row[x+1]
                                 && dval > dogp_rowm[x-1] && dval > dogp_rowm[x] && dval > dogp_rowm[x+1]
                                 && dval > dogp_rowp[x-1] && dval > dogp_rowp[x] && dval > dogp_rowp[x+1])
                                || (dval < 0 && dval < dog_row[x-1] && dval < dog_row[x+1]
                                    && dval < dog_rowm[x-1] && dval < dog_rowm[x] && dval < dog_rowm[x+1]
                                    && dval < dog_rowp[x-1] && dval < dog_rowp[x] && dval < dog_rowp[x+1]
                                    && dval < dogm_row[x-1] && dval < dogm_row[x] && dval < dogm_row[x+1]
                                    && dval < dogm_rowm[x-1] && dval < dogm_rowm[x] && dval < dogm_rowm[x+1]
                                    && dval < dogm_rowp[x-1] && dval < dogm_rowp[x] && dval < dogm_rowp[x+1]
                                    && dval < dogp_row[x-1] && dval < dogp_row[x] && dval < dogp_row[x+1]
                                    && dval < dogp_rowm[x-1] && dval < dogp_rowm[x] && dval < dogp_rowm[x+1]
                                    && dval < dogp_rowp[x-1] && dval < dogp_rowp[x] && dval < dogp_rowp[x+1]))
                            && nx_sift_check_edge_threshold(dog_rowm, dog_row,
                                                            dog_rowp, x, edge_thr)) {
                                nx_sift_interp_peak_location(det, store,
                                                             octave, sigma_c,
                                                             i, x, y);
                        }
                }
        }
}

static void nx_sift_detector_process_octave(struct NXSIFTDetector *det,
                                            struct NXSIFTKeyStore *store,
                                            int octave,
                                            float sigma_c)
{
        const int n_scales = det->param.n_scales_per_octave;
        for (int i = 1; i < n_scales + 1; ++i) {
                nx_image_deriv_x(det->gx, det->dogs[i]);
                nx_image_deriv_y(det->gy, det->dogs[i]);
                nx_sift_process_dog(det, store, octave, sigma_c, i);
        }
}

int nx_sift_detector_compute(struct NXSIFTDetector *det,
                             struct NXImage *image,
                             int *max_n_keys,
                             struct NXKeypoint **keys,
                             uchar **desc)
{
        NX_ASSERT_PTR(det);
        NX_ASSERT_PTR(image);
        NX_ASSERT_PTR(keys);
        NX_ASSERT_PTR(desc);

        const int MIN_DIM = 32;

        float sigma_c = 0.5;
        nx_image_resize(det->levels[0], image->width, image->height,
                        NX_IMAGE_STRIDE_DEFAULT,
                        NX_IMAGE_GRAYSCALE, NX_IMAGE_FLOAT32);
        nx_image_convert_dtype(det->levels[0], image);

        if (det->param.sigma0 > sigma_c) {
                float sigma_g = kernel_sigma(sigma_c, det->param.sigma0);
                nx_image_smooth(det->levels[0], det->levels[0],
                                sigma_g, sigma_g, NULL);
                sigma_c = det->param.sigma0;
        }

        struct NXSIFTKeyStore store;
        store.n = 0;
        store.cap = *max_n_keys;
        store.keys = *keys;
        store.desc = *desc;

        int octave = 0;
        while (det->levels[0]->width > MIN_DIM
               && det->levels[0]->height > MIN_DIM) {
                nx_sift_detector_prepare_octave(det, sigma_c);
                nx_sift_detector_process_octave(det, &store, octave, sigma_c);
                nx_image_downsample(det->levels[0],
                                    det->levels[det->param.n_scales_per_octave]);
                octave++;
        }

        *max_n_keys = store.cap;
        *keys = store.keys;
        *desc = store.desc;

        return store.n;
}
