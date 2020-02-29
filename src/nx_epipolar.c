/**
 * @file nx_epipolar.c
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
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
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
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_svd.h"
#include "virg/nexus/nx_vec234.h"
#include "virg/nexus/nx_mat234.h"
#include "virg/nexus/nx_math.h"
#include "virg/nexus/nx_uniform_sampler.h"
#include "virg/nexus/nx_point_match_2d_stats.h"

int nx_fundamental_mark_inliers (const double *F, int n_corr,
                                 struct NXPointMatch2D *corr_list,
                                 double inlier_tolerance)
{
        NX_ASSERT_PTR(F);
        NX_ASSERT_PTR(corr_list);

        int n_inliers = 0;
        for (int i = 0; i < n_corr; ++i) {
                struct NXPointMatch2D *pm_i = corr_list + i;
                double d = nx_fundamental_epipolar_distance_fwd(F, pm_i);
                /* double d_effective = d / pm_i->sigma_xp; */
                if (d < inlier_tolerance) {
                        pm_i->is_inlier = NX_TRUE;
                        n_inliers++;
                } else {
                        pm_i->is_inlier = NX_FALSE;
                }
        }

        return n_inliers;
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

static inline double nx_fundamental_svd_solve(double *F, int n_corr, double *A)
{
        double U[9*9];
        double S[9];
        nx_dsvd_us(&U[0], 9, &S[0], 9, n_corr, A, 9);

        F[0] = U[9*8+0];
        F[3] = U[9*8+1];
        F[6] = U[9*8+2];
        F[1] = U[9*8+3];
        F[4] = U[9*8+4];
        F[7] = U[9*8+5];
        F[2] = U[9*8+6];
        F[5] = U[9*8+7];
        F[8] = U[9*8+8];

        return S[8];
}

static inline double nx_fundamental_fix_rank(double *F)
{
        NX_ASSERT_PTR(F);

        double U[3*3];
        double S[3];
        double Vt[3*3];
        nx_dsvd_usvt(&U[0], 3, &S[0], &Vt[0], 3, 3, 3, F, 3);

        Vt[0] *= S[0];
        Vt[3] *= S[0];
        Vt[6] *= S[0];
        Vt[1] *= S[1];
        Vt[4] *= S[1];
        Vt[7] *= S[1];
        Vt[2] = 0.0;
        Vt[5] = 0.0;
        Vt[8] = 0.0;
        nx_dmat3_mul(F, U, Vt);

        return S[2];
}

double nx_fundamental_estimate_8pt(double *F, int corr_ids[8],
                                   const struct NXPointMatch2D *corr_list)
{
        NX_ASSERT_PTR(F);
        NX_ASSERT_PTR(corr_list);

        size_t lA = 8 * 9;
        double *A = NX_NEW_D(lA);
        double *Ac = A;
        for (int i = 0; i < 8; ++i, Ac += 9)
                nx_fundamental_constraints_from_corr(Ac, corr_list + corr_ids[i]);

        double sval = nx_fundamental_svd_solve(F, 8, A);
        nx_fundamental_fix_rank(F);
        nx_free(A);

        return sval;
}

double nx_fundamental_estimate(double *F, int n_corr,
                               const struct NXPointMatch2D *corr_list)
{
        NX_ASSERT_PTR(F);
        NX_ASSERT_PTR(corr_list);

        if (n_corr < 8) {
                NX_WARNING(NX_LOG_TAG, "Insufficient number of correspondences for fundamental matrix estimation!");
                return DBL_MAX;
        }
        NX_ASSERT(n_corr >= 8);

        size_t lA = n_corr * 9;
        double *A = NX_NEW_D(lA);
        double *Ac = A;
        for (int i = 0; i < n_corr; ++i, Ac += 9)
                nx_fundamental_constraints_from_corr(Ac, corr_list + i);

        double sval = nx_fundamental_svd_solve(F, n_corr, A);
        nx_fundamental_fix_rank(F);
        nx_free(A);

        /* nx_dmat3_print(F, "F"); */
        /* NX_LOG("SDA", "%.4g / %.4g", sval, s2); */

        return sval;
}

