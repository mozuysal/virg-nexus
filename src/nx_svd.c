/**
 * @file nx_svd.c
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
#include "virg/nexus/nx_svd.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_log.h"

/* --------------------------------- LAPACK SVD ----------------------------------- */
extern void dgesvd_(char* jobu, char* jobvt, int* m, int* n,
                    double* A, int* lda,
                    double* S, double* U, int* ldu, double* VT, int* ldvt,
                    double* work, int* lwork,
                    int* info);

extern void sgesvd_(char* jobu, char* jobvt, int* m, int* n,
                    float* A, int* lda,
                    float* S, float* U, int* ldu, float* VT, int* ldvt,
                    float* work, int* lwork,
                    int* info);


/*
 * -----------------------------------------------------------------------------
 *                                Single-Precision
 * -----------------------------------------------------------------------------
 */
static int nx_sgesvd_query_lwork(char jobu, char jobvt, int m, int n)
{
        float work = 0;
        int lwork = -1;

        float* A = NULL;
        float* S = NULL;
        float* U = NULL;
        float* Vt = NULL;

        int ldA = m;
        int ldU = m;
        int ldVt = n;

        int info;
        sgesvd_(&jobu, &jobvt, &m, &n, A, &ldA, S, U, &ldU, Vt, &ldVt, &work, &lwork, &info);

        if(!info) {
                return (int)work;
        }

        NX_FATAL(NX_LOG_TAG, "Error querying for SVD work size!");
}

static void nx_ssvd(char jobU, char jobVt, int m, int n,
                   float* A, int ldA, float* S, float* U, int ldU, float* Vt, int ldVt)
{
        int  lwork = nx_sgesvd_query_lwork(jobU, jobVt, m, n);
        float* work  = NX_NEW(lwork, float);

        int info;
        sgesvd_(&jobU, &jobVt, &m, &n, A, &ldA, S, U, &ldU, Vt, &ldVt, work, &lwork, &info);

        nx_free(work);
}

void nx_ssvd_only_s(float *S, int m, int n, float *A, int ldA)
{
        char jobU  = 'N';
        char jobVt = 'N';
        nx_ssvd(jobU, jobVt, m, n, A, ldA, S, NULL, m, NULL, n);
}

void nx_ssvd_us(float *U, int ldU, float *S, int m, int n, float *A, int ldA)
{
        char jobU  = 'A';
        char jobVt = 'N';
        nx_ssvd(jobU, jobVt, m, n, A, ldA, S, U, ldU, NULL, n);
}

void nx_ssvd_svt(float *S, float *Vt, int ldVt, int m, int n, float *A, int ldA)
{
        char jobU  = 'N';
        char jobVt = 'A';
        nx_ssvd(jobU, jobVt, m, n, A, ldA, S, NULL, m, Vt, ldVt);
}

void nx_ssvd_usvt(float *U, int ldU, float *S, float *Vt, int ldVt, int m, int n, float *A, int ldA)
{
        char jobU  = 'A';
        char jobVt = 'A';
        nx_ssvd(jobU, jobVt, m, n, A, ldA, S, U, ldU, Vt, ldVt);
}

/*
 * -----------------------------------------------------------------------------
 *                                Double-Precision
 * -----------------------------------------------------------------------------
 */
static int nx_dgesvd_query_lwork(char jobu, char jobvt, int m, int n)
{
        double work = 0;
        int lwork = -1;

        double* A = NULL;
        double* S = NULL;
        double* U = NULL;
        double* Vt = NULL;

        int ldA = m;
        int ldU = m;
        int ldVt = n;

        int info;
        dgesvd_(&jobu, &jobvt, &m, &n, A, &ldA, S, U, &ldU, Vt, &ldVt, &work, &lwork, &info);

        if(!info) {
                return (int)work;
        }

        NX_FATAL(NX_LOG_TAG, "Error querying for SVD work size!");
}

static void nx_dsvd(char jobU, char jobVt, int m, int n,
                   double* A, int ldA, double* S, double* U, int ldU, double* Vt, int ldVt)
{
        int  lwork = nx_dgesvd_query_lwork(jobU, jobVt, m, n);
        double* work  = NX_NEW(lwork, double);

        int info;
        dgesvd_(&jobU, &jobVt, &m, &n, A, &ldA, S, U, &ldU, Vt, &ldVt, work, &lwork, &info);

        nx_free(work);
}

void nx_dsvd_only_s(double *S, int m, int n, double *A, int ldA)
{
        char jobU  = 'N';
        char jobVt = 'N';
        nx_dsvd(jobU, jobVt, m, n, A, ldA, S, NULL, m, NULL, n);
}

void nx_dsvd_us(double *U, int ldU, double *S, int m, int n, double *A, int ldA)
{
        char jobU  = 'A';
        char jobVt = 'N';
        nx_dsvd(jobU, jobVt, m, n, A, ldA, S, U, ldU, NULL, n);
}

void nx_dsvd_svt(double *S, double *Vt, int ldVt, int m, int n, double *A, int ldA)
{
        char jobU  = 'N';
        char jobVt = 'A';
        nx_dsvd(jobU, jobVt, m, n, A, ldA, S, NULL, m, Vt, ldVt);
}

void nx_dsvd_usvt(double *U, int ldU, double *S, double *Vt, int ldVt, int m, int n, double *A, int ldA)
{
        char jobU  = 'A';
        char jobVt = 'A';
        nx_dsvd(jobU, jobVt, m, n, A, ldA, S, U, ldU, Vt, ldVt);
}
