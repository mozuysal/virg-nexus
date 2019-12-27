/**
 * @file vg_stereo_matcher_main.cc
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
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_vec.h"
#include "virg/nexus/nx_mat234.h"
#include "virg/nexus/nx_mat.h"
#include "virg/nexus/nx_homography.h"
#include "virg/nexus/nx_epipolar.h"
#include "virg/nexus/nx_pinhole.h"

#include "virg/nexus/vg_options.hpp"
#include "virg/nexus/vg_image.hpp"
#include "virg/nexus/vg_image_pyr.hpp"
#include "virg/nexus/vg_harris_detector.hpp"
#include "virg/nexus/vg_brief_extractor.hpp"
#include "virg/nexus/vg_point_correspondence_2d.hpp"
#include "virg/nexus/vg_homography.hpp"
#include "virg/nexus/vg_fundamental_matrix.hpp"
#include "virg/nexus/vg_image_annotator.hpp"

using namespace std;

using virg::nexus::VGOptions;
using virg::nexus::VGColor;
using virg::nexus::VGImage;
using virg::nexus::VGImagePyr;
using virg::nexus::VGHarrisDetector;
using virg::nexus::VGBriefExtractor;
using virg::nexus::VGDescriptorMap;
using virg::nexus::VGPointCorrespondence2D;
using virg::nexus::VGHomography;
using virg::nexus::VGFundamentalMatrix;
using virg::nexus::VGImageAnnotator;

static const char* LOG_TAG = "STEREO";
static const int MAX_N_KEYS = 2000;
static const int N_PYR_LEVELS = 5;
static const float SIGMA0 = 1.2f;
static const int   N_OCTETS = 32;
static const float KEY_SIGMA0 = 0.5f;
static const float MATCH_COST_UPPER_BOUND = 70.0f;
static const float INLIER_TOL_H = 3.0f;
static const float INLIER_TOL_F = 2.0f;
static const int MAX_RANSAC_ITER = 10000;

struct Frame {
        string label;
        VGImagePyr pyr;
        std::vector<struct NXKeypoint> keys;
        VGDescriptorMap dmap;
};

struct StereoFrame {
        Frame left;
        Frame right;
        VGPointCorrespondence2D hcorr;
        VGPointCorrespondence2D fcorr;
        VGHomography H;
        VGFundamentalMatrix F;
};

static void build_frame(Frame& frm, string label, VGImage& img, bool is_verbose) {
        frm.pyr = VGImagePyr::build_fast_from(img, N_PYR_LEVELS, SIGMA0);

        VGHarrisDetector detector;
        if (is_verbose)
                NX_LOG(LOG_TAG, "Processing image %s", label.c_str());
        int n_keys;
        for (int k = 0; k < 20; ++k) {
                n_keys = detector.detect_pyr(frm.pyr, frm.keys,
                                             N_PYR_LEVELS-2, MAX_N_KEYS, true);
                if (n_keys < MAX_N_KEYS && n_keys >= 0.95*MAX_N_KEYS)
                        break;
        }
        if (is_verbose) {
                NX_LOG(LOG_TAG, "  Detected %d keypoints", n_keys);

                string filename = "/tmp/" + label + "_keys.png";
                NX_LOG(LOG_TAG, "Saving keypoint image to %s", filename.c_str());
                VGImageAnnotator ia(img);
                ia.draw_keypoints(static_cast<int>(frm.keys.size()),
                                  &frm.keys[0], false);
                ia.get_canvas().xsave(filename);
        }

        VGBriefExtractor be(N_OCTETS);
        frm.dmap.set_n_octets(N_OCTETS);
        be.compute_pyr(frm.pyr, static_cast<int>(frm.keys.size()),
                       frm.keys.data(), frm.dmap);
}

static void match_frames(StereoFrame& stereo, bool is_verbose)
{
        stereo.hcorr.clear();
        int n_left = stereo.left.dmap.size();
        for (int i = 0; i < n_left; ++i) {
                const uchar* dleft = stereo.left.dmap.get_descriptor(i);
                uint64_t id_left = stereo.left.dmap.get_id(i);
                const struct NXKeypoint* key_left = &stereo.left.keys[id_left];

                VGDescriptorMap::SearchResult r = stereo.right.dmap.search_nn(dleft);
                uint64_t id_right = r.id;
                const struct NXKeypoint* key_right = &stereo.right.keys[id_right];

                if (r.match_cost < MATCH_COST_UPPER_BOUND) {
                        stereo.hcorr.add_keypoint_match(key_left, key_right,
                                                       KEY_SIGMA0, r.match_cost);
                }
        }
        stereo.fcorr = stereo.hcorr.clone();

        if (is_verbose) {
                NX_LOG(LOG_TAG, "Established %d putative correspondences", stereo.hcorr.size());

                string filename = "/tmp/h_matches.png";
                NX_LOG(LOG_TAG, "Saving match image to %s", filename.c_str());
                VGImageAnnotator ia = VGImageAnnotator::create_match_image(stereo.left.pyr[0],
                                                                           stereo.right.pyr[0],
                                                                           stereo.hcorr, false);
                ia.get_canvas().xsave(filename);
        }

}

static int estimate_initial_homography(StereoFrame& sf, bool is_verbose)
{
        sf.hcorr.normalize();
        int n_inliers = sf.H.estimate_ransac(sf.hcorr,
                                             INLIER_TOL_H/sf.hcorr.stats()->dp,
                                             MAX_RANSAC_ITER);
        sf.hcorr.denormalize_homography(sf.H.data());
        sf.hcorr.denormalize();

        double te_fwd = sf.H.transfer_error_fwd(sf.hcorr);
        double te_sym = sf.H.transfer_error_sym(sf.hcorr);

        if (is_verbose) {
                NX_LOG(LOG_TAG, "RANSAC for homography terminated with %d inliers.", n_inliers);
                NX_LOG(LOG_TAG, "   transfer error (fwd/sym) = %.2f / %.2f",
                       te_fwd, te_sym);

                string filename = "/tmp/h_matches_inliers.png";
                NX_LOG(LOG_TAG, "Saving matched inliers as image to %s", filename.c_str());
                VGImageAnnotator ia = VGImageAnnotator::create_match_image(sf.left.pyr[0],
                                                                           sf.right.pyr[0],
                                                                           sf.hcorr, true);
                ia.get_canvas().xsave(filename);
        }

        return n_inliers;
}


static int search_keypoint_around(const double* x, const vector<NXKeypoint>& keys)
{
        int id = -1;
        double d_best = INLIER_TOL_H;
        for (int i = 0; i < static_cast<int>(keys.size()); i++) {
                double kx[2] = { nx_keypoint_xs0(&keys[i]),
                                 nx_keypoint_ys0(&keys[i]) };
                double dx = kx[0] - x[0];
                double dy = kx[1] - x[1];
                double d = sqrt(dx*dx + dy*dy);
                if (d < d_best) {
                        d_best = d;
                        id = i;
                }
        }

        return id;
}

static void match_frames_guided_by_homography(StereoFrame& stereo,
                                              bool is_verbose)
{
        int n_left = stereo.left.keys.size();
        for (int i = 0; i < n_left; ++i) {
                const struct NXKeypoint* key_left = &stereo.left.keys[i];
                double x[2] = { nx_keypoint_xs0(key_left),
                                nx_keypoint_ys0(key_left) };
                double xp[2];
                stereo.H.transfer_fwd(&xp[0], &x[0]);
                int id_right = search_keypoint_around(&xp[0], stereo.right.keys);
                if (id_right >= 0) {
                        const struct NXKeypoint* key_right = &stereo.right.keys[id_right];
                        stereo.hcorr.add_keypoint_match(key_left, key_right,
                                                       KEY_SIGMA0, 0.0f,
                                                       true);
                }
        }

        if (is_verbose) {
                NX_LOG(LOG_TAG, "Established %d H guided correspondences",
                       stereo.hcorr.size());

                string filename = "/tmp/h_matches_guided.png";
                NX_LOG(LOG_TAG, "Saving guided match image to %s", filename.c_str());
                VGImageAnnotator ia = VGImageAnnotator::create_match_image(stereo.left.pyr[0],
                                                                           stereo.right.pyr[0],
                                                                           stereo.hcorr, true);
                ia.get_canvas().xsave(filename);
        }

}

static int estimate_guided_homography(StereoFrame& sf, bool is_verbose)
{
        sf.hcorr.normalize();
        int n_inliers = sf.H.estimate_from_inliers(sf.hcorr,
                                                   INLIER_TOL_H/sf.hcorr.stats()->dp);
        sf.hcorr.denormalize_homography(sf.H.data());
        sf.hcorr.denormalize();

        double te_fwd = sf.H.transfer_error_fwd(sf.hcorr);
        double te_sym = sf.H.transfer_error_sym(sf.hcorr);

        if (is_verbose) {
                NX_LOG(LOG_TAG, "Homography from guided matches returned %d inliers.", n_inliers);
                NX_LOG(LOG_TAG, "   transfer error (fwd/sym) = %.2f / %.2f",
                       te_fwd, te_sym);

                string filename = "/tmp/h_matches_guided_inliers.png";
                NX_LOG(LOG_TAG, "Saving matched inliers after guided matching as image to %s", filename.c_str());
                VGImageAnnotator ia = VGImageAnnotator::create_match_image(sf.left.pyr[0],
                                                                           sf.right.pyr[0],
                                                                           sf.hcorr, true);
                ia.get_canvas().xsave(filename);
        }

        return n_inliers;
}

static double
homography_transfer_error_and_jacobian(int n, struct NXPointMatch2D *pm,
                                       const double *h,
                                       double *e, double *Jt)
{
        double E = 0;
        int n_inliers = 0;
        for (int i = 0; i < n; ++i) {
                if (!pm[i].is_inlier)
                        continue;
                n_inliers++;
                double xi = pm[i].x[0];
                double yi = pm[i].x[1];
                double Hx[3];
                Hx[0] = h[0] * xi + h[3] * yi + h[6];
                Hx[1] = h[1] * xi + h[4] * yi + h[7];
                Hx[2] = h[2] * xi + h[5] * yi + h[8];
                double Ex = pm[i].xp[0] - Hx[0] / Hx[2];
                double Ey = pm[i].xp[1] - Hx[1] / Hx[2];
                E += (Ex * Ex + Ey * Ey);

                if (e) {
                        e[0] = Ex;
                        e[1] = Ey;
                        e += 2;
                }

                if (Jt) {
                        Jt[0] = xi / Hx[2];
                        Jt[1] = 0.0;
                        Jt[2] = - xi * Hx[0] / (Hx[2] * Hx[2]);
                        Jt[3] = yi / Hx[2];
                        Jt[4] = 0.0;
                        Jt[5] = - yi * Hx[0] / (Hx[2] * Hx[2]);
                        Jt[6] = 1.0 / Hx[2];
                        Jt[7] = 0.0;
                        Jt[8] = - Hx[0] / (Hx[2] * Hx[2]);

                        Jt[9]  = 0.0;
                        Jt[10] = xi / Hx[2];
                        Jt[11] = - xi * Hx[1] / (Hx[2] * Hx[2]);
                        Jt[12] = 0.0;
                        Jt[13] = yi / Hx[2];
                        Jt[14] = - yi * Hx[1] / (Hx[2] * Hx[2]);;
                        Jt[15] = 0.0;
                        Jt[16] = 1.0 / Hx[2];
                        Jt[17] = - Hx[1] / (Hx[2] * Hx[2]);
                        Jt += 18;
                }
        }

        E /= n_inliers;

        return E;
}

static void lev_mar_compute_A(int n, double *A, double lambda,
                              int m, const double *Jt)
{
        for (int c = 0; c < n; ++c) {
                for (int r = 0; r < n; ++r) {
                        A[c * n + r] = 0.0;
                        for (int k = 0; k < m; ++k) {
                                A[c * n + r] = Jt[k * n + r] * Jt[c * m + k];
                        }
                }
        }

        for (int i = 0; i < n; ++i)
                A[i * n + i] += lambda * A[i * n + i];
 }

static int estimate_homography_nonlinear(StereoFrame& sf, bool is_verbose)
{
        double *h = sf.H.data();
        for (int i = 0; i < 9; ++i)
                h[i] /= h[8];
        printf("h = [%16.10f%16.10f%16.10f%16.10f%16.10f%16.10f%16.10f%16.10f%16.10f]\n",
               h[0], h[1], h[2], h[3], h[4], h[5], h[6], h[7], h[8]);

        int n = sf.hcorr.size();
        struct NXPointMatch2D *pm = sf.hcorr.matches();

        int N = nx_point_match_2d_count_inliers(n, pm);
        double *e = NX_NEW_D(2*N);
        double *Jt = NX_NEW_D(2*N*9);
        double E = homography_transfer_error_and_jacobian(n, pm, h, e, Jt);

        double Jte[9];
        nx_dmat_mul(9, 1, 2*N, Jt, 9, e, 2*N, Jte, 9);
        double g_mag = 4.0 * nx_dvec_norm(9, &Jte[0]);
        NX_LOG(LOG_TAG, "Nonlinear Opt. for Homography E = %.6f, |g| = %.6f", E, g_mag);

        double lambda = 1.0;
        double A[9*9];
        double hnew[9];
        for (int i = 0; i < 10; ++i) {
                NXBool done = NX_FALSE;
                double Enew;
                while (!done) {
                        lev_mar_compute_A(9, &A[0], lambda, 2*N, Jt);
                        int ipiv[9];
                        int info = nx_dsym_solver(NX_SYMMETRIC_LOWER, 9, 1, &A[0], 9,
                                                  &Jte[0], 9, &ipiv[0]);
                        if (info != 0) {
                                NX_FATAL("!", "info = %d", info);
                        }

                        for (int j = 0; j < 9; ++j)
                                hnew[j] = h[j] + Jte[j];

                        Enew = homography_transfer_error_and_jacobian(n, pm, hnew,
                                                                      NULL, NULL);
                        if (Enew < E) {
                                done = NX_TRUE;
                                lambda /= 1.2;
                        } else {
                                lambda *= 2.0;
                        }
                        NX_LOG(NX_LOG_TAG, "Enew = %.2e, lambda = %.2e", Enew, lambda);
                        static int tries = 0;
                        tries ++;
                        if (tries > 40)
                                NX_FATAL("NX!", "Quitting");
                }
                for (int j = 0; j < 9; ++j)
                        h[j] += Jte[j];
                E = homography_transfer_error_and_jacobian(n, pm, h, e, Jt);
                nx_dmat_mul(9, 1, 2*N, Jt, 9, e, 2*N, Jte, 9);
                g_mag = 4.0 * nx_dvec_norm(9, &Jte[0]);

                if (i % 1 == 1) {
                        NX_LOG(LOG_TAG, "   Iter %10d: E = %.6f, |g| = %.6f",
                               i, E, g_mag);
                }

        }

        if (is_verbose) {
                E = homography_transfer_error_and_jacobian(n, pm, h, e, Jt);
                nx_dmat_mul(9, 1, 2*N, Jt, 9, e, 2*N, Jte, 9);
                g_mag = 4.0 * nx_dvec_norm(9, &Jte[0]);
                NX_LOG(LOG_TAG, "   Iter END: E = %.6f, |g| = %.6f",
                       E, g_mag);
                double te_fwd = nx_homography_transfer_error_fwd(h, n, pm);
                double te_sym = nx_homography_transfer_error_sym(h, n, pm);
                NX_LOG(LOG_TAG, "   Transfer error (fwd/sym) = %.2f / %.2f",
                       te_fwd, te_sym);
                printf("h = [%16.10f%16.10f%16.10f%16.10f%16.10f%16.10f%16.10f%16.10f%16.10f]\n",
                       h[0], h[1], h[2], h[3], h[4], h[5], h[6], h[7], h[8]);
        }

        int n_inliers = nx_homography_mark_inliers(h, n, pm, INLIER_TOL_H);
        if (is_verbose) {
                double te_fwd = nx_homography_transfer_error_fwd(h, n, pm);
                double te_sym = nx_homography_transfer_error_sym(h, n, pm);

                NX_LOG(LOG_TAG, "Homography from nonlinear opt. returned %d inliers.", n_inliers);
                NX_LOG(LOG_TAG, "   transfer error (fwd/sym) = %.2f / %.2f",
                       te_fwd, te_sym);
        }

        nx_free(e);
        nx_free(Jt);

        return n_inliers;
}

static int estimate_initial_fundamental_matrix(StereoFrame& sf, bool is_verbose)
{
        sf.fcorr.normalize();
        int n_inliers = sf.F.estimate_ransac(sf.fcorr,
                                             INLIER_TOL_F/sf.fcorr.stats()->dp,
                                             MAX_RANSAC_ITER);
        sf.fcorr.denormalize_fundamental(sf.F.data());
        sf.fcorr.denormalize();

        // double te_fwd = sf.H.transfer_error_fwd(sf.hcorr);
        // double te_sym = sf.H.transfer_error_sym(sf.hcorr);

        if (is_verbose) {
                NX_LOG(LOG_TAG, "RANSAC for fundamental matrix terminated with %d inliers.", n_inliers);
                // NX_LOG(LOG_TAG, "   transfer error (fwd/sym) = %.2f / %.2f",
                       // te_fwd, te_sym);

                string filename = "/tmp/f_matches_inliers.png";
                NX_LOG(LOG_TAG, "Saving matched inliers as image to %s", filename.c_str());
                VGImageAnnotator ia = VGImageAnnotator::create_match_image(sf.left.pyr[0],
                                                                           sf.right.pyr[0],
                                                                           sf.fcorr, true);
                ia.get_canvas().xsave(filename);
        }

        return n_inliers;
}

int main(int argc, char** argv)
{
        VGOptions opt;
        opt.add_string("-l", "image file for the left image.", VGOptions::REQUIRED, "");
        opt.add_string("-r", "image file for the right image.", VGOptions::REQUIRED, "");
        opt.add_bool("-v|--verbose", "display more information.", VGOptions::NOT_REQUIRED, false);
        opt.add_help();
        opt.set_from_args(argc, argv);

        string left_image  = opt.get_string("-l");
        string right_image = opt.get_string("-r");
        bool is_verbose = opt.get_bool("-v");

        VGImage images[2];
        images[0].xload(left_image, VGImage::LOAD_GRAYSCALE);
        images[1].xload(right_image, VGImage::LOAD_GRAYSCALE);

        if (is_verbose) {
                NX_LOG(LOG_TAG, "Loaded %dx%d image from %s.", images[0].width(), images[0].height(), left_image.c_str());
                NX_LOG(LOG_TAG, "Loaded %dx%d image from %s.", images[1].width(), images[1].height(), right_image.c_str());
        }

        StereoFrame sf;
        build_frame(sf.left,  "left",  images[0], is_verbose);
        build_frame(sf.right, "right", images[1], is_verbose);
        match_frames(sf, is_verbose);

        int n_inliers_h = estimate_initial_homography(sf, is_verbose);
        if (n_inliers_h >= 15) {
                match_frames_guided_by_homography(sf, is_verbose);
                estimate_guided_homography(sf, is_verbose);
                n_inliers_h = estimate_homography_nonlinear(sf, is_verbose);
        }


        // int n_inliers_f = estimate_initial_fundamental_matrix(sf, is_verbose);
        // if (n_inliers_f >= 30) {
        //         double E[9];
        //         // nx_essential_from_fundamental(&E[0], sf.F.data(),
        //                                       // &sf.Kl[0], &sf.Kr[0]);

        // }

        return EXIT_SUCCESS;
}

/// Local Variables:
/// mode: c++
/// compile-command: "make vg-stereo-matcher"
/// End:
