/**
 * @file nx_keypoint.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_KEYPOINT_H
#define VIRG_NEXUS_NX_KEYPOINT_H

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

struct NXKeypoint {
        int x;
        int y;

        float xs;
        float ys;

        int level;
        float scale;
        float sigma;

        float score;
        float ori; // in radians

        long id;
};

static inline int nx_keypoint_x0(struct NXKeypoint *key)
{
        return key->x * key->scale;
}

static inline int nx_keypoint_y0(struct NXKeypoint *key)
{
        return key->y * key->scale;
}

static inline float nx_keypoint_xs0(struct NXKeypoint *key)
{
        return key->xs * key->scale;
}

static inline float nx_keypoint_ys0(struct NXKeypoint *key)
{
        return key->ys * key->scale;
}

__NX_END_DECL

#endif
