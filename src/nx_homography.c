/**
 * @file nx_homography.c
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
#include "virg/nexus/nx_homography.h"

#include <math.h>
#include <string.h>
#include <float.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_math.h"
#include "virg/nexus/nx_uniform_sampler.h"
#include "virg/nexus/nx_mat234.h"
#include "virg/nexus/nx_svd.h"
#include "virg/nexus/nx_point_match_2d_stats.h"

static inline void line_eqn_2d(const double* p1, const double* p2, double* l)
{
        l[0] = p2[1] - p1[1];
        l[1] = p1[0] - p2[0];
        l[2] = - p1[0]*l[0] - p1[1]*l[1];
}

// Returns false if the projection of a square centered around
// origin is skewed too much or mirrored
NXBool nx_homography_check(const double *h, double max_abs_cos)
{
        // Project square corners to A, B, C, D
        double t_sum[3]  = { h[0] + h[3], h[1] + h[4], h[2] + h[5] };
        double t_diff[3] = { h[0] - h[3], h[1] - h[4], h[2] - h[5] };

        // proj. (-1, -1)
        double s_inv = 1.0f / (-t_sum[2] + h[8]);
        double A[2] = { (-t_sum[0] + h[6]) * s_inv,
                        (-t_sum[1] + h[7]) * s_inv };

        // proj. (+1, -1)
        s_inv = 1.0f / (t_diff[2] + h[8]);
        double B[2] = { (t_diff[0] + h[6]) * s_inv,
                        (t_diff[1] + h[7]) * s_inv };

        // proj. (+1, +1)
        s_inv = 1.0f / (t_sum[2] + h[8]);
        double C[2] = { (t_sum[0] + h[6]) * s_inv,
                        (t_sum[1] + h[7]) * s_inv };

        // proj. (-1, +1)
        s_inv = 1.0f / (-t_diff[2] + h[8]);
        double D[2] = { (-t_diff[0] + h[6]) * s_inv,
                        (-t_diff[1] + h[7]) * s_inv };

        double l_AC[3];
        line_eqn_2d(&A[0], &C[0], &l_AC[0]);

        double n_inv = 1.0 / (l_AC[0]*l_AC[0] + l_AC[1]*l_AC[1]);
        l_AC[0] *= n_inv;
        l_AC[1] *= n_inv;
        l_AC[2] *= n_inv;

        double B_val = l_AC[0] * B[0] + l_AC[1] * B[1] + l_AC[2];
        double D_val = l_AC[0] * D[0] + l_AC[1] * D[1] + l_AC[2];

        NXBool cross_check = (B_val > 0) && (D_val < 0);

        double AC[2] = { -l_AC[1], l_AC[0] };
        double BD[2] = { B[0] - D[0], B[1] - D[1] };

        n_inv = 1.0 / (BD[0]*BD[0] + BD[1]*BD[1]);
        BD[0] *= n_inv;
        BD[1] *= n_inv;

        double cos_AC_BD = AC[0]*BD[0]+AC[1]*BD[1];
        NXBool angle_check = fabs(cos_AC_BD) <= max_abs_cos;

        return cross_check && angle_check;
}

int nx_homography_count_inliers(int n_corr, const struct NXPointMatch2D *corr_list)
{
        int n_inliers = 0;
        for (int i = 0; i < n_corr; ++i)
                if (corr_list[i].is_inlier)
                        ++n_inliers;
        return n_inliers;
}

int nx_homography_mark_inliers(int n_corr, struct NXPointMatch2D *corr_list,
                               const double *h, double inlier_tolerance)
{
        double tol_sqr = inlier_tolerance*inlier_tolerance;
        int n_inliers = 0;

        for (int i = 0; i < n_corr; ++i) {
                float mp[2];
                nx_homography_map(&mp[0], &(corr_list[i].x[0]), h);

                double d[2] = { mp[0] - corr_list[i].xp[0],
                                mp[1] - corr_list[i].xp[1] };
                if (d[0]*d[0] + d[1]*d[1] < tol_sqr) {
                        corr_list[i].is_inlier = NX_TRUE;
                        ++n_inliers;
                } else {
                        corr_list[i].is_inlier = NX_FALSE;
                }
        }

        return n_inliers;
}

void nx_homography_estimate_4pt(double *h, int corr_ids[4], const struct NXPointMatch2D *corr_list)
{
        double x[8];
        for( int i = 0; i < 4; ++i ) {
                const struct NXPointMatch2D* corr = corr_list + corr_ids[i];
                x[2*i]   = corr->x[0];
                x[2*i+1] = corr->x[1];
        }

        double hu[9];
        nx_homography_estimate_unit(hu, x);

        double xp[8];
        for( int i = 0; i < 4; ++i ) {
                const struct NXPointMatch2D* corr = corr_list + corr_ids[i];
                xp[2*i]   = corr->xp[0];
                xp[2*i+1] = corr->xp[1];
        }

        double hup[9];
        nx_homography_estimate_unit(hup, xp);

        double hup_inv[9] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
        nx_dmat3_inv(hup_inv, hup);
        nx_dmat3_mul(h, hup_inv, hu);
}

static inline void nx_homography_constraints_from_corr(double *rcons, const struct NXPointMatch2D *corr)
{
        const float* m  = &(corr->x[0]);
        const float* mp = &(corr->xp[0]);

        rcons[0]  = 0.0;         rcons[1]  = 0.0;         rcons[2]  = 0.0;
        rcons[3]  = -m[0];       rcons[4]  = -m[1];       rcons[5]  = -1.0;
        rcons[6]  = mp[1]*m[0];  rcons[7]  = mp[1]*m[1];  rcons[8]  = mp[1];
        rcons[9]  = m[0];        rcons[10] = m[1];        rcons[11] = 1.0;
        rcons[12] = 0.0;         rcons[13] = 0.0;         rcons[14] = 0.0;
        rcons[15] = -mp[0]*m[0]; rcons[16] = -mp[0]*m[1]; rcons[17] = -mp[0];
}

static inline double nx_homography_svd_solve(double *h, int n_corr, double *A)
{
        double U[9*9];
        double S[9];
        nx_dsvd_us(&U[0], 9, &S[0], 9, 2*n_corr, A, 9);

        h[0] = U[9*8+0];
        h[3] = U[9*8+1];
        h[6] = U[9*8+2];
        h[1] = U[9*8+3];
        h[4] = U[9*8+4];
        h[7] = U[9*8+5];
        h[2] = U[9*8+6];
        h[5] = U[9*8+7];
        h[8] = U[9*8+8];

        return S[8];
}

double nx_homography_estimate_dlt(double *h, int n_corr, const struct NXPointMatch2D *corr_list)
{
        if (n_corr < 4) {
                NX_WARNING(NX_LOG_TAG, "Insufficient number of correspondences for homography estimation by DLT!");
                return DBL_MAX;
        }

        size_t lA = 2*n_corr * 9;
        double *A = NX_NEW_D(lA);
        double *Ac = A;
        for (int i = 0; i < n_corr; ++i, Ac += 18)
                nx_homography_constraints_from_corr(Ac, corr_list + i);

        double sval = nx_homography_svd_solve(h, n_corr, A);
        nx_free(A);

        return sval;
}

double nx_homography_estimate_dlt_inliers(double *h, int n_corr, const struct NXPointMatch2D *corr_list)
{
        int n_inliers = nx_homography_count_inliers(n_corr, corr_list);
        if (n_inliers < 4) {
                NX_WARNING(NX_LOG_TAG, "Insufficient number of inliers for homography estimation by DLT!");
                return DBL_MAX;
        }

        size_t lA = 2*n_inliers * 9;
        double *A = NX_NEW_D(lA);
        double *Ac = A;
        for (int i = 0; i < n_corr; ++i)
                if (corr_list[i].is_inlier) {
                        nx_homography_constraints_from_corr(Ac, corr_list + i);
                        Ac += 18;
                }

        double sval = nx_homography_svd_solve(h, n_inliers, A);
        nx_free(A);

        return sval;
}

static int nx_hcorr_cmp_match_cost(const void *c0, const void *c1)
{
        const struct NXPointMatch2D *corr0 = (const struct NXPointMatch2D *)c0;
        const struct NXPointMatch2D *corr1 = (const struct NXPointMatch2D *)c1;

        if (corr0->match_cost > corr1->match_cost)
                return +1;
        else if (corr0->match_cost < corr1->match_cost)
                return -1;
        else
                return 0;
}

static inline void nx_select_prosac_candidates(int n_top, int corr_ids[4])
{
        corr_ids[0] = (int)(NX_UNIFORM_SAMPLE_S*n_top);

        do {
            corr_ids[1] = (int)(NX_UNIFORM_SAMPLE_S*n_top);
        } while(corr_ids[1]==corr_ids[0]);

        do {
            corr_ids[2] = (int)(NX_UNIFORM_SAMPLE_S*n_top);
        } while(corr_ids[2]==corr_ids[1] || corr_ids[2]==corr_ids[0]);

        do {
            corr_ids[3] = (int)(NX_UNIFORM_SAMPLE_S*n_top);
        } while(corr_ids[3]==corr_ids[1] || corr_ids[3]==corr_ids[1] || corr_ids[3]==corr_ids[0]);
}

int nx_homography_estimate_ransac(double *h, int n_corr, struct NXPointMatch2D *corr_list, double inlier_tolerance, int max_n_iter)
{
        double min_angle = 15.0 * NX_PI / 180.0;
        double max_abs_cos = fabs(cos(min_angle));
        double h_best[9];
        int n_inliers_best = 0;
        int n_inliers = 0;
        int corr_ids[4];

        if (n_corr < 4)
            return 0;

        // This routine mimics PROSAC by reordering and sampling from the top
        // group. In every iteration the top group grows by PROSAC_INC
        const int PROSAC_START = 10;
        const int PROSAC_INC   = 1;

        int n_top_hypo = PROSAC_START;
        if (n_top_hypo > n_corr)
            n_top_hypo = n_corr;
        qsort((void *)corr_list, n_corr, sizeof(struct NXPointMatch2D), nx_hcorr_cmp_match_cost);

        // Main loop. Quits after max. num. of iterations of we already have a
        // large group of inliers.
        int iter = 0;
        while (iter < max_n_iter && n_inliers < 100) {

            // In every iteration try to generate a good homography
            int tries = 10;
            do {
                nx_select_prosac_candidates(n_top_hypo, corr_ids);
                nx_homography_estimate_4pt(h, corr_ids, corr_list);
            } while (--tries && !nx_homography_check(h, max_abs_cos));

            n_inliers = nx_homography_mark_inliers(n_corr, corr_list, h, inlier_tolerance);

            // update if we got sth. better than current best
            if (n_inliers > n_inliers_best) {
                memcpy(h_best, h, 9*sizeof(h[0]));
                n_inliers_best = n_inliers;
            }

            /* printf("%d ", iter); */
            ++iter;
            n_top_hypo += PROSAC_INC;
            if (n_top_hypo >= n_corr) {
                n_top_hypo = n_corr;
            }
        }

        // reestimate with all inliers and relabel inlier until it does not improve much (by 5)
        memcpy(h, h_best, 9*sizeof(h[0]));
        n_inliers_best = nx_homography_mark_inliers(n_corr, corr_list, h_best, inlier_tolerance);
        n_inliers = 1;
        while (n_inliers_best > (n_inliers + 5)) {
                nx_homography_estimate_dlt_inliers(h, n_corr, corr_list);
                n_inliers = n_inliers_best;
                n_inliers_best = nx_homography_mark_inliers(n_corr, corr_list, h, inlier_tolerance);
        }

        // last check for homography, return zero inliers if fails.
        if (!nx_homography_check(h, max_abs_cos))
            return 0;

        return n_inliers_best;
}

