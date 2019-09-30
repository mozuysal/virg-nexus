/**
 * @file nx_svd.h
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
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_SVD_H
#define VIRG_NEXUS_NX_SVD_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

void nx_ssvd_only_s(float *S, int m, int n, float *A, int ldA);
void nx_ssvd_us(float *U, int ldU, float *S, int m, int n, float *A, int ldA);
void nx_ssvd_svt(float *S, float *Vt, int ldVt, int m, int n, float *A, int ldA);
void nx_ssvd_usvt(float *U, int ldU, float *S, float *Vt, int ldVt, int m, int n, float *A, int ldA);

void nx_dsvd_only_s(double *S, int m, int n, double *A, int ldA);
void nx_dsvd_us(double *U, int ldU, double *S, int m, int n, double *A, int ldA);
void nx_dsvd_svt(double *S, double *Vt, int ldVt, int m, int n, double *A, int ldA);
void nx_dsvd_usvt(double *U, int ldU, double *S, double *Vt, int ldVt, int m, int n, double *A, int ldA);

__NX_END_DECL

#endif
