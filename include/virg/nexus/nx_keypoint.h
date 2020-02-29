/**
 * @file nx_keypoint.h
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
