/**
 * @file nx_homography.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2015 Mustafa Ozuysal. All rights reserved.
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

double nx_homography_estimate_4pt        (double *h, int corr_ids[4],
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
