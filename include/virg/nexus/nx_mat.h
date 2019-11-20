/**
 * @file nx_mat.h
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
#ifndef VIRG_NEXUS_NX_MAT_H
#define VIRG_NEXUS_NX_MAT_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_vec.h"

__NX_BEGIN_DECL

static inline void nx_dmat_set_zero(int m, int n, double *A, int ldA);
static inline void nx_dmat_set     (int m, int n, double *A,
                                    int ldA, double value);


/* --------------------------------- Definitions ---------------------------------- */
static inline void nx_dmat_set_zero(int m, int n, double *A, int ldA)
{
        NX_ASSERT_PTR(A);
        NX_ASSERT(ldA >= m);

        for (int i = 0; i < n; ++i) {
                nx_dvec_set_zero(m, A + i*ldA);
        }
}

static inline void nx_dmat_set(int m, int n, double *A, int ldA, double value)
{
        NX_ASSERT_PTR(A);
        NX_ASSERT(ldA >= m);

        for (int i = 0; i < n; ++i) {
                nx_dvec_set(m, A + i * ldA, value);
        }
}

__NX_END_DECL

#endif
