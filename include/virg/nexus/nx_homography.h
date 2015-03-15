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

__NX_BEGIN_DECL

struct NXHCorr {
        double x[2];
        double xp[2];
        double match_cost;
        int id;
        int idp;
        NXBool is_inlier;
};

double nx_homography_map(double *xp, const double *x, const double *h);

// x[TL, TR, BR, BL] -> unit square[(0,0) - (1,0) - (1,1) - (0,1)]
double nx_homography_estimate_unit(double *h, const double *x);
NXBool nx_homography_check(const double *h, double max_abs_cos);
int nx_homography_count_inliers(int n_corr, const struct NXHCorr *corr_list);
int nx_homography_mark_inliers(int n_corr, struct NXHCorr *corr_list, const double *h, double inlier_tolerance);

void nx_homography_estimate_4pt(double *h, int corr_ids[4], const struct NXHCorr *corr_list);
double nx_homography_estimate_dlt(double *h, int n_corr, const struct NXHCorr *corr_list);
double nx_homography_estimate_dlt_inliers(double *h, int n_corr, const struct NXHCorr *corr_list);

int nx_homography_estimate_ransac(double *h, int n_corr, struct NXHCorr *corr_list, double inlier_tolerance, int max_n_iter);
int nx_homography_estimate_norm_ransac(double *h, int n_corr, struct NXHCorr *corr_list, double inlier_tolerance, int max_n_iter);

__NX_END_DECL

#endif
