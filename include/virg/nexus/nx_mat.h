/**
 * @file nx_mat.h
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
#ifndef VIRG_NEXUS_NX_MAT_H
#define VIRG_NEXUS_NX_MAT_H

#include <stdio.h>

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_vec.h"
#include "virg/nexus/nx_bit_ops.h"

__NX_BEGIN_DECL

static inline void nx_dmat_set_zero(int m, int n, double *A, int ldA);
static inline void nx_dmat_set     (int m, int n, double *A,
                                    int ldA, double value);
static inline void nx_dmat_mul     (int m, int n, int k,
                                    const double *A, int ldA,
                                    const double *B, int ldB,
                                    double *C, int ldC);
static inline void nx_dmat_transpose(int m, int n, double *A);

void nx_dmat_xwrite(FILE *stream, int m, int n, const double *A);
double *nx_dmat_xread(FILE *stream, int *m, int *n);

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

static inline void nx_dmat_mul(int m, int n, int k,
                               const double *A, int ldA,
                               const double *B, int ldB,
                               double *C, int ldC)
{
        for (int c = 0; c < n; ++c) {
                for (int r = 0; r < m; ++r) {
                        C[c * ldC + r] = 0;
                        for (int j = 0; j < k; ++j)
                                C[c * ldC + r] += A[j * ldA + r]
                                        * B[c * ldB + j];
                }
        }
}

static inline void nx_dmat_transpose(int m, int n, double *A)
{
        NX_ASSERT(m > 0);
        NX_ASSERT(n > 0);
        NX_ASSERT_PTR(A);

        size_t q = m * n;
        uint8_t *is_moved = nx_bit_array_new(q);
        nx_bit_array_set_zero(q, is_moved);
        size_t i = 1; // skip 0 since it stays where it is.
        while (i < (q - 1)) {
                if (!nx_bit_array_get(is_moved, i)) {
                        // start cycle
                        size_t start = i;
                        size_t idx0 = i;
                        size_t idx1 = (i * n) % (q - 1);
                        // skip if item i stays at the same position
                        if (idx0 != idx1) {
                                // move elements and mark them until cycle ends
                                double t0 = A[idx0];
                                double t1;
                                do {
                                        t1 = A[idx1];
                                        A[idx1] = t0;
                                        t0 = t1;
                                        nx_bit_array_set(is_moved, idx0,
                                                         NX_TRUE);
                                        idx0 = idx1;
                                        idx1 = (idx1 * n) % (q - 1);
                                } while (idx0 != start);
                        }
                }
                ++i;
        }

        nx_free(is_moved);
}

__NX_END_DECL

#endif
