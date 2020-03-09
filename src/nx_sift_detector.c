/**
 * @file nx_sift_detector.c
 *
 * Copyright (C) 2019,2020 Mustafa Ozuysal. All rights reserved.
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
 * You should have received a copy of the GNU General Public License along with
 * VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
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
#include "virg/nexus/nx_string.h"
#include "virg/nexus/nx_filesystem.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_image_io.h"
#include "virg/nexus/nx_io.h"
#include "virg/nexus/nx_math.h"
#include "virg/nexus/nx_mat234.h"
#include "virg/nexus/nx_vec234.h"
#include "virg/nexus/nx_vec.h"
#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_hash_sha256.h"

#define NX_SIFT_N_ORI_BINS 36

void nx_sift_parameters_add_to_options(struct NXOptions *opt)
{
        struct NXSIFTDetectorParams default_params = nx_sift_default_parameters();
        nx_options_add(opt, "biddiddi",
                       "--sift-double-image", "double input image size before computation", NX_FALSE,
                       "--sift-n-scales-per-octave", "number of intermediate scales within each octave", default_params.n_scales_per_octave,
                       "--sift-sigma0", "initial sigma for the input image", (double)default_params.sigma0,
                       "--sift-kernel-truncation-factor", "multipler for filter size computation", (double)default_params.kernel_truncation_factor,
                       "--sift-border-distance", "distance to border within which to skip extraction", default_params.border_distance,
                       "--sift-peak-threshold", "DoG score threshold, decrease to get more keypoints", (double)default_params.peak_threshold,
                       "--sift-edge-threshold", "threshold for filtering edge like regions", (double)default_params.edge_threshold,
                       "--sift-magnification-factor", "multipler to determine descriptor radius", default_params.magnification_factor);
}

struct NXSIFTDetectorParams
nx_sift_parameters_from_options(struct NXOptions *opt)
{
        struct NXSIFTDetectorParams params;
        params.double_image = nx_options_get_bool(opt, "--sift-double-image");
        params.n_scales_per_octave = nx_options_get_int(opt, "--sift-n-scales-per-octave");
        params.sigma0 = nx_options_get_double(opt, "--sift-sigma0");
        params.kernel_truncation_factor = nx_options_get_double(opt, "--sift-kernel-truncation-factor");
        params.border_distance = nx_options_get_int(opt, "--sift-border-distance");
        params.peak_threshold = nx_options_get_double(opt, "--sift-peak-threshold");
        params.edge_threshold = nx_options_get_double(opt, "--sift-edge-threshold");
        params.magnification_factor = nx_options_get_int(opt, "--sift-magnification-factor");

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

int nx_sift_key_store_append(struct NXSIFTKeyStore *store)
{
        if (store->n >= store->cap) {
                int new_cap = (int)(store->cap * 1.6f);
                store->keys = (struct NXKeypoint *)nx_xrealloc(store->keys,
                                                               new_cap*sizeof(struct NXKeypoint));
                store->desc = (uchar *)nx_xrealloc(store->desc,
                                                   new_cap*NX_SIFT_DESC_DIM*sizeof(uchar));
                store->cap = new_cap;
        }

        NX_ASSERT(store->n < store->cap);
        int id = store->n;
        store->n++;
        return id;
}

void nx_sift_compute_fdescriptor(struct NXSIFTDetector *det,
                                 struct NXKeypoint *key,
                                 float *desc)
{
        const int N_PATCH_XY = 4;
        const int N_ORI_BINS = 8;
        NX_ASSERT(N_PATCH_XY * N_PATCH_XY * N_ORI_BINS == NX_SIFT_DESC_DIM);

        nx_svec_set_zero(NX_SIFT_DESC_DIM, desc);

        float patch_size = key->sigma * det->param.magnification_factor;
        float radius_in_patches = (N_PATCH_XY + 1.0f) / 2.0f;
        int sample_radius = (int)(patch_size * radius_in_patches
                                  * sqrtf(2.0f) + 0.5f);
        float cori = cosf(key->ori);
        float sori = sinf(key->ori);
        float rx_offset = key->xs - key->x;
        float ry_offset = key->ys - key->y;

        for (int dy = -sample_radius; dy <= +sample_radius; ++dy) {
                int sample_y = dy + key->y;
                // skip if sample is out of the image
                if (sample_y < 0 || sample_y >= det->gx->height)
                        continue;
                const float *gx_row = det->gx->data.f32 + sample_y * det->gx->row_stride;
                const float *gy_row = det->gy->data.f32 + sample_y * det->gy->row_stride;
                for (int dx = -sample_radius; dx <= +sample_radius; ++dx) {
                        int sample_x = dx + key->x;
                        // skip if sample is out of the image
                        if (sample_x < 0 || sample_x >= det->gx->width)
                                continue;

                        // calculate sample's patch coordinates
                        float ry =  cori * dy + sori * dx;
                        float rx = -sori * dy + cori * dx;
                        float patch_x = (rx - rx_offset) / patch_size;
                        float patch_y = (ry - ry_offset) / patch_size;

                        if (patch_x > -radius_in_patches
                            && patch_x < radius_in_patches
                            && patch_y > -radius_in_patches
                            && patch_y < radius_in_patches) {
                                float gx = gx_row[sample_x];
                                float gy = -gy_row[sample_x];
                                float gmag = sqrtf(gx*gx + gy*gy);
                                float gori = atan2f(gy, gx);
                                float patch_sigma = 0.5f * N_PATCH_XY;
                                float gweight = expf(-0.5f * (patch_x*patch_x + patch_y*patch_y)
                                                     / (patch_sigma * patch_sigma));
                                // Calculate sample values
                                float sample_weight = gweight * gmag;
                                float sample_ori = gori - key->ori;
                                while (sample_ori > 2*NX_PI)
                                        sample_ori -= 2*NX_PI;
                                while (sample_ori < 0.0f)
                                        sample_ori += 2*NX_PI;

                                // Update descriptor
                                float px = patch_x + N_PATCH_XY / 2 - 0.5f;
                                float py = patch_y + N_PATCH_XY / 2 - 0.5f;
                                float h  = N_ORI_BINS * (sample_ori / (2.0 * NX_PI));
                                int pxi = px > 0.0f ? (int)px : (int)(px - 1.0f); // floor
                                int pyi = py > 0.0f ? (int)py : (int)(py - 1.0f); // floor
                                int hi  = h  > 0.0f ? (int)h  : (int)(h  - 1.0f); // floor
                                int hi0 = (hi >= N_ORI_BINS) ? 0 : hi;
                                int hi1 = ((hi0 + 1) >= N_ORI_BINS) ? 0 : hi0 + 1;
                                float xeps = px - pxi;
                                float yeps = py - pyi;
                                float heps = h  - hi;
                                if (pyi >= 0 && pyi < N_PATCH_XY) {
                                        float w = sample_weight * (1.0f - yeps);
                                        float *prow = desc + pyi * N_PATCH_XY * N_ORI_BINS;
                                        if (pxi >= 0 && pxi < N_PATCH_XY) {
                                                float ww = w * (1.0f - xeps);
                                                float ww1 = ww * heps;
                                                float ww0 = ww - ww1;
                                                prow[pxi * N_ORI_BINS + hi0] += ww0;
                                                prow[pxi * N_ORI_BINS + hi1] += ww1;
                                        }
                                        if ((pxi+1) >= 0 && (pxi+1) < N_PATCH_XY) {
                                                float ww = w * xeps;
                                                float ww1 = ww * heps;
                                                float ww0 = ww - ww1;
                                                prow[(pxi+1) * N_ORI_BINS + hi0] += ww0;
                                                prow[(pxi+1) * N_ORI_BINS + hi1] += ww1;
                                        }
                                }
                                if ((pyi+1) >= 0 && (pyi+1) < N_PATCH_XY) {
                                        float w = sample_weight * yeps;
                                        float *prow = desc + (pyi+1) * N_PATCH_XY * N_ORI_BINS;
                                        if (pxi >= 0 && pxi < N_PATCH_XY) {
                                                float ww = w * (1.0f - xeps);
                                                float ww1 = ww * heps;
                                                float ww0 = ww - ww1;
                                                prow[pxi * N_ORI_BINS + hi0] += ww0;
                                                prow[pxi * N_ORI_BINS + hi1] += ww1;
                                        }
                                        if ((pxi+1) >= 0 && (pxi+1) < N_PATCH_XY) {
                                                float ww = w * xeps;
                                                float ww1 = ww * heps;
                                                float ww0 = ww - ww1;
                                                prow[(pxi+1) * N_ORI_BINS + hi0] += ww0;
                                                prow[(pxi+1) * N_ORI_BINS + hi1] += ww1;
                                        }
                                }
                        }
                }
        }
}

void nx_sift_compute_descriptor(struct NXSIFTDetector *det,
                                struct NXKeypoint *key,
                                uchar *desc)
{
        float fdesc[NX_SIFT_DESC_DIM];
        nx_sift_compute_fdescriptor(det, key, &fdesc[0]);
        nx_svec_to_unit(NX_SIFT_DESC_DIM, &fdesc[0]);

        for (int i = 0; i < NX_SIFT_DESC_DIM; ++i)
                if (fdesc[i] > 0.2f)
                        fdesc[i] = 0.2f;
        nx_svec_to_unit(NX_SIFT_DESC_DIM, &fdesc[0]);

        for (int i = 0; i < NX_SIFT_DESC_DIM; ++i)
                desc[i] = nx_min_i(255, (int)(512.0f * fdesc[i]));
}

float nx_sift_compute_ori_hist(float *hist, const struct NXImage *gx,
                               const struct NXImage *gy, float x, float y,
                               float sigma)
{
        nx_svec_set_zero(NX_SIFT_N_ORI_BINS, &hist[1]);

        sigma *= 1.5f; // Grow sigma for orientation estimation
        const float sigma_sq = sigma * sigma;
        const float dist_factor = -0.5 / sigma_sq;

        int r = (int)(sigma * 3.0f);
        int w = gx->width;
        int h = gx->height;
        int xi = (int)(x + 0.5f);
        int yi = (int)(y + 0.5f);
        for (int v = yi - r; v <= yi + r; ++v) {
                if (v <= 0 || v >= h - 2)
                        continue;
                const float *rx = gx->data.f32 + v * gx->row_stride;
                const float *ry = gy->data.f32 + v * gy->row_stride;
                for (int u = xi - r; u <= xi + r; ++u) {
                        // skip if outside image border
                        if (u <= 0 || u >= w - 2)
                                continue;

                        float dr_sq = (u - x) * (u - x) + (v - y) * (v - y);
                        // skip if too far from the center
                        if (dr_sq > (r * r + 0.5))
                                continue;

                        float gx_val = rx[u];
                        float gy_val = -ry[u];
                        float gmag = sqrtf(gx_val*gx_val + gy_val*gy_val);
                        float gori = atan2f(gy_val, gx_val);
                        int bin = (int)(NX_SIFT_N_ORI_BINS * (gori + NX_PI + 0.00001f)
                                        / (2.0 * NX_PI));
                        if (bin >= NX_SIFT_N_ORI_BINS)
                                bin = NX_SIFT_N_ORI_BINS - 1;
                        hist[bin+1] += expf(dist_factor * dr_sq) * gmag;
                }
        }

        // Smooth buffer by averaging multiple times using an auxilary buffer to
        // handle boundaries
        float hist_buffer[NX_SIFT_N_ORI_BINS+2];
        for (int i = 0; i < 6; ++i) {
                memcpy(&hist_buffer[1], &hist[1], NX_SIFT_N_ORI_BINS*sizeof(*hist));
                hist_buffer[0] = hist_buffer[NX_SIFT_N_ORI_BINS];
                hist_buffer[NX_SIFT_N_ORI_BINS+1] = hist_buffer[1];
                for (int j = 1; j <= NX_SIFT_N_ORI_BINS; ++j)
                        hist_buffer[j-1] = (hist_buffer[j-1] + hist_buffer[j]
                                            + hist_buffer[j+1]) / 3.0f;
                memcpy(&hist[1], &hist_buffer[0], NX_SIFT_N_ORI_BINS*sizeof(*hist));
        }
        // create cyclic border of length one
        hist[0] = hist_buffer[NX_SIFT_N_ORI_BINS];
        hist[NX_SIFT_N_ORI_BINS+1] = hist[1];

        // Compute and return histogram peak
        float hist_peak = hist[1];
        for (int i = 2; i <= NX_SIFT_N_ORI_BINS; ++i)
                if (hist[i] > hist_peak)
                        hist_peak = hist[i];

        return hist_peak;
}

void nx_sift_compute_keys(struct NXSIFTDetector *det,
                          struct NXSIFTKeyStore *store,
                          int octave, float sigma_c, float dog_val,
                          float i, float x, float y)
{
        const int n_scales = det->param.n_scales_per_octave;

        // To make histograms cyclic, we store histograms from hist[1] to
        // hist[N], add two bins for the last and first elements in a cyclic
        // fashion
        float hist[NX_SIFT_N_ORI_BINS+2];
        float sigma = sigma_c * pow(2.0, i / n_scales);
        float hist_peak = nx_sift_compute_ori_hist(&hist[0],
                                                   det->gx, det->gy,
                                                   x, y, sigma);

        for (int b = 1; b <= NX_SIFT_N_ORI_BINS; ++b) {
                if (hist[b] > hist[b-1] && hist[b] > hist[b+1]
                    && hist[b] > hist_peak * 0.8f) {

                        // interpolate peak position for y = cx^2 + dx + e
                        float d = 0.5f * (hist[b+1] - hist[b-1]);
                        float two_c = hist[b-1] + hist[b+1] - 2.0f * hist[b];
                        float peak_offset = -d / two_c; // \in (-1.0, 1.0)
                        float ori = NX_PI * (2.0f * (b - 0.5f + peak_offset)
                                             / NX_SIFT_N_ORI_BINS - 1.0f);

                        // Create keypoint
                        int key_id = nx_sift_key_store_append(store);
                        struct NXKeypoint *key = store->keys + key_id;
                        key->x = (int)(x + 0.5f);
                        key->y = (int)(y + 0.5f);
                        key->xs = x;
                        key->ys = y;
                        key->level = octave;
                        key->scale = pow(2.0, octave);
                        key->sigma = sigma;
                        key->score = dog_val;
                        key->ori = ori;
                        key->id = key_id;

                        uchar *desc = store->desc + key_id * NX_SIFT_DESC_DIM;
                        nx_sift_compute_descriptor(det, key, desc);

                        /* NX_LOG(NX_LOG_TAG, "Key (%6.3f, %6.3f), sigma = %6.3f, ori = %6.3f", */
                               /* key->xs, key->ys, key->sigma, key->ori); */
                }
        }
}

