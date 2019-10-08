/**
 * @file nx_pinhole.c
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
#include "virg/nexus/nx_pinhole.h"

#include <float.h>
#include <math.h>

#include "virg/nexus/nx_svd.h"
#include "virg/nexus/nx_math.h"
#include "virg/nexus/nx_mat234.h"
#include "virg/nexus/nx_assert.h"

void nx_pinhole_projection_from_krt(double *P, const double *K,
                                    const double *R, const double *t)
{
        NX_ASSERT_PTR(P);

        const double I[12] = { 1.0, 0.0, 0.0,
                               0.0, 1.0, 0.0,
                               0.0, 0.0, 1.0,
                               0.0, 0.0, 0.0 };
        const double *Kv = K ? K : &I[0];
        const double *Rv = R ? R : &I[0];
        const double *tv = t ? t : &I[9];

        nx_dmat3_mul_ua(P, Kv, Rv);
        P[9]  = Kv[0]*tv[0] + Kv[3]*tv[1] + Kv[6]*tv[2];
        P[10] = Kv[4]*tv[1] + Kv[7]*tv[2];
        P[11] = tv[2];
}

double nx_pinhole_project(const double *P, int n, const float *X, float *x)
{
        NX_ASSERT_PTR(P);
        NX_ASSERT(n > 0);
        NX_ASSERT_PTR(X);
        NX_ASSERT_PTR(x);

        double min_abs_s = DBL_MAX;
        for (int i = 0; i < n; ++i, x += 2, X += 3) {
                /* NX_LOG(NX_LOG_TAG, "%02d: %.2f %.2f %.2f", i, X[0], X[1], X[2]); */

                double s = (P[2]*X[0] + P[5]*X[1] + P[8]*X[2] + P[11]);
                min_abs_s = nx_min_d(min_abs_s, fabs(s));

                /* NX_LOG(NX_LOG_TAG, "  %.2f -> min = %.2f", s, min_abs_s); */

                double inv_s = 1.0 / s;
                x[0] = (P[0]*X[0] + P[3]*X[1] + P[6]*X[2] + P[9])*inv_s;
                x[1] = (P[1]*X[0] + P[4]*X[1] + P[7]*X[2] + P[10])*inv_s;
        }

        NX_ASSERT(min_abs_s > 0.0);

        return min_abs_s;
}

void nx_pinhole_project_h(const double *P, int n, const float *Xh, float *xh)
{
        NX_ASSERT_PTR(P);
        NX_ASSERT(n > 0);
        NX_ASSERT_PTR(Xh);
        NX_ASSERT_PTR(xh);

        for (int i = 0; i < n; ++i, xh += 3, Xh += 4) {
                xh[0] = P[0]*Xh[0] + P[3]*Xh[1] + P[6]*Xh[2] + P[9] *Xh[3];
                xh[1] = P[1]*Xh[0] + P[4]*Xh[1] + P[7]*Xh[2] + P[10]*Xh[3];
                xh[2] = P[2]*Xh[0] + P[5]*Xh[1] + P[8]*Xh[2] + P[11]*Xh[3];
        }
}

static inline void
nx_triangulation_constraints_from_corr(double *rcons,
                                       const struct NXPointMatch2D *corr,
                                       const double *P0, const double *P1)
{
        const float x = corr->x[0];
        const float y = corr->x[1];
        const float xp = corr->xp[0];
        const float yp = corr->xp[1];

        rcons[0] = x*P0[2]  - P0[0];
        rcons[1] = x*P0[5]  - P0[3];
        rcons[2] = x*P0[8]  - P0[6];
        rcons[3] = x*P0[11] - P0[9];

        rcons[4] = y*P0[2]  - P0[1];
        rcons[5] = y*P0[5]  - P0[4];
        rcons[6] = y*P0[8]  - P0[7];
        rcons[7] = y*P0[11] - P0[10];

        rcons[8]  = xp*P1[2]  - P1[0];
        rcons[9]  = xp*P1[5]  - P1[3];
        rcons[10] = xp*P1[8]  - P1[6];
        rcons[11] = xp*P1[11] - P1[9];

        rcons[12] = yp*P1[2]  - P1[1];
        rcons[13] = yp*P1[5]  - P1[4];
        rcons[14] = yp*P1[8]  - P1[7];
        rcons[15] = yp*P1[11] - P1[10];

}

int nx_pinhole_triangulate(int n_pt, float *X, int *corr_ids,
                           int n_corr, const struct NXPointMatch2D *corr_list,
                           const double *P0, const double *P1)
{
        NX_ASSERT_PTR(X);
        NX_ASSERT_PTR(corr_ids);
        NX_ASSERT_PTR(corr_list);
        NX_ASSERT_PTR(P0);
        NX_ASSERT_PTR(P1);

        int n = nx_point_match_2d_count_inliers(n_corr,
                                                corr_list);
        if (n > n_pt) {
                n = n_pt;
        }

        double A[16];
        double U[16];
        double S[4];
        int pt_i = 0;
        for (int i = 0; i < n_corr; ++i) {
                if (corr_list[i].is_inlier) {
                        nx_triangulation_constraints_from_corr(&A[0],
                                                               corr_list + i,
                                                               P0, P1);
                        nx_dsvd_us(&U[0], 4, &S[0], 4, 4, A, 4);

                        float *Xi = X + 3*pt_i;
                        double W = U[4*3+3];
                        Xi[0] = U[4*3+0] / W;
                        Xi[1] = U[4*3+1] / W;
                        Xi[2] = U[4*3+2] / W;
                        corr_ids[pt_i] = i;

                        pt_i++;
                }
        }

        NX_ASSERT(n == pt_i);

        return n;
}
