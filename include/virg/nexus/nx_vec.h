/**
 * @file nx_vec.h
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
#ifndef VIRG_NEXUS_NX_VEC_H
#define VIRG_NEXUS_NX_VEC_H

#include <math.h>
#include <float.h>

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_assert.h"

__NX_BEGIN_DECL

static inline float nx_svec_norm_sq(int n, const float *v);
static inline float nx_svec_norm   (int n, const float *v);
static inline float nx_svec_to_unit(int n, float *v);
static inline int   nx_svec_max_idx(int n, const float *v);
static inline float nx_svec_max_val(int n, const float *v);

/*
 * -----------------------------------------------------------------------------
 *                                   Definitions
 * -----------------------------------------------------------------------------
 */
static inline float nx_svec_norm_sq(int n, const float *v)
{
        NX_ASSERT_PTR(v);

        float nrm_sq = 0.0f;
        for (int i = 0; i < n; ++i) {
                nrm_sq += v[i]*v[i];
        }
        return nrm_sq;
}


static inline float nx_svec_norm(int n, const float *v)
{
        return sqrt(nx_svec_norm_sq(n, v));
}

static inline float nx_svec_to_unit(int n, float *v)
{
        NX_ASSERT_PTR(v);

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

        float max_val = -FLT_MAX;
        for (int i = 0; i < n; ++i) {
                if (v[i] > max_val) {
                        max_val = v[i];
                }
        }
        return max_val;
}

__NX_END_DECL

#endif