int nx_homography_estimate_norm_ransac(double *h, int n_corr,
                                       struct NXPointMatch2D *corr_list,
                                       double inlier_tolerance, int max_n_iter)
{
        struct NXPointMatch2DStats *stats = nx_point_match_2d_stats_new();
        nx_point_match_2d_stats_normalize_matches(stats, n_corr, corr_list);

        double norm_tol = inlier_tolerance / stats->dp;
        int n_inliers = nx_homography_estimate_ransac(h, n_corr, corr_list, norm_tol, max_n_iter);

        nx_point_match_2d_stats_denormalize_homography(stats, h);
        nx_point_match_2d_stats_denormalize_matches(stats, n_corr, corr_list);
        nx_point_match_2d_stats_free(stats);

        return n_inliers;
}

// x[TL, TR, BR, BL] -> unit square[(0,0) - (1,0) - (1,1) - (0,1)]
double nx_homography_estimate_unit(double *h, const double *x)
{
        double t1 = x[0] * x[3];
        double t2 = x[0] * x[5];
        double t3 = x[4] * x[3];
        double t4 = x[2] * x[1];
        double t5 = x[4] * x[1];
        double t6 = x[2] * x[5];
        double t8 = x[2] * x[7];
        double t9 = x[6] * x[5];
        double t10 = x[6] * x[3];
        double t11 = x[7] * x[4];
        double t12 = -t8 + t6 - t3 - t9 + t10 + t11;
        double t15 = x[0] * x[6];
        double t16 = x[3]*x[3];
        double t19 = 2.0f * t16 * t5 * t15;
        double t20 = t16 * x[4];
        double t22 = x[6] * t20 * t2;
        double t24 = x[0] * x[2];
        double t25 = x[7]*x[7];
        double t26 = x[4] * t25;
        double t28 = x[1] * t26 * t24;
        double t30 = x[0]*x[0];
        double t31 = x[7] * t30;
        double t32 = x[3] * t9;
        double t33 = t32 * t31;
        double t35 = x[5] * t8;
        double t36 = x[6] * x[4];
        double t37 = x[1] * t36;
        double t39 = 2.0f * t37 * t35;
        double t41 = x[7] * t10;
        double t43 = 2.0f * t41 * x[4] * t2;
        double t44 = x[4]*x[4];
        double t45 = t44 * x[7];
        double t46 = x[6] * t16;
        double t48 = x[7] * x[0];
        double t50 = x[1] * t10;
        double t51 = t50 * x[4] * t48;
        double t53 = x[6] * x[2];
        double t54 = x[5] * t53;
        double t55 = x[3] * t5;
        double t56 = t55 * t54;
        double t58 = x[2]*x[2];
        double t59 = x[7] * t58;
        double t60 = x[5]*x[5];
        double t61 = x[6] * t60;
        double t63 = t60 * t58;
        double t64 = x[6] * x[1];
        double t65 = t64 * t63;
        double t67 = t60 * x[0];
        double t68 = x[3] * t53;
        double t69 = t68 * t67;
        double t71 = x[1]*x[1];
        double t72 = t71 * x[6];
        double t74 = x[6]*x[6];
        double t75 = t74 * x[5];
        double t76 = x[4] * t71;
        double t78 = -t19 + 2.0f * t22 + 2.0f * t28 + 2.0f * t33 + t39 - t43 + t46 * t45 + 2.0f * t51 - 2.0f * t56 - t61 * t59 + 2.0f * t65 - 2.0f * t69 + t72 * t45 - t76 * t75;
        double t79 = x[2] * t71;
        double t80 = t79 * t75;
        double t82 = x[5] * t24;
        double t83 = x[7] * t3;
        double t84 = t83 * t82;
        double t86 = x[5] * t58;
        double t87 = t72 * t86;
        double t89 = x[7] * t53;
        double t92 = t72 * t59;
        double t93 = t60 * x[2];
        double t94 = x[1] * t74;
        double t95 = t94 * t93;
        double t97 = x[3] * t74;
        double t98 = t79 * t97;
        double t99 = x[1] * t86;
        double t101 = 2.0f * t99 * t48;
        double t103 = t44 * t25;
        double t104 = x[2] * x[3];
        double t107 = x[5] * x[4];
        double t108 = t74 * t16;
        double t110 = t71 * t44;
        double t111 = t104 * t110;
        double t112 = t25 * x[0];
        double t113 = x[2] * x[4];
        double t117 = 2.0f * t80 + 2.0f * t84 - 2.0f * t87 + 2.0f * t89 * t67 + t92 - 2.0f * t95 - t98 + t101 + t86 * t26 - t104 * t103 + t97 * t93 - t108 * t107 - t111 - 2.0f * x[5] * t113 * t112;
        double t119 = x[2] * t30;
        double t120 = x[3] * t25;
        double t121 = t120 * t119;
        double t122 = t86 * t76;
        double t123 = x[2] * t48;
        double t126 = t60 * t30;
        double t127 = t104 * t126;
        double t128 = x[1] * t9;
        double t129 = t128 * t123;
        double t131 = x[6] * x[7];
        double t133 = x[1] * t15;
        double t134 = x[3] * t6;
        double t138 = t25 * t30;
        double t139 = t3 * t138;
        double t141 = x[5] * t30;
        double t142 = t20 * t141;
        double t143 = t20 * t31;
        double t147 = x[6] * t30;
        double t148 = x[7] * t16;
        double t149 = t148 * t147;
        double t150 = x[1] * t58;
        double t151 = t150 * t67;
        double t152 = t121 + t122 - 2.0f * t55 * t123 + t127 - 2.0f * t129 - t131 * t126 + 2.0f * t134 * t133 + t94 * t67 - 2.0f * t139 - t142 + 2.0f * t143 - 2.0f * t50 * t45 - t149 - t151;
        double t154 = t1 * t103;
        double t157 = x[1] * t25;
        double t158 = t157 * t58 * x[0];
        double t159 = x[1] * t44;
        double t161 = t16 * t44;
        double t162 = t161 * t48;
        double t164 = t44 * x[0];
        double t165 = t16 * x[1];
        double t166 = t165 * t164;
        double t167 = t74 * x[0];
        double t168 = t165 * t167;
        double t169 = t71 * t36;
        double t170 = t169 * t8;
        double t173 = x[3] * t2 * t94;
        double t176 = 2.0f * t134 * t31;
        double t177 = x[1] * t75;
        double t180 = t71 * t53;
        double t182 = 2.0f * t180 * t3;
        double t183 = x[1] * t104;
        double t184 = t183 * t45;
        double t186 = t99 * t11;
        double t188 = t141 * t26 + 2.0f * t154 - t158 - t159 * t112
                - 2.0f * t162 + t166 + t168 - 2.0f * t170 - 2.0f * t173
                - t176 + 2.0f * t177 * t3 + t182 + 2.0f * t184 - 2.0f * t186;

        double det  = t78 + t117 + t152 + t188;

        double t191 = 1.0f / det;
        double t194 = x[3] * t15;
        double t195 = x[0] * x[4];
        double t196 = x[3] * t195;
        double t198 = x[1] * t53;
        double t200 = x[3] * t36;
        double t202 = x[3] * t30;
        double t203 = -t194 - t196 + x[1] * t195 + t198 + t82
                - x[1] * t24 - t54 + t200 - t37
                + x[5] * t15 - t141 + t202;
        double t208 = t3 * t48;
        double t209 = x[7] * x[1];
        double t212 = x[7] * x[5];
        double t213 = t212 * t24;
        double t214 = x[1] * x[0];
        double t216 = t64 * t6;
        double t217 = t64 * t3;
        double t218 = x[1] * x[5];
        double t220 = x[7] * t202 - x[5] * t31 - t208 - t209 * t24 + t5 * t48 + t213 - t10 * t214 + t180 - t216 + t217 - t169 + t218 * t15;
        double t223 = x[3] * t45;
        double t225 = t74 * t60;
        double t232 = t25 * x[2];
        double t236 = -t223 + t103 - x[2] * t61 + t225 - x[6] * t67 + x[1] * t97 + t208 + x[3] * t159 - t177 - x[3] * t75 - x[7] * t159 + x[2] * t67 - x[4] * t232 + t25 * t24 - x[4] * t112 + t216;
        double t249 = x[3] * x[7];
        double t256 = -2.0f * t213 - 2.0f * t217 - 2.0f * t36 * t212 + t3 * t9 - t3 * t2 + t11 * t6 + t9 * t5 - t6 * t5 + t8 * t5 + t64 * t11 + t131 * t2 - t249 * t15 + t131 * t3 - t64 * t8 + t9 * t1 + t131 * t6 + t11 * t2;
        double t261 = t200 * t6;
        double t264 = x[4] * t138;
        double t267 = 2.0f * t198 * t3;
        double t268 = t25 * t119;
        double t269 = x[2] * t126;
        double t270 = x[6] * t126;
        double t272 = x[3] * x[1];
        double t274 = t74 * t93;
        double t275 = t37 * t48;
        double t277 = -t261 - t86 * t11 + t128 * t195 - t264 - t89 * t3 + t267 + t268 + t269 - t270 + t133 * t6 + t272 * t167 - t274 + t275 + t3 * t31;
        double t279 = x[3] * t113 * t2;
        double t282 = 2.0f * t133 * t3;
        double t283 = t25 * t58;
        double t284 = x[0] * t283;
        double t286 = 2.0f * t212 * t119;
        double t289 = x[1] * t131 * t24;
        double t294 = t11 * t141;
        double t295 = x[5] * t4;
        double t298 = 2.0f * t2 * t59;
        double t299 = t37 * t8;
        double t300 = t74 * t67;
        double t301 = t279 - t249 * t147 - t282 - t284 - t286 - t64 * t86 - t289 - t94 * t2 - t196 * t8 + x[7] * t1 * t53 + t294 - t295 * t195 + t298 - t299 + t300;
        double t303 = x[4] * t283;
        double t304 = t58 * t67;
        double t305 = t44 * t112;
        double t306 = x[7] * t4;
        double t307 = t306 * t195;
        double t308 = x[6] * t63;
        double t310 = t4 * t97;
        double t311 = t131 * t86;
        double t313 = x[2] * t103;
        double t314 = t3 * t141;
        double t315 = t10 * t141;
        double t316 = t272 * t164;
        double t319 = t303 - t304 + t305 + t307 + t308 - t295 * t36 - t310 - t311 + t41 * t195 - t313 - t314 + t315 + t316 - x[3] * t44 * t48;
        double t320 = t209 * t164;
        double t321 = t97 * t2;
        double t323 = t200 * t2;
        double t325 = t8 * t159;
        double t327 = t59 * t5;
        double t328 = t104 * t75;
        double t330 = t64 * t59;
        double t332 = t68 * t2;
        double t333 = x[6] * t11;
        double t334 = t333 * t6;
        double t336 = t333 * t2;
        double t338 = -t320 - t321 - t104 * t159 + t323 + t86 * t5 + t325 + t104 * t45 - t327 + t328 + t131 * t141 + t330 + t94 * t6 - t332 + 2.0f * t334 - 2.0f * t336;
        double t346 = x[2] * t44;
        double t349 = -t19 + t22 + t28 + t33 + t39 - t43 - t59 * t76 - x[1] * t2 * x[2] * t11 - t346 * t157 + t51 - t56 + t65 - t69 - t54 * t76;
        double t350 = x[0] * t16;
        double t356 = t80 + x[6] * t350 * t11 + t84 - t87 + t194 * t35 + t92 - t95 - t350 * t75 + t83 * t141 - t98 + t101 - t111 + t121 + t122 + t127;
        double t361 = -t129 - t139 - t142 + t143 - t149 + t26 * t150 - t151 + t154 - t158 - t162 + t166 + t168 + t196 * t128 - t223 * t214;
        double t368 = t30 * t16;
        double t372 = t133 * t93 - t170 - t173 - t176 - t128 * t59 + t182 - t198 * t83 + t184 - t196 * t232 - t10 * t126 + t97 * t67 - t186 + t368 * t9 + t8 * t110 + t295 * t97;
        double t394 = t16 * t75 - x[1] * t63 - x[5] * t283 + 2.0f * x[1] * t107 * t8 + x[5] * t368 + x[1] * t283 - t71 * t45 + x[7] * t126 + t71 * t97 - t71 * t75 - t30 * x[5] * t25 + t60 * t59 + 2.0f * t55 * t6 + t71 * t86;
        double t419 = -2.0f * t11 * t46 - t71 * t59 - 2.0f * x[3] * t107 * t8 + t16 * t45 + x[3] * t110 - t16 * t159 - x[7] * t368 - 2.0f * t5 * t232 + 2.0f * t64 * t20 + 2.0f * x[1] * t11 * t10 - 2.0f * t183 * t11 + x[1] * t225 + 2.0f * t32 * t11 + 2.0f * t76 * t8;
        double t445 = x[3] * t218;
        double t448 = x[3] * t209;
        double t451 = 2.0f * t232 * t2 + 2.0f * x[1] * t60 * t24 - 2.0f * t6 * t76 - 2.0f * t46 * t2 - 2.0f * t120 * t24 - 2.0f * t60 * x[7] * t24 - 2.0f * t64 * t67 + 2.0f * t148 * t15 + 2.0f * t10 * t67 - x[3] * t103 + x[1] * t103 - 2.0f * t306 * t2 - 2.0f * t445 * t24 + 2.0f * t448 * t24;
        double t452 = x[3] * t212;
        double t477 = 2.0f * t452 * t24 - 2.0f * t452 * t15 - 2.0f * t448 * t15 - 2.0f * t72 * t3 - 2.0f * t37 * t212 - 2.0f * t55 * t9 - x[1] * t108 - x[3] * t126 + 2.0f * t76 * t9 + 2.0f * t3 * t232 + 2.0f * t128 * t48 + 2.0f * t445 * t15 - x[3] * t225 + x[3] * t138;
        double t494 = -x[4] * t71 * t74 - t16 * t164 - 2.0f * t261 + 2.0f * t50 * t24 - x[4] * t108 - t264 + t16 * t167 + x[4] * t71 * t58 - 2.0f * x[4] * t212 * t24 + t267 + t268 + t269 - t270 - t274;
        double t512 = t71 * t44 * x[6] + 2.0f * t275 + 2.0f * x[7] * t15 * t6 + 2.0f * t279 - t282 - t284 - t71 * t346 - t286 - t71 * x[6] * t58 - 2.0f * t289 - t16 * t147 + t71 * t74 * x[2] + 2.0f * t294 + t16 * x[4] * t30;
        double t521 = t298 - 2.0f * t299 + t300 + t303 - t304 + t305 + 2.0f * t307 + t308 + x[6] * t161 - 2.0f * t310 - 2.0f * t311 - t313 - 2.0f * t314 + 2.0f * t315;
        double t525 = t316 - t320 - t321 + t97 * t5 + t323 + t325 - t327 + t328 + t330 - t332 + t334 - t336 - t10 * t159 - t183 * t195;

        h[0] = -t191 * (-x[7] + x[1]) * t12 * (t1 - t2 - t3 - t4 + t5 + t6);
        h[1] = t191 * (-x[3] + x[1]) * (t236 + t256);
        h[2] = t191 * (t394 + t419 + t451 + t477);
        h[3] = t191 * t203 * t12;
        h[4] = -t191 * (t277 + t301 + t319 + t338);
        h[5] = -t191 * (t494 + t512 + t521 + 2.0f * t525);
        h[6] = -t191 * t220 * t12;
        h[7] = t191 * (t349 + t356 + t361 + t372);
        h[8] = 1.0;

        return det;
}

