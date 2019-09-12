/**
 * @file nx_epipolar.c
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
#include "virg/nexus/nx_epipolar.h"

#include <float.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_svd.h"

int nx_fundamental_count_inliers(int n_corr,
                                 const struct NXPointMatch2D *corr_list)
{
        return 0;
}

int nx_fundamental_mark_inliers (const double *f, int n_corr,
                                 struct NXPointMatch2D *corr_list,
                                 double inlier_tolerance)
{
        for (int i = 0; i < n_corr; ++i) {
                corr_list[i].is_inlier = NX_FALSE;
        }

        return 0;
}

double nx_fundamental_estimate_8_point(double *f, int corr_ids[8],
                                       const struct NXPointMatch2D *corr_list)
{
        return 10000.0;
}

static inline void nx_fundamental_constraints_from_corr(double *rcons, const struct NXPointMatch2D *corr)
{
        double x  = corr->x[0];
        double y  = corr->x[1];
        double xp = corr->xp[0];
        double yp = corr->xp[1];

        rcons[0]  = x*xp; rcons[1]  = xp*y; rcons[2]  = xp;
        rcons[3]  = yp*x; rcons[4]  = yp*y; rcons[5]  = yp;
        rcons[6]  = x;    rcons[7]  = y;    rcons[8]  = 1.0;
}

static inline double nx_fundamental_svd_solve(double *f, int n_corr, double *A)
{
        double U[9*9];
        double S[9];
        nx_dsvd_us(&U[0], 9, &S[0], 9, n_corr, A, 9);

        f[0] = U[9*8+0];
        f[3] = U[9*8+1];
        f[6] = U[9*8+2];
        f[1] = U[9*8+3];
        f[4] = U[9*8+4];
        f[7] = U[9*8+5];
        f[2] = U[9*8+6];
        f[5] = U[9*8+7];
        f[8] = U[9*8+8];

        return S[8];
}


double nx_fundamental_estimate(double *f, int n_corr,
                               const struct NXPointMatch2D *corr_list)
{
        if (n_corr < 8) {
                NX_WARNING(NX_LOG_TAG, "Insufficient number of correspondences for fundamental matrix estimation!");
                return DBL_MAX;
        }

        size_t lA = n_corr * 9;
        double *A = NX_NEW_D(lA);
        double *Ac = A;
        for (int i = 0; i < n_corr; ++i, Ac += 9)
                nx_fundamental_constraints_from_corr(Ac, corr_list + i);

        double sval = nx_fundamental_svd_solve(f, n_corr, A);
        nx_free(A);

        return sval;
}

double nx_fundamental_estimate_inliers(double *f, int n_corr,
                                       const struct NXPointMatch2D *corr_list)
{
        return 10000.0;
}
