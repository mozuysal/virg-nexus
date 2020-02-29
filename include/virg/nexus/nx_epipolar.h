/**
 * @file nx_epipolar.h
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
 * You should have received a copy of the GNU General Public License along with
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_EPIPOLAR_H
#define VIRG_NEXUS_NX_EPIPOLAR_H

#include <math.h>

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_point_match_2d.h"

__NX_BEGIN_DECL

static inline double
nx_fundamental_epipolar_distance_fwd(const double *F,
                                     const struct NXPointMatch2D *pm)
{
        double x[2] =  { pm->x[0],  pm->x[1] };
        double xp[2] = { pm->xp[0], pm->xp[1] };

        double l[3];
        l[0] = F[0]*x[0] + F[3]*x[1] + F[6];
        l[1] = F[1]*x[0] + F[4]*x[1] + F[7];
        l[2] = F[2]*x[0] + F[5]*x[1] + F[8];

        // point - line distance in 2D
        return fabs((l[0]*xp[0] + l[1]*xp[1] + l[2])
                    / sqrt(l[0]*l[0] + l[1]*l[1]));
}

int nx_fundamental_mark_inliers(const double *F, int n_corr,
                                struct NXPointMatch2D *corr_list,
                                double inlier_tolerance);


double nx_fundamental_estimate_8pt    (double *F, int corr_ids[8],
                                       const struct NXPointMatch2D *corr_list);
double nx_fundamental_estimate        (double *F, int n_corr,
                                       const struct NXPointMatch2D *corr_list);
double nx_fundamental_estimate_inliers(double *F, int n_corr,
                                       const struct NXPointMatch2D *corr_list);

int nx_fundamental_estimate_ransac     (double *F, int n_corr,
                                        struct NXPointMatch2D *corr_list,
                                        double inlier_tolerance, int max_n_iter);
int nx_fundamental_estimate_norm_ransac(double *F, int n_corr,
                                        struct NXPointMatch2D *corr_list,
                                        double inlier_tolerance, int max_n_iter);

void nx_essential_from_fundamental(double *E, const double *F,
                                   const double *K, const double *Kp);

void nx_essential_from_Rt(double *E, const double *R, const double *t);

double nx_essential_decompose_to_Rt(const double *E, double **R, double **t);

int nx_epipolar_pick_best_Rt(const double * const *R, const double * const *t,
                             int n, const struct NXPointMatch2D *corr_list,
                             int *z_sign_sum);

__NX_END_DECL

#endif