double nx_fundamental_estimate_inliers(double *F, int n_corr,
                                       const struct NXPointMatch2D *corr_list)
{
        NX_ASSERT_PTR(F);
        NX_ASSERT_PTR(corr_list);

        int n_inliers = nx_point_match_2d_count_inliers(n_corr, corr_list);
        if (n_inliers < 8) {
                NX_WARNING(NX_LOG_TAG, "Insufficient number of inliers for fundamental matrix estimation!");
                return DBL_MAX;
        }
        NX_ASSERT(n_inliers >= 8);

        size_t lA = n_inliers * 9;
        double *A = NX_NEW_D(lA);
        double *Ac = A;
        for (int i = 0; i < n_corr; ++i)
                if (corr_list[i].is_inlier) {
                        nx_fundamental_constraints_from_corr(Ac, corr_list + i);
                        Ac += 9;
                }

        double sval = nx_fundamental_svd_solve(F, n_inliers, A);
        nx_fundamental_fix_rank(F);
        nx_free(A);

        return sval;
}

static inline void nx_select_prosac_candidates(int n_top, int corr_ids[8])
{
        corr_ids[0] = (int)(NX_UNIFORM_SAMPLE_S*n_top);
        for (int i = 1; i < 8; ++i) {
                corr_ids[i] = (int)(NX_UNIFORM_SAMPLE_S*n_top);
                for (int j = i-1; j > 0; --j)
                        if (corr_ids[i] == corr_ids[j]) {
                                --i;
                                break;
                        }
        }
}

int nx_fundamental_estimate_ransac(double *F, int n_corr,
                                   struct NXPointMatch2D *corr_list,
                                   double inlier_tolerance, int max_n_iter)
{
        NX_ASSERT_PTR(F);
        NX_ASSERT_PTR(corr_list);

        double F_best[9];
        int n_inliers_best = 0;
        int n_inliers = 0;
        int corr_ids[8];

        if (n_corr < 8) {
                NX_WARNING(NX_LOG_TAG, "Insufficient number (< 8) of correspondences for fundamental matrix estimation with RANSAC!");
                return 0;
        }
        NX_ASSERT(n_corr >= 8);

        // This routine mimics PROSAC by reordering and sampling from the top
        // group. In every iteration the top group grows by PROSAC_INC
        const int PROSAC_START = 10;
        const int PROSAC_INC   = 1;

        int n_top_hypo = PROSAC_START;
        if (n_top_hypo > n_corr)
                n_top_hypo = n_corr;
        qsort((void *)corr_list, n_corr, sizeof(struct NXPointMatch2D),
              nx_point_match_2d_cmp_match_cost);

        // Main loop. Quits after max. num. of iterations or when we already
        // have a large group of inliers.
        int iter = 0;
        while (iter < max_n_iter && n_inliers < 100) {
                nx_select_prosac_candidates(n_top_hypo, corr_ids);
                nx_fundamental_estimate_8pt(F, corr_ids, corr_list);

                n_inliers = nx_fundamental_mark_inliers(F, n_corr, corr_list,
                                                        inlier_tolerance);

                // update if we got sth. better than current best
                if (n_inliers > n_inliers_best) {
                        memcpy(F_best, F, 9*sizeof(F[0]));
                        n_inliers_best = n_inliers;
                }
                /* NX_LOG("FRANSAC", "Iter %4d, inliers %4d, best inliers %4d", */
                       /* iter, n_inliers, n_inliers_best); */

                ++iter;
                n_top_hypo += PROSAC_INC;
                if (n_top_hypo >= n_corr) {
                        n_top_hypo = n_corr;
                }
        }

        // reestimate with all inliers and relabel inlier until it does not
        // improve much (by 5)
        memcpy(F, F_best, 9*sizeof(F[0]));
        n_inliers_best = nx_fundamental_mark_inliers(F_best, n_corr,
                                                     corr_list, inlier_tolerance);
        /* NX_LOG("FRANSAC", "Iter END, best inliers %4d", n_inliers_best); */

        n_inliers = 1;
        while (n_inliers_best > (n_inliers + 5)) {
                nx_fundamental_estimate_inliers(F, n_corr, corr_list);
                n_inliers = n_inliers_best;
                n_inliers_best = nx_fundamental_mark_inliers(F, n_corr,
                                                             corr_list,
                                                             inlier_tolerance);
        }

        /* NX_LOG("FRANSAC", "Estimate ALL, best inliers %4d", n_inliers_best); */
        return n_inliers_best;
}

int nx_fundamental_estimate_norm_ransac(double *F, int n_corr,
                                        struct NXPointMatch2D *corr_list,
                                        double inlier_tolerance, int max_n_iter)
{
        NX_ASSERT_PTR(F);
        NX_ASSERT_PTR(corr_list);

        struct NXPointMatch2DStats *stats = nx_point_match_2d_stats_new();
        nx_point_match_2d_stats_normalize_matches(stats, n_corr, corr_list);

        double norm_tol = inlier_tolerance / stats->dp;
        int n_inliers = nx_fundamental_estimate_ransac(F, n_corr, corr_list,
                                                       norm_tol, max_n_iter);

        nx_point_match_2d_stats_denormalize_fundamental(stats, F);
        nx_point_match_2d_stats_denormalize_matches(stats, n_corr, corr_list);
        nx_point_match_2d_stats_free(stats);

        return n_inliers;
}

