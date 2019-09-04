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
        int id;
        int idp;
};

inline struct NXPointMatch2D
nx_point_match_2d_from_keypoints(int id,  const struct NXKeypoint *k,
                                 int idp, const struct NXKeypoint *kp,
                                 float sigma0,
                                 float match_cost) {
        struct NXPointMatch2D pm2d;
        pm2d.x[0] = nx_keypoint_x0(k);
        pm2d.x[1] = nx_keypoint_y0(k);
        pm2d.xp[0] = nx_keypoint_x0(kp);
        pm2d.xp[1] = nx_keypoint_y0(kp);
        pm2d.match_cost = match_cost;
        pm2d.sigma_x = sigma0*k->scale;
        pm2d.sigma_xp = sigma0*kp->scale;
        pm2d.id = id;
        pm2d.idp = idp;

        return pm2d;
}

__NX_END_DECL

#endif