double nx_homography_transfer_error_fwd(const double *h, int n_corr,
                                        const struct NXPointMatch2D *corr_list)
{
        double e = 0.0;
        for (int i = 0; i < n_corr; ++i) {
                const struct NXPointMatch2D *pm = corr_list + i;
                if (pm->is_inlier) {
                        float xp[2];
                        nx_homography_map(&xp[0], &pm->x[0], h);
                        double dx = xp[0] - pm->xp[0];
                        double dy = xp[1] - pm->xp[1];
                        double e_i = sqrt(dx*dx + dy*dy) / pm->sigma_xp;
                        e += e_i;
                }
        }
        return e / n_corr;
}

double nx_homography_transfer_error_bwd(const double *h, int n_corr,
                                        const struct NXPointMatch2D *corr_list)
{
        double h_inv[9];
        nx_dmat3_inv(&h_inv[0], h);

        double e = 0.0;
        for (int i = 0; i < n_corr; ++i) {
                const struct NXPointMatch2D *pm = corr_list + i;
                if (pm->is_inlier) {
                        float x[2];
                        nx_homography_map(&x[0], &pm->xp[0], &h_inv[0]);
                        double dx = x[0] - pm->x[0];
                        double dy = x[1] - pm->x[1];
                        e += sqrt(dx*dx + dy*dy) / pm->sigma_x;
                }
        }
        return e / n_corr;
}

double nx_homography_transfer_error_sym(const double *h, int n_corr,
                                        const struct NXPointMatch2D *corr_list)
{
        return 0.5 * (nx_homography_transfer_error_fwd(h, n_corr, corr_list)
                      + nx_homography_transfer_error_bwd(h, n_corr, corr_list));
}