// Compute E = K'.T*F*K
void nx_essential_from_fundamental(double *E, const double *F,
                                   const double *K, const double *Kp)
{
        double Fk[9];
        Fk[0] = K[0]*F[0];
        Fk[1] = K[0]*F[1];
        Fk[2] = K[0]*F[2];
        Fk[3] = K[3]*F[0] + K[4]*F[3];
        Fk[4] = K[3]*F[1] + K[4]*F[4];
        Fk[5] = K[3]*F[2] + K[4]*F[5];
        Fk[6] = K[6]*F[0] + K[7]*F[3] + F[6];
        Fk[7] = K[6]*F[1] + K[7]*F[4] + F[7];
        Fk[8] = K[6]*F[2] + K[7]*F[5] + F[8];

        E[0] = Kp[0]*Fk[0];
        E[3] = Kp[0]*Fk[3];
        E[6] = Kp[0]*Fk[6];
        E[1] = Kp[1]*Fk[0] + Kp[4]*Fk[1];
        E[4] = Kp[1]*Fk[3] + Kp[4]*Fk[4];
        E[7] = Kp[1]*Fk[6] + Kp[4]*Fk[7];
        E[2] = Kp[2]*Fk[0] + Kp[5]*Fk[1] + Fk[2];
        E[5] = Kp[2]*Fk[3] + Kp[5]*Fk[4] + Fk[5];
        E[8] = Kp[2]*Fk[6] + Kp[5]*Fk[7] + Fk[8];
}


// Computes E = [t]x * R
void nx_essential_from_Rt(double *E, const double *R, const double *t)
{
        E[0] = -t[2]*R[1] + t[1]*R[2];
        E[1] =  t[2]*R[0] - t[0]*R[2];
        E[2] = -t[1]*R[0] + t[0]*R[1];
        E[3] = -t[2]*R[4] + t[1]*R[5];
        E[4] =  t[2]*R[3] - t[0]*R[5];
        E[5] = -t[1]*R[3] + t[0]*R[4];
        E[6] = -t[2]*R[7] + t[1]*R[8];
        E[7] =  t[2]*R[6] - t[0]*R[8];
        E[8] = -t[1]*R[6] + t[0]*R[7];
}

double nx_essential_decompose_to_Rt(const double *E, double **R, double **t)
{
        double U[9];
        double S[3];
        double Vt[9];
        nx_dmat3_svd(E, U, S, Vt);

        double det = nx_dmat3_det(&U[0]);
        if (det < 0.0)
                for (int i = 0; i < 9; ++i)
                        U[i] = -U[i];

        det = nx_dmat3_det(&Vt[0]);
        if (det < 0.0)
                for (int i = 0; i < 9; ++i)
                        Vt[i] = -Vt[i];

        R[0][0] = U[3]*Vt[0] - U[0]*Vt[1] + U[6]*Vt[2];
        R[0][1] = U[4]*Vt[0] - U[1]*Vt[1] + U[7]*Vt[2];
        R[0][2] = U[5]*Vt[0] - U[2]*Vt[1] + U[8]*Vt[2];
        R[0][3] = U[3]*Vt[3] - U[0]*Vt[4] + U[6]*Vt[5];
        R[0][4] = U[4]*Vt[3] - U[1]*Vt[4] + U[7]*Vt[5];
        R[0][5] = U[5]*Vt[3] - U[2]*Vt[4] + U[8]*Vt[5];
        R[0][6] = U[3]*Vt[6] - U[0]*Vt[7] + U[6]*Vt[8];
        R[0][7] = U[4]*Vt[6] - U[1]*Vt[7] + U[7]*Vt[8];
        R[0][8] = U[5]*Vt[6] - U[2]*Vt[7] + U[8]*Vt[8];

        t[0][0] = U[6]; t[0][1] = U[7]; t[0][2] = U[8];

        memcpy(&R[1][0], &R[0][0], 9*sizeof(R[0][0]));
        t[1][0] = -U[6]; t[1][1] = -U[7]; t[1][2] = -U[8];

        R[2][0] = -U[3]*Vt[0] + U[0]*Vt[1] + U[6]*Vt[2];
        R[2][1] = -U[4]*Vt[0] + U[1]*Vt[1] + U[7]*Vt[2];
        R[2][2] = -U[5]*Vt[0] + U[2]*Vt[1] + U[8]*Vt[2];
        R[2][3] = -U[3]*Vt[3] + U[0]*Vt[4] + U[6]*Vt[5];
        R[2][4] = -U[4]*Vt[3] + U[1]*Vt[4] + U[7]*Vt[5];
        R[2][5] = -U[5]*Vt[3] + U[2]*Vt[4] + U[8]*Vt[5];
        R[2][6] = -U[3]*Vt[6] + U[0]*Vt[7] + U[6]*Vt[8];
        R[2][7] = -U[4]*Vt[6] + U[1]*Vt[7] + U[7]*Vt[8];
        R[2][8] = -U[5]*Vt[6] + U[2]*Vt[7] + U[8]*Vt[8];

        t[2][0] = U[6]; t[2][1] = U[7]; t[2][2] = U[8];

        memcpy(&R[3][0], &R[2][0], 9*sizeof(R[0][0]));
        t[3][0] = -U[6]; t[3][1] = -U[7]; t[3][2] = -U[8];

        return S[2];
}

