/**
 * @file nx_point_match_2d.c
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
#include "virg/nexus/nx_point_match_2d.h"

#include <string.h>
#include <math.h>

struct NXPointMatch2DStats
nx_point_match_2d_normalize(int n_corr,
                            struct NXPointMatch2D *corr_list)
{
        struct NXPointMatch2DStats stats;
        memset(&stats, 0, sizeof(stats));

        for (int i = 0; i < n_corr; ++i) {
                struct NXPointMatch2D *corr = corr_list + i;
                stats.m[0] += corr->x[0];
                stats.m[1] += corr->x[1];
                stats.mp[0] += corr->xp[0];
                stats.mp[1] += corr->xp[1];
        }

        stats.m[0] /= n_corr;
        stats.m[1] /= n_corr;
        stats.mp[0] /= n_corr;
        stats.mp[1] /= n_corr;

        for (int i = 0; i < n_corr; ++i) {
                struct NXPointMatch2D *corr = corr_list + i;
                corr->x[0] -= stats.m[0];
                corr->x[1] -= stats.m[1];
                corr->xp[0] -= stats.mp[0];
                corr->xp[1] -= stats.mp[1];

                stats.d += sqrt(corr->x[0]*corr->x[0] + corr->x[1]*corr->x[1]);
                stats.dp += sqrt(corr->xp[0]*corr->xp[0] + corr->xp[1]*corr->xp[1]);
        }

        stats.d /= n_corr;
        stats.dp /= n_corr;

        for (int i = 0; i < n_corr; ++i) {
                struct NXPointMatch2D *corr = corr_list + i;
                corr->x[0]     /= stats.d;
                corr->x[1]     /= stats.d;
                corr->xp[0]    /= stats.dp;
                corr->xp[1]    /= stats.dp;
                corr->sigma_x  /= stats.d;
                corr->sigma_xp /= stats.dp;
        }

        return stats;
}

void nx_point_match_2d_denormalize(int n_corr,
                                   struct NXPointMatch2D *corr_list,
                                   struct NXPointMatch2DStats stats)
{
        for (int i = 0; i < n_corr; ++i) {
                struct NXPointMatch2D *corr = corr_list + i;
                corr->x[0] = stats.d * corr->x[0] + stats.m[0];
                corr->x[1] = stats.d * corr->x[1] + stats.m[1];
                corr->xp[0] = stats.dp * corr->xp[0] + stats.mp[0];
                corr->xp[1] = stats.dp * corr->xp[1] + stats.mp[1];
                corr->sigma_x  *= stats.d;
                corr->sigma_xp *= stats.dp;
        }
}

