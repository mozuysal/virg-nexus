/**
 * @file nx_homography.h
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
 * VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_HOMOGRAPHY_H
#define VIRG_NEXUS_NX_HOMOGRAPHY_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_keypoint.h"
#include "virg/nexus/nx_point_match_2d.h"

__NX_BEGIN_DECL

static inline double nx_homography_transfer_fwd(const double *h, float *xp,
                                                const float *x)
{
        double s = h[2]*x[0]+h[5]*x[1]+h[8];
        double s_inv = 1.0 / s;

        xp[0] = (h[0]*x[0]+h[3]*x[1]+h[6]) * s_inv;
        xp[1] = (h[1]*x[0]+h[4]*x[1]+h[7]) * s_inv;

        return s;
}

// x[TL, TR, BR, BL] -> unit square[(0,0) - (1,0) - (1,1) - (0,1)]
double nx_homography_estimate_unit(double *h, const double *x);
NXBool nx_homography_check        (const double *h, double max_abs_cos);
int    nx_homography_mark_inliers (const double *h, int n_corr,
                                   struct NXPointMatch2D *corr_list,
                                   double inlier_tolerance);

double nx_homography_estimate_4pt        (double *h, const int corr_ids[4],
                                          const struct NXPointMatch2D *corr_list);
double nx_homography_estimate_dlt        (double *h, int n_corr,
                                          const struct NXPointMatch2D *corr_list);
double nx_homography_estimate_dlt_inliers(double *h, int n_corr,
                                          const struct NXPointMatch2D *corr_list);

int nx_homography_estimate_ransac     (double *h, int n_corr,
                                       struct NXPointMatch2D *corr_list,
                                       double inlier_tolerance, int max_n_iter);
int nx_homography_estimate_norm_ransac(double *h, int n_corr,
                                       struct NXPointMatch2D *corr_list,
                                       double inlier_tolerance, int max_n_iter);

double nx_homography_transfer_error_fwd(const double *h, int n_corr,
                                        const struct NXPointMatch2D *corr_list);
double nx_homography_transfer_error_bwd(const double *h, int n_corr,
                                        const struct NXPointMatch2D *corr_list);
double nx_homography_transfer_error_sym(const double *h, int n_corr,
                                        const struct NXPointMatch2D *corr_list);

__NX_END_DECL

#endif
