/**
 * @file nx_linear_solvers.c
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
 * VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_linear_solvers.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_log.h"

extern void dsysv_(char *uplo, int *n, int *nrhs, double *A, int *lda,
                   int *ipiv, double *B, int *ldb,
                   double *work, int *lwork, int *info);

NXBool nx_dsym_solver(enum NXSymmetricUpLo uplo, int n, int n_rhs,
                      double *A, int lda, double *B, int ldb, int *ipiv)
{
        NX_ASSERT_PTR(A);
        NX_ASSERT_PTR(B);
        NX_ASSERT_PTR(ipiv);

        NX_ASSERT(n >= 1);
        NX_ASSERT(n_rhs >= 1);
        NX_ASSERT(lda >= n);
        NX_ASSERT(ldb >= n);

        int info = 0;
        int lwork = n*n;
        double *work = NX_NEW_D(lwork);
        char f_uplo = uplo == NX_SYMMETRIC_UPPER ? 'U' : 'L';
        dsysv_(&f_uplo, &n, &n_rhs, A, &lda, ipiv, B, &ldb,
               work, &lwork, &info);

        nx_free(work);
        return info == 0;
}

