/**
 * @file nx_point_match_2d.h
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
 * along with VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_POINT_MATCH_2D_H
#define VIRG_NEXUS_NX_POINT_MATCH_2D_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_keypoint.h"

__NX_BEGIN_DECL

struct NXPointMatch2D {
        float x[2];
        float xp[2];
        float match_cost;
        float sigma_x;
        float sigma_xp;
        uint64_t id;
        uint64_t idp;
        NXBool is_inlier;
};

static inline struct NXPointMatch2D
nx_point_match_2d_from_keypoints(const struct NXKeypoint *k,
                                 const struct NXKeypoint *kp,
                                 float sigma0,
                                 float match_cost,
                                 NXBool is_inlier) {
        struct NXPointMatch2D pm2d;
        pm2d.x[0] = nx_keypoint_xs0(k);
        pm2d.x[1] = nx_keypoint_ys0(k);
        pm2d.xp[0] = nx_keypoint_xs0(kp);
        pm2d.xp[1] = nx_keypoint_ys0(kp);
        pm2d.match_cost = match_cost;
        pm2d.sigma_x = sigma0*k->scale;
        pm2d.sigma_xp = sigma0*kp->scale;
        pm2d.id = k->id;
        pm2d.idp = kp->id;
        pm2d.is_inlier = is_inlier;

        return pm2d;
}

static inline int
nx_point_match_2d_count_inliers(int n_corr,
                                const struct NXPointMatch2D *corr_list)
{
        int n_inliers = 0;
        for (int i = 0; i < n_corr; ++i)
                if (corr_list[i].is_inlier)
                        ++n_inliers;
        return n_inliers;
}

static inline int
nx_point_match_2d_cmp_match_cost(const void *m0, const void *m1)
{
        const struct NXPointMatch2D *match0 = (const struct NXPointMatch2D *)m0;
        const struct NXPointMatch2D *match1 = (const struct NXPointMatch2D *)m1;

        if (match0->match_cost > match1->match_cost)
                return +1;
        else if (match0->match_cost < match1->match_cost)
                return -1;
        else
                return 0;
}


__NX_END_DECL

#endif
