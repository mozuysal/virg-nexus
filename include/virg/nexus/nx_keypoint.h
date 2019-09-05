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

#include <stdio.h>

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

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

        uint64_t id;
};

static inline int nx_keypoint_x0(const struct NXKeypoint *key)
{
        return key->x * key->scale;
}

static inline int nx_keypoint_y0(const struct NXKeypoint *key)
{
        return key->y * key->scale;
}

static inline float nx_keypoint_xs0(const struct NXKeypoint *key)
{
        return key->xs * key->scale;
}

static inline float nx_keypoint_ys0(const struct NXKeypoint *key)
{
        return key->ys * key->scale;
}

int nx_keypoint_write(const struct NXKeypoint *key, int n, FILE *stream);
int nx_keypoint_read(struct NXKeypoint *key, int n, FILE *stream);

void nx_keypoint_xwrite(const struct NXKeypoint *key, int n, FILE *stream);
void nx_keypoint_xread(struct NXKeypoint *key, int n, FILE *stream);

__NX_END_DECL

#endif
