/**
 * @file nx_svd.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2015 Mustafa Ozuysal. All rights reserved.
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