static int nx_epipolar_get_z_sign_sum(int n, const struct NXPointMatch2D *corr_list,
                                      const double *R, const double *t)
{
        // Algorithm 20, Chapter 13, page 583 in
        // Photogrammetric Computer Vision by W. Förstner and B. P. Wrobel
        // for correspondence u <-> v and relative orientation R, t
        // b = -R.T * t
        // m = N((b x u) x b)
        // sign_v = sign(det([b m (u x R.T * v)]))
        // sign_u = sign_v * sign(m.T * R.T * v)
        double A[9];
        double *b = &A[0];
        b[0] = -R[0]*t[0] - R[1]*t[1] - R[2]*t[2];
        b[1] = -R[3]*t[0] - R[4]*t[1] - R[5]*t[2];
        b[2] = -R[6]*t[0] - R[7]*t[1] - R[8]*t[2];

        int s_sum  = 0;
        int sp_sum = 0;
        for (int i = 0; i < n; ++i) {
                const struct NXPointMatch2D *pm = corr_list + i;
                double u[3] = { pm->x[0], pm->x[1], 1.0 };
                double *m = &A[3];
                double temp[3];
                nx_dvec3_cross(&temp[0], b, &u[0]);
                nx_dvec3_cross(m, &temp[0], b);
                nx_dvec3_to_unit(m);

                double Rtv[3];
                Rtv[0] = R[0]*pm->xp[0] + R[1]*pm->xp[1] + R[2];
                Rtv[1] = R[3]*pm->xp[0] + R[4]*pm->xp[1] + R[5];
                Rtv[2] = R[6]*pm->xp[0] + R[7]*pm->xp[1] + R[8];
                nx_dvec3_cross(&A[6], &u[0], &Rtv[0]);

                int sp = nx_signum(nx_dmat3_det(&A[0]));
                int s  = sp * nx_signum(nx_dvec3_dot(&m[0], &Rtv[0]));

                s_sum  += s;
                sp_sum += sp;
        }

        return s_sum + sp_sum;
}

int nx_epipolar_pick_best_Rt(const double * const *R, const double * const *t,
                             int n, const struct NXPointMatch2D *corr_list,
                             int *z_sign_sum)
{
        NX_ASSERT_PTR(R);
        NX_ASSERT_PTR(R[0]);
        NX_ASSERT_PTR(R[1]);
        NX_ASSERT_PTR(R[2]);
        NX_ASSERT_PTR(R[3]);
        NX_ASSERT_PTR(t);
        NX_ASSERT_PTR(t[0]);
        NX_ASSERT_PTR(t[1]);
        NX_ASSERT_PTR(t[2]);
        NX_ASSERT_PTR(t[3]);
        NX_ASSERT_PTR(corr_list);
        NX_ASSERT(n > 0);

        int idx = 0;
        int sum_idx = nx_epipolar_get_z_sign_sum(n, corr_list, &R[0][0], &t[0][0]);

        for (int i = 1; i < 4; ++i) {
                int sum = nx_epipolar_get_z_sign_sum(n, corr_list,
                                                     &R[i][0], &t[i][0]);
                if (sum > sum_idx) {
                        idx = i;
                        sum_idx = sum;
                }
        }

        if (z_sign_sum)
                *z_sign_sum = sum_idx;

        return idx;
}
