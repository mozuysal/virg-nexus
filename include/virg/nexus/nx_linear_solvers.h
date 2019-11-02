/**
 * @file nx_linear_solvers.h
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
#ifndef VIRG_NEXUS_NX_LINEAR_SOLVERS_H
#define VIRG_NEXUS_NX_LINEAR_SOLVERS_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

enum NXSymmetricUpLo {
        NX_SYMMETRIC_UPPER = 0,
        NX_SYMMETRIC_LOWER };

NXBool nx_dsym_solver(enum NXSymmetricUpLo uplo, int n, int n_rhs,
                      double *A, int lda, double *B, int ldb, int *ipiv);


__NX_END_DECL

#endif
