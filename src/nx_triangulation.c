/**
 * @file nx_triangulation.c
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

#include "virg/nexus/nx_triangulation.h"

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_svd.h"

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

int nx_triangulate(int n_pt, struct NXRPoint3D *X_list,
                   int n_corr, const struct NXPointMatch2D *corr_list,
                   const double *P0, const double *P1)
{
        NX_ASSERT_PTR(X_list);
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

                        struct NXRPoint3D *rpt = X_list + pt_i;
                        rpt->X[0] = U[4*3+0];
                        rpt->X[1] = U[4*3+1];
                        rpt->X[2] = U[4*3+2];
                        rpt->X[3] = U[4*3+3];
                        rpt->id = pt_i;
                        rpt->corr_id = i;

                        pt_i++;
                }
        }

        NX_ASSERT(n == pt_i);

        return n;
}
