/**
 * @file nx_vec234.h
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
 * You should have received a copy of the GNU General Public License along with
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_VEC234_H
#define VIRG_NEXUS_NX_VEC234_H

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

static inline float nx_svec3_norm_sq(const float *v);
static inline float nx_svec3_dist_sq(const float *v0, const float *v1);
static inline float nx_svec3_dot    (const float *v0, const float *v1);

static inline float nx_svec4h_to_euclidean(float *v);

static inline void   nx_dvec3_copy  (double *v0, const double *v1);
static inline void   nx_dvec3_cross (double *c, const double *a, const double *b);
static inline double nx_dvec3_dot    (const double *v0, const double *v1);
static inline double nx_dvec3_norm_sq(const double *v);
static inline double nx_dvec3_norm   (const double *v);
static inline double nx_dvec3_to_unit(double *v);
static inline void   nx_dvec3_print  (const double *v, const char *label);

/*
 * -----------------------------------------------------------------------------
 *                                   Definitions
 * -----------------------------------------------------------------------------
 */

static inline float nx_svec3_norm_sq(const float *v)
{
        return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
}

static inline float nx_svec3_dist_sq(const float *v0, const float *v1)
{
        float d0 = v0[0] - v1[0];
        float d1 = v0[1] - v1[1];
        float d2 = v0[2] - v1[2];

        return d0*d0 + d1*d1 + d2*d2;
}

static inline float nx_svec3_dot(const float *v0, const float *v1)
{
        return v0[0]*v1[0] + v0[1]*v1[1] + v0[2]*v1[2];
}

static inline float nx_svec4h_to_euclidean(float *v)
{
        float w = v[3];
        v[0] /= w;
        v[1] /= w;
        v[2] /= w;
        v[3] = 1.0f;

        return w;
}

static inline void nx_dvec3_copy(double *v0, const double *v1)
{
        memcpy(v0, v1, 3*sizeof(*v0));
}

static inline void nx_dvec3_cross(double *c, const double *a, const double *b)
{
        c[0] = a[1]*b[2] - a[2]*b[1];
        c[1] = a[2]*b[0] - a[0]*b[2];
        c[2] = a[0]*b[1] - a[1]*b[0];
}

static inline double nx_dvec3_dot(const double *v0, const double *v1)
{
        return v0[0]*v1[0] + v0[1]*v1[1] + v0[2]*v1[2];
}

static inline double nx_dvec3_norm_sq(const double *v)
{
        return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
}

static inline double nx_dvec3_norm(const double *v)
{
        return sqrt(nx_dvec3_norm_sq(v));
}

static inline double nx_dvec3_to_unit(double *v)
{
        double n = nx_dvec3_norm(v);

        if (n != 0.0) {
                v[0] /= n;
                v[1] /= n;
                v[2] /= n;
        }

        return n;
}

static inline void nx_dvec3_print(const double *v, const char *label)
{
        printf("%s = [ %10.4f%10.4f%10.4f ]\n", label, v[0], v[1], v[2]);
}

__NX_END_DECL

#endif
