/**
 * @file nx_point_match_2d_stats.c
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
#include "virg/nexus/nx_point_match_2d_stats.h"

#include <string.h>
#include <math.h>

#include "virg/nexus/nx_alloc.h"

struct NXPointMatch2DStats *nx_point_match_2d_stats_new()
{
        struct NXPointMatch2DStats *stats = NX_NEW(1, struct NXPointMatch2DStats);
        nx_point_match_2d_stats_reset(stats);
        return stats;
}

void nx_point_match_2d_stats_free(struct NXPointMatch2DStats *stats)
{
        nx_free(stats);
}

void nx_point_match_2d_stats_reset(struct NXPointMatch2DStats *stats)
{
        stats->m[0]  = 0.0;
        stats->m[1]  = 0.0;
        stats->mp[0] = 0.0;
        stats->mp[1] = 0.0;
        stats->d     = 1.0;
        stats->dp    = 1.0;
}

void nx_point_match_2d_stats_normalize_matches(struct NXPointMatch2DStats *stats,
                                               int n_corr,
                                               struct NXPointMatch2D *corr_list)
{
        memset(stats, 0, sizeof(*stats));

        for (int i = 0; i < n_corr; ++i) {
                struct NXPointMatch2D *corr = corr_list + i;
                stats->m[0] += corr->x[0];
                stats->m[1] += corr->x[1];
                stats->mp[0] += corr->xp[0];
                stats->mp[1] += corr->xp[1];
        }

        stats->m[0] /= n_corr;
        stats->m[1] /= n_corr;
        stats->mp[0] /= n_corr;
        stats->mp[1] /= n_corr;

        for (int i = 0; i < n_corr; ++i) {
                struct NXPointMatch2D *corr = corr_list + i;
                corr->x[0] -= stats->m[0];
                corr->x[1] -= stats->m[1];
                corr->xp[0] -= stats->mp[0];
                corr->xp[1] -= stats->mp[1];

                stats->d += sqrt(corr->x[0]*corr->x[0] + corr->x[1]*corr->x[1]);
                stats->dp += sqrt(corr->xp[0]*corr->xp[0] + corr->xp[1]*corr->xp[1]);
        }

        stats->d /= n_corr;
        stats->dp /= n_corr;

        for (int i = 0; i < n_corr; ++i) {
                struct NXPointMatch2D *corr = corr_list + i;
                corr->x[0]     /= stats->d;
                corr->x[1]     /= stats->d;
                corr->xp[0]    /= stats->dp;
                corr->xp[1]    /= stats->dp;
                corr->sigma_x  /= stats->d;
                corr->sigma_xp /= stats->dp;
        }
}

void nx_point_match_2d_stats_denormalize_matches(const struct NXPointMatch2DStats *stats,
                                                 int n_corr,
                                                 struct NXPointMatch2D *corr_list)
{
        for (int i = 0; i < n_corr; ++i) {
                struct NXPointMatch2D *corr = corr_list + i;
                corr->x[0] = stats->d * corr->x[0] + stats->m[0];
                corr->x[1] = stats->d * corr->x[1] + stats->m[1];
                corr->xp[0] = stats->dp * corr->xp[0] + stats->mp[0];
                corr->xp[1] = stats->dp * corr->xp[1] + stats->mp[1];
                corr->sigma_x  *= stats->d;
                corr->sigma_xp *= stats->dp;
        }
}

void nx_point_match_2d_stats_denormalize_homography(const struct NXPointMatch2DStats *stats,
                                                    double *h)
{
        double sx  = 1.0 / stats->d;
        double sxp = 1.0 / stats->dp;

        double tx  = -stats->m[0]*sx;
        double ty  = -stats->m[1]*sx;
        double txp = -stats->mp[0]*sxp;
        double typ = -stats->mp[1]*sxp;

        double htemp[9];

        double t[4];
        t[0] = (h[0] - h[2]*txp) * stats->dp;
        t[1] = (h[1] - h[2]*typ) * stats->dp;
        t[2] = (h[3] - h[5]*txp) * stats->dp;
        t[3] = (h[4] - h[5]*typ) * stats->dp;

        htemp[0] = sx*t[0];
        htemp[1] = sx*t[1];
        htemp[2] = sx*h[2];

        htemp[3] = sx*t[2];
        htemp[4] = sx*t[3];
        htemp[5] = sx*h[5];

        htemp[6] = tx*t[0] + ty*t[2] + h[6]*stats->dp - h[8]*txp*stats->dp;
        htemp[7] = tx*t[1] + ty*t[3] + h[7]*stats->dp - h[8]*typ*stats->dp;
        htemp[8] = h[8] + tx*h[2] + ty*h[5];

        memcpy(h, htemp, 9*sizeof(*h));
}

void nx_point_match_2d_stats_denormalize_fundamental(const struct NXPointMatch2DStats *stats,
                                                     double *f)
{
        double sx  = 1.0 / stats->d;
        double sxp = 1.0 / stats->dp;

        double tx  = -stats->m[0]*sx;
        double ty  = -stats->m[1]*sx;
        double txp = -stats->mp[0]*sxp;
        double typ = -stats->mp[1]*sxp;

        double FT[9];
        FT[0] = f[0]*sx;
        FT[1] = f[1]*sx;
        FT[2] = f[2]*sx;
        FT[3] = f[3]*sx;
        FT[4] = f[4]*sx;
        FT[5] = f[5]*sx;
        FT[6] = f[0]*tx+f[3]*ty+f[6];
        FT[7] = f[1]*tx+f[4]*ty+f[7];
        FT[8] = f[2]*tx+f[5]*ty+f[8];

        f[0] = sxp*FT[0];
        f[1] = sxp*FT[1];
        f[2] = txp*FT[0] + typ*FT[1] + FT[2];
        f[3] = sxp*FT[3];
        f[4] = sxp*FT[4];
        f[5] = txp*FT[3] + typ*FT[4] + FT[5];
        f[6] = sxp*FT[6];
        f[7] = sxp*FT[7];
        f[8] = txp*FT[6] + typ*FT[7] + FT[8];
}
