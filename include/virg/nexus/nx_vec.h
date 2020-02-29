/**
 * @file nx_vec.h
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
 * VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_VEC_H
#define VIRG_NEXUS_NX_VEC_H

#include <math.h>
#include <float.h>
#include <limits.h>
#include <string.h>

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_assert.h"

__NX_BEGIN_DECL

static inline void   nx_dvec_set_zero(int n, double *v);
static inline void   nx_dvec_set     (int n, double *v, double value);
static inline double nx_dvec_norm_sq (int n, const double *v);
static inline double nx_dvec_norm    (int n, const double *v);
static inline void   nx_dvec_axpy    (int n, double *y, double a, const double *x);

static inline void  nx_svec_set_zero(int n, float *v);
static inline float nx_svec_norm_sq (int n, const float *v);
static inline float nx_svec_norm    (int n, const float *v);
static inline float nx_svec_to_unit (int n, float *v);
static inline int   nx_svec_max_idx (int n, const float *v);
static inline float nx_svec_max_val (int n, const float *v);

static inline int nx_ucvec_dist_sq(int n, const uchar *v0, const uchar *v1);

static inline void nx_ivec_set_zero(int n, int *v);
static inline int  nx_ivec_max_idx (int n, const int *v);

/*
 * -----------------------------------------------------------------------------
 *                                   Definitions
 * -----------------------------------------------------------------------------
 */
static inline void nx_dvec_set_zero(int n, double *v)
{
        NX_ASSERT_PTR(v);
        NX_ASSERT(n >= 0);

        memset(v, 0, n * sizeof(*v));
}

static inline void nx_dvec_set(int n, double *v, double value)
{
        NX_ASSERT_PTR(v);
        NX_ASSERT(n >= 0);

        for (int i = 0; i < n; ++i) {
                v[i] = value;
        }
}

static inline double nx_dvec_norm_sq(int n, const double *v)
{
        NX_ASSERT_PTR(v);
        NX_ASSERT(n >= 0);

        double nrm_sq = 0.0;
        for (int i = 0; i < n; ++i) {
                nrm_sq += v[i]*v[i];
        }
        return nrm_sq;
}

static inline double nx_dvec_norm(int n, const double *v)
{
        NX_ASSERT_PTR(v);
        NX_ASSERT(n >= 0);

        return sqrt(nx_dvec_norm_sq(n, v));
}

static inline void nx_dvec_axpy(int n, double *y, double a, const double *x)
{
        NX_ASSERT_PTR(x);
        NX_ASSERT_PTR(y);
        NX_ASSERT(n >= 0);

        for (int i = 0; i < n; ++i) {
                y[i] += a * x[i];
        }
}

static inline void nx_svec_set_zero(int n, float *v)
{
        NX_ASSERT_PTR(v);
        NX_ASSERT(n >= 0);

        memset(v, 0, n * sizeof(*v));
}

static inline float nx_svec_norm_sq(int n, const float *v)
{
        NX_ASSERT_PTR(v);
        NX_ASSERT(n >= 0);

        float nrm_sq = 0.0f;
        for (int i = 0; i < n; ++i) {
                nrm_sq += v[i]*v[i];
        }
        return nrm_sq;
}


static inline float nx_svec_norm(int n, const float *v)
{
        NX_ASSERT_PTR(v);
        NX_ASSERT(n >= 0);

        return sqrtf(nx_svec_norm_sq(n, v));
}

static inline float nx_svec_to_unit(int n, float *v)
{
        NX_ASSERT_PTR(v);
        NX_ASSERT(n >= 0);

        float nrm = nx_svec_norm(n, v);

        if (nrm != 0.0f) {
                for (int i = 0; i < n; ++i) {
                        v[i] /= nrm;
                }
        } else {
                NX_WARNING(NX_LOG_TAG, "Failed to normalize svec, norm is zero!");
        }

        return nrm;
}

static inline int nx_svec_max_idx(int n, const float *v)
{
        NX_ASSERT_PTR(v);
        NX_ASSERT(n >= 0);

        int max_idx = -1;
        float max_val = -FLT_MAX;
        for (int i = 0; i < n; ++i) {
                if (v[i] > max_val) {
                        max_idx = i;
                        max_val = v[i];
                }
        }
        return max_idx;
}

static inline float nx_svec_max_val(int n, const float *v)
{
        NX_ASSERT_PTR(v);
        NX_ASSERT(n >= 0);

        float max_val = -FLT_MAX;
        for (int i = 0; i < n; ++i) {
                if (v[i] > max_val) {
                        max_val = v[i];
                }
        }
        return max_val;
}

static inline int nx_ucvec_dist_sq(int n, const uchar *v0, const uchar *v1)
{
        NX_ASSERT_PTR(v0);
        NX_ASSERT_PTR(v1);
        NX_ASSERT(n >= 0);

        int d_sq = 0;
        for (int i = 0; i < n; ++i) {
                int d = v0[i] - (int)v1[i];
                d_sq += d*d;
        }

        return d_sq;
}

static inline void nx_ivec_set_zero(int n, int *v)
{
        NX_ASSERT_PTR(v);
        NX_ASSERT(n >= 0);

        memset(v, 0, n * sizeof(*v));
}

static inline int nx_ivec_max_idx(int n, const int *v)
{
        NX_ASSERT_PTR(v);
        NX_ASSERT(n >= 0);

        int max_idx = -1;
        int max_val = INT_MIN;
        for (int i = 0; i < n; ++i) {
                if (v[i] > max_val) {
                        max_idx = i;
                        max_val = v[i];
                }
        }
        return max_idx;
}


__NX_END_DECL

#endif
