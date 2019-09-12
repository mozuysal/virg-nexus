/**
 * @file nx_epipolar.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2019 Mustafa Ozuysal. All rights reserved.
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
nx_fundamental_epipolar_distance_fwd(const double *f,
                                     struct NXPointMatch2D *pm)
{
        double x[2] =  { pm->x[0],  pm->x[1] };
        double xp[2] = { pm->xp[0], pm->xp[1] };

        double l[3];
        l[0] = f[0]*x[0] + f[3]*x[1] + f[6];
        l[1] = f[1]*x[0] + f[4]*x[1] + f[7];
        l[2] = f[2]*x[0] + f[5]*x[1] + f[8];

        // point - line distance in 2D
        return fabs((l[0]*xp[0] + l[1]*xp[1] + l[2])
                    / sqrt(l[0]*l[0] + l[1]*l[1]));
}

int nx_fundamental_mark_inliers(const double *f, int n_corr,
                                struct NXPointMatch2D *corr_list,
                                double inlier_tolerance);


double nx_fundamental_estimate_8pt    (double *f, int corr_ids[8],
                                       const struct NXPointMatch2D *corr_list);
double nx_fundamental_estimate        (double *f, int n_corr,
                                       const struct NXPointMatch2D *corr_list);
double nx_fundamental_estimate_inliers(double *f, int n_corr,
                                       const struct NXPointMatch2D *corr_list);

int nx_fundamental_estimate_ransac     (double *f, int n_corr,
                                        struct NXPointMatch2D *corr_list,
                                        double inlier_tolerance, int max_n_iter);
int nx_fundamental_estimate_norm_ransac(double *f, int n_corr,
                                        struct NXPointMatch2D *corr_list,
                                        double inlier_tolerance, int max_n_iter);

__NX_END_DECL

#endif