void nx_sift_interp_peak_location(struct NXSIFTDetector *det,
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

                double A[9] = { dog_row[x-1] - 2.0 * dog_row[x] + dog_row[x+1], // Hxx
                                ((dog_rowp[x+1] - dog_rowp[x-1]) -
                                 (dog_rowm[x+1] - dog_rowm[x-1])) / 4.0, // Hxy
                                ((dogp_row[x+1] - dogp_row[x-1]) -
                                 (dogm_row[x+1] - dogm_row[x-1])) / 4.0, // Hxs
                                0.0,
                                dog_rowm[x]  - 2.0 * dog_row[x] + dog_rowp[x],  // Hyy
                                ((dogp_rowp[x] - dogp_rowm[x]) -
                                 (dogm_rowp[x] - dogm_rowm[x])) / 4.0,   // Hys
                                0.0,
                                0.0,
                                dogm_row[x]  - 2.0 * dog_row[x] + dogp_row[x] };  // Hss

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

        nx_sift_compute_keys(det, store, octave, sigma_c, dval,
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

void nx_sift_process_dog(struct NXSIFTDetector *det,
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

void nx_sift_detector_process_octave(struct NXSIFTDetector *det,
                                     struct NXSIFTKeyStore *store,
                                     int octave,
                                     float sigma_c)
{
        const int n_scales = det->param.n_scales_per_octave;
        for (int i = 1; i < n_scales + 1; ++i) {
                nx_image_deriv_x(det->gx, det->levels[i-1]);
                nx_image_deriv_y(det->gy, det->levels[i-1]);
                nx_sift_process_dog(det, store, octave, sigma_c, i);
        }
}

void nx_sift_detector_prepare_octave(struct NXSIFTDetector *det,
                                     float sigma_c)
{
        const int n_scales = det->param.n_scales_per_octave;
        float scale_multiplier = pow(2.0, 1.0 / n_scales);
        for (int i = 1; i < n_scales + 3; ++i) {
                nx_image_copy(det->levels[i], det->levels[i-1]);
                float sigma_g = kernel_sigma(sigma_c, scale_multiplier * sigma_c);
                nx_image_smooth(det->levels[i], det->levels[i],
                                sigma_g, sigma_g,
                                det->param.kernel_truncation_factor, NULL);
                sigma_c *= scale_multiplier;
        }

        for (int i = 0; i < n_scales + 2; ++i) {
                nx_image_subtract(det->dogs[i],
                                  det->levels[i], det->levels[i+1]);
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
        NX_IMAGE_ASSERT_GRAYSCALE(image);
        NX_ASSERT_PTR(keys);
        NX_ASSERT_PTR(desc);

        const int MIN_DIM = 2 * det->param.border_distance + 2;

        float sigma_c = 0.5f;
        int octave = 0;

        struct NXImage *dbl_img = nx_image_alloc();
        if (det->param.double_image) {
                nx_image_upsample(dbl_img, image);
                image = dbl_img;

                sigma_c *= 2.0f;
                octave--;
        }

        nx_image_resize(det->levels[0], image->width, image->height,
                        NX_IMAGE_STRIDE_DEFAULT,
                        NX_IMAGE_GRAYSCALE, NX_IMAGE_FLOAT32);
        nx_image_convert_dtype(det->levels[0], image);

        nx_image_free(dbl_img);
        image = NULL;
        dbl_img = NULL;

        if (det->param.sigma0 > sigma_c) {
                float sigma_g = kernel_sigma(sigma_c, det->param.sigma0);
                nx_image_smooth(det->levels[0], det->levels[0],
                                sigma_g, sigma_g,
                                det->param.kernel_truncation_factor, NULL);
                sigma_c = det->param.sigma0;
        }

        struct NXSIFTKeyStore store;
        store.n = 0;
        store.cap = *max_n_keys;
        store.keys = *keys;
        store.desc = *desc;

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

int nx_sift_detector_compute_with_cache(struct NXSIFTDetector *detector,
                                        struct NXImage *image,
                                        int *max_n_keys,
                                        struct NXKeypoint **keys,
                                        uchar **desc,
                                        const char *cache_dir)
{
        NX_ASSERT_PTR(detector);
        NX_ASSERT_PTR(image);
        NX_IMAGE_ASSERT_GRAYSCALE(image);
        NX_ASSERT_PTR(keys);
        NX_ASSERT_PTR(desc);

        if (!cache_dir)
                cache_dir = NX_DEFAULT_CACHE_DIRECTORY;

        int n_rows = image->height;
        int n_msgs = n_rows + 1;
        uint8_t **msgs = NX_NEW(n_msgs, uint8_t *);
        size_t *lmsg = NX_NEW_Z(n_msgs);
        for (int i = 0; i < n_rows; ++i) {
                if (image->dtype == NX_IMAGE_UCHAR) {
                        msgs[i] = (uint8_t *)(image->data.uc + i * image->row_stride);
                        lmsg[i] = image->width * image->n_channels * sizeof(uchar);
                } else if (image->dtype == NX_IMAGE_UCHAR) {
                        msgs[i] = (uint8_t *)(image->data.f32 + i * image->row_stride);
                        lmsg[i] = image->width * image->n_channels * sizeof(float);
                }
        }
        msgs[n_msgs - 1] = (uint8_t *)(&detector->param);
        lmsg[n_msgs - 1] = sizeof(detector->param);
        uint8_t hash[32];
        nx_sha256_multi(&hash[0], n_msgs, (const uint8_t *const *)msgs, lmsg);

        int n_keys = 0;
        char *hash_str = nx_hash_to_str(32, &hash[0]);
        char *cachefilepath = nx_fstr("%s/%s.sift", cache_dir, hash_str);
        if (nx_check_file(cachefilepath)) {
                // read from cache file
                FILE *fin = nx_xfopen(cachefilepath, "rb");
                nx_xfread(&n_keys, sizeof(n_keys), 1, fin);

                if (*max_n_keys < n_keys) {
                        *keys = (struct NXKeypoint *)nx_xrealloc(*keys, n_keys * sizeof(struct NXKeypoint));
                        *desc = (uchar *)nx_xrealloc(*desc, n_keys * NX_SIFT_DESC_DIM * sizeof(uchar));
                        *max_n_keys = n_keys;
                }
                nx_xfread(*keys, sizeof(struct NXKeypoint), n_keys, fin);
                nx_xfread(*desc, sizeof(uchar), n_keys * NX_SIFT_DESC_DIM, fin);
                nx_fclose(fin, cachefilepath);
                NX_LOG(NX_LOG_TAG, "Read %d SIFT keypoints and descriptors from cache file %s",
                       n_keys, cachefilepath);
        } else {
                n_keys = nx_sift_detector_compute(detector, image,
                                                  max_n_keys, keys, desc);

                // cache keys and descriptors
                nx_ensure_dir(cache_dir);
                FILE *fout = nx_xfopen(cachefilepath, "wb");
                nx_xfwrite(&n_keys, sizeof(n_keys), 1, fout);
                nx_xfwrite(*keys, sizeof(struct NXKeypoint), n_keys, fout);
                nx_xfwrite(*desc, sizeof(uchar), n_keys * NX_SIFT_DESC_DIM, fout);
                nx_fclose(fout, cachefilepath);
                NX_LOG(NX_LOG_TAG, "Cached %d SIFT keypoints and descriptors to %s",
                       n_keys, cachefilepath);
        }

        nx_free(hash_str);
        nx_free(cachefilepath);

        return n_keys;
}

int nx_sift_match_brute_force(int n,  const struct NXKeypoint *keys,
                              const uchar *desc,
                              int np, const struct NXKeypoint *keyps,
                              const uchar *descp,
                              struct NXPointMatch2D *corr,
                              float dist_ratio_thr)
{
        NX_ASSERT(n >= 0);
        NX_ASSERT_PTR(keys);
        NX_ASSERT_PTR(desc);
        NX_ASSERT(np >= 0);
        NX_ASSERT_PTR(keyps);
        NX_ASSERT_PTR(descp);
        NX_ASSERT_PTR(corr);

        const float SIFT_LOCALIZATION_STD_DEV = 0.3f;

        NXBool check_dist_ratio = dist_ratio_thr > 0.0f && dist_ratio_thr < 1.0f;

        if (check_dist_ratio) {
                const float DIST_THR_SQ = dist_ratio_thr * dist_ratio_thr;
                int n_matches = 0;

                for (int i = 0; i < n; ++i) {
                        int nn_ids[2] = {-1, -1};
                        int nn_dist_sq[2] = { INT_MAX, INT_MAX };
                        const struct NXKeypoint *k = keys + i;
                        const uchar *desc_i = desc + i*NX_SIFT_DESC_DIM;

                        for (int j = 0; j < np; ++j) {
                                const uchar *desc_j = descp + j*NX_SIFT_DESC_DIM;

                                int d_sq = nx_ucvec_dist_sq(NX_SIFT_DESC_DIM,
                                                            desc_i, desc_j);
                                if (d_sq < nn_dist_sq[0]) {
                                        nn_dist_sq[1] = nn_dist_sq[0];
                                        nn_dist_sq[0] = d_sq;

                                        nn_ids[1] = nn_ids[0];
                                        nn_ids[0] = j;
                                } else if (d_sq < nn_dist_sq[1]) {
                                        nn_dist_sq[1] = d_sq;
                                        nn_ids[1] = j;
                                }
                        }

                        if (nn_dist_sq[0] < DIST_THR_SQ * nn_dist_sq[1]) {
                                const struct NXKeypoint *kp = keyps + nn_ids[0];
                                struct NXPointMatch2D *pm = corr + n_matches;
                                *pm = nx_point_match_2d_from_keypoints(k, kp,
                                                                       SIFT_LOCALIZATION_STD_DEV,
                                                                       nn_dist_sq[0],
                                                                       NX_FALSE);
                                ++n_matches;
                        }
                }

                return n_matches;
        } else {
                for (int i = 0; i < n; ++i) {
                        int nn_id = -1;
                        int nn_dist_sq = INT_MAX;
                        const struct NXKeypoint *k = keys + i;
                        const uchar *desc_i = desc + i*NX_SIFT_DESC_DIM;

                        for (int j = 0; j < np; ++j) {
                                const uchar *desc_j = descp + j*NX_SIFT_DESC_DIM;

                                int d_sq = nx_ucvec_dist_sq(NX_SIFT_DESC_DIM,
                                                            desc_i, desc_j);
                                if (d_sq < nn_dist_sq) {
                                        nn_dist_sq = d_sq;
                                        nn_id = j;
                                }
                        }

                        const struct NXKeypoint *kp = keyps + nn_id;
                        struct NXPointMatch2D *pm = corr + i;
                        *pm = nx_point_match_2d_from_keypoints(k, kp,
                                                               SIFT_LOCALIZATION_STD_DEV,
                                                               nn_dist_sq,
                                                               NX_FALSE);
                }

                return n;
        }
}

int nx_sift_match_brute_force_with_cache(int n,  const struct NXKeypoint *keys,
                                         const uchar *desc,
                                         int np, const struct NXKeypoint *keyps,
                                         const uchar *descp,
                                         struct NXPointMatch2D *corr,
                                         float dist_ratio_thr,
                                         const char *cache_dir)
{
        NX_ASSERT(n >= 0);
        NX_ASSERT_PTR(keys);
        NX_ASSERT_PTR(desc);
        NX_ASSERT(np >= 0);
        NX_ASSERT_PTR(keyps);
        NX_ASSERT_PTR(descp);
        NX_ASSERT_PTR(corr);

        if (!cache_dir)
                cache_dir = NX_DEFAULT_CACHE_DIRECTORY;

        int n_msgs = 5;
        uint8_t **msgs = NX_NEW(n_msgs, uint8_t *);
        size_t *lmsg = NX_NEW_Z(n_msgs);
        msgs[0] = (uint8_t *)(keys);
        lmsg[0] = n * sizeof(struct NXKeypoint);
        msgs[1] = (uint8_t *)(desc);
        lmsg[1] = n * NX_SIFT_DESC_DIM * sizeof(uchar);

        msgs[2] = (uint8_t *)(keyps);
        lmsg[2] = np * sizeof(struct NXKeypoint);
        msgs[3] = (uint8_t *)(descp);
        lmsg[3] = np * NX_SIFT_DESC_DIM * sizeof(uchar);

        msgs[4] = (uint8_t *)(&dist_ratio_thr);
        lmsg[4] = sizeof(dist_ratio_thr);

        uint8_t hash[32];
        nx_sha256_multi(&hash[0], n_msgs, (const uint8_t *const *)msgs, lmsg);

        int n_corr = 0;
        char *hash_str = nx_hash_to_str(32, &hash[0]);
        char *cachefilepath = nx_fstr("%s/%s.sift_matches", cache_dir, hash_str);
        if (nx_check_file(cachefilepath)) {
                // read from cache file
                FILE *fin = nx_xfopen(cachefilepath, "rb");
                nx_xfread(&n_corr, sizeof(n_corr), 1, fin);
                nx_xfread(corr, sizeof(struct NXPointMatch2D), n_corr, fin);
                nx_fclose(fin, cachefilepath);
                NX_LOG(NX_LOG_TAG, "Read %d SIFT matches from cache file %s",
                       n_corr, cachefilepath);
        } else {
                n_corr = nx_sift_match_brute_force(n, keys, desc,
                                                   np, keyps, descp,
                                                   corr, dist_ratio_thr);
                // cache keys and descriptors
                nx_ensure_dir(cache_dir);
                FILE *fout = nx_xfopen(cachefilepath, "wb");
                nx_xfwrite(&n_corr, sizeof(n_corr), 1, fout);
                nx_xfwrite(corr, sizeof(struct NXPointMatch2D), n_corr, fout);
                nx_fclose(fout, cachefilepath);
                NX_LOG(NX_LOG_TAG, "Cached %d SIFT matches to %s",
                       n_corr, cachefilepath);
        }

        nx_free(hash_str);
        nx_free(cachefilepath);

        return n_corr;
}
