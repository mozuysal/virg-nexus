/**
 * @file nx_point_match_2d.h
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

struct NXPointMatch2DStats {
        double m[2]; // mean point
        double mp[2]; // mean point

        double d; // distance to mean
        double dp; // distance to mean
};

struct NXPointMatch2DStats nx_point_match_2d_normalize(int n_corr,
                                                       struct NXPointMatch2D *corr_list);

void nx_point_match_2d_denormalize(int n_corr,
                                   struct NXPointMatch2D *corr_list,
                                   struct NXPointMatch2DStats stats);

__NX_END_DECL

#endif
