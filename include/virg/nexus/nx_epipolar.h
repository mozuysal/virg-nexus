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

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_point_match_2d.h"

__NX_BEGIN_DECL


int nx_fundamental_mark_inliers(const double *f, int n_corr,
                                struct NXPointMatch2D *corr_list,
                                double inlier_tolerance);


double nx_fundamental_estimate_8_point(double *f, int corr_ids[8],
                                       const struct NXPointMatch2D *corr_list);
double nx_fundamental_estimate        (double *f, int n_corr,
                                       const struct NXPointMatch2D *corr_list);
double nx_fundamental_estimate_inliers(double *f, int n_corr,
                                       const struct NXPointMatch2D *corr_list);

__NX_END_DECL

#endif
