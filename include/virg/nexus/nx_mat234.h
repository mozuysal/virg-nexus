/**
 * @file nx_mat234.h
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
#ifndef VIRG_NEXUS_NX_MAT234_H
#define VIRG_NEXUS_NX_MAT234_H

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_svd.h"

__NX_BEGIN_DECL

static inline float  nx_smat3_inv(float *M3i, const float *M3);
static inline double nx_sdmat3_inv(float *M3i, const float *M3);
static inline void   nx_smat3_mul(float *C, const float *A, const float *B);

static inline double nx_dmat3_inv(double *M3i, const double *M3);
static inline void   nx_dmat3_mul(double *C, const double *A, const double *B);
static inline void   nx_dmat3_mul_ua(double *C, const double *U, const double *A);
static inline void   nx_dmat3_eye(double *A);
static inline double nx_dmat3_det(const double *A);
static inline double nx_dmat3_xptFx(const double *F, const double *x, const double* xp);
static inline void   nx_dmat3_print(const double *A, const char *label);
static inline void   nx_dmat3_svd(const double *A, double *U, double *S, double *Vt);
static inline double nx_dmat3_frob_norm(const double *A);
static inline void   nx_dmat3_sub      (double *A, const double *B);

static inline void nx_dmat34_print(const double *A, const char *label);

/*
 * -----------------------------------------------------------------------------
 *                                   Definitions
 * -----------------------------------------------------------------------------
 */
static inline float nx_smat3_inv(float *M3i, const float *M3)
{
        const float t4  = M3[0] * M3[4];
        const float t6  = M3[0] * M3[7];
        const float t8  = M3[3] * M3[1];
        const float t10 = M3[6] * M3[1];
        const float t12 = M3[3] * M3[2];
        const float t14 = M3[6] * M3[2];

        float detM3 = t4 * M3[8] - t6 * M3[5] - t8 * M3[8] + t10 * M3[5] + t12 * M3[7] - t14 * M3[4];

        if(fabs(detM3) > 0.0f) {
                const float t17 = 1.0 / detM3;
                M3i[0] = (M3[4] * M3[8] - M3[7] * M3[5]) * t17;
                M3i[1] = -(M3[1] * M3[8] - M3[7] * M3[2]) * t17;
                M3i[2] = (M3[1] * M3[5] - M3[4] * M3[2]) * t17;
                M3i[3] = -(M3[3] * M3[8] - M3[6] * M3[5]) * t17;
                M3i[4] = (M3[0] * M3[8] - t14) * t17;
                M3i[5] = -(M3[0] * M3[5] - t12) * t17;
                M3i[6] = (M3[3] * M3[7] - M3[6] * M3[4]) * t17;
                M3i[7] = -(t6 - t10) * t17;
                M3i[8] = (t4 - t8) * t17;
        /* } else { */
                /* nx_warning(NX_LOG_TAG, "Zero determinant in matrix3 inversion!"); */
        }

        return detM3;
}

static inline double nx_sdmat3_inv(float *M3i, const float *M3)
{
        double M3d[9];
        double M3di[9];
        double detM3d;

        for (int i = 0; i < 9; ++i)
                M3d[i] = M3[i];

        detM3d = nx_dmat3_inv(M3di, M3d);

        for (int i = 0; i < 9; ++i)
                M3i[i] = M3di[i];

        return detM3d;

}

static inline void nx_smat3_mul(float *C, const float *A, const float *B)
{
        C[0] = A[0]*B[0]+A[3]*B[1]+A[6]*B[2];
        C[1] = A[1]*B[0]+A[4]*B[1]+A[7]*B[2];
        C[2] = A[2]*B[0]+A[5]*B[1]+A[8]*B[2];

        C[3] = A[0]*B[3]+A[3]*B[4]+A[6]*B[5];
        C[4] = A[1]*B[3]+A[4]*B[4]+A[7]*B[5];
        C[5] = A[2]*B[3]+A[5]*B[4]+A[8]*B[5];

        C[6] = A[0]*B[6]+A[3]*B[7]+A[6]*B[8];
        C[7] = A[1]*B[6]+A[4]*B[7]+A[7]*B[8];
        C[8] = A[2]*B[6]+A[5]*B[7]+A[8]*B[8];
}

static inline double nx_dmat3_inv(double *M3i, const double *M3)
{
        const double t4  = M3[0] * M3[4];
        const double t6  = M3[0] * M3[7];
        const double t8  = M3[3] * M3[1];
        const double t10 = M3[6] * M3[1];
        const double t12 = M3[3] * M3[2];
        const double t14 = M3[6] * M3[2];

        double detM3 = t4 * M3[8] - t6 * M3[5] - t8 * M3[8] + t10 * M3[5] + t12 * M3[7] - t14 * M3[4];

        if(fabs(detM3) > 0.0) {
                const double t17 = 1.0 / detM3;
                M3i[0] = (M3[4] * M3[8] - M3[7] * M3[5]) * t17;
                M3i[1] = -(M3[1] * M3[8] - M3[7] * M3[2]) * t17;
                M3i[2] = (M3[1] * M3[5] - M3[4] * M3[2]) * t17;
                M3i[3] = -(M3[3] * M3[8] - M3[6] * M3[5]) * t17;
                M3i[4] = (M3[0] * M3[8] - t14) * t17;
                M3i[5] = -(M3[0] * M3[5] - t12) * t17;
                M3i[6] = (M3[3] * M3[7] - M3[6] * M3[4]) * t17;
                M3i[7] = -(t6 - t10) * t17;
                M3i[8] = (t4 - t8) * t17;
        /* } else { */
                /* nx_warning(NX_LOG_TAG, "Zero determinant in matrix3 inversion!"); */
        }

        return detM3;
}

static inline void nx_dmat3_mul(double *C, const double *A, const double *B)
{
        C[0] = A[0]*B[0]+A[3]*B[1]+A[6]*B[2];
        C[1] = A[1]*B[0]+A[4]*B[1]+A[7]*B[2];
        C[2] = A[2]*B[0]+A[5]*B[1]+A[8]*B[2];

        C[3] = A[0]*B[3]+A[3]*B[4]+A[6]*B[5];
        C[4] = A[1]*B[3]+A[4]*B[4]+A[7]*B[5];
        C[5] = A[2]*B[3]+A[5]*B[4]+A[8]*B[5];

        C[6] = A[0]*B[6]+A[3]*B[7]+A[6]*B[8];
        C[7] = A[1]*B[6]+A[4]*B[7]+A[7]*B[8];
        C[8] = A[2]*B[6]+A[5]*B[7]+A[8]*B[8];
}

static inline void nx_dmat3_mul_ua(double *C, const double *U, const double *A)
{
        C[0] = U[0]*A[0]+U[3]*A[1]+U[6]*A[2];
        C[1] = U[4]*A[1]+U[7]*A[2];
        C[2] = U[8]*A[2];

        C[3] = U[0]*A[3]+U[3]*A[4]+U[6]*A[5];
        C[4] = U[4]*A[4]+U[7]*A[5];
        C[5] = U[8]*A[5];

        C[6] = U[0]*A[6]+U[3]*A[7]+U[6]*A[8];
        C[7] = U[4]*A[7]+U[7]*A[8];
        C[8] = U[8]*A[8];
}

static inline void nx_dmat3_eye(double *A)
{
        memset(A, 0, 9*sizeof(*A));
        A[0] = 1.0;
        A[4] = 1.0;
        A[8] = 1.0;
}

static inline double nx_dmat3_det(const double *A)
{
        return A[0]*(A[4]*A[8]-A[5]*A[7])
                - A[3]*(A[1]*A[8]-A[2]*A[7])
                + A[6]*(A[1]*A[5]-A[2]*A[4]);
}

static inline double nx_dmat3_xptFx(const double *F, const double *x,
                                    const double* xp)
{
        double r = 0.0;

        r += xp[0] * (x[0]*F[0] + x[1]*F[3] + F[6]);
        r += xp[1] * (x[0]*F[1] + x[1]*F[4] + F[7]);
        r +=   1.0 * (x[0]*F[2] + x[1]*F[5] + F[8]);

        return r;
}

static inline void nx_dmat3_print(const double *A, const char *label)
{
        if (label)
                printf("%s\n---------\n", label);

        printf("[ %10.4f%10.4f%10.4f ]\n", A[0], A[3], A[6]);
        printf("[ %10.4f%10.4f%10.4f ]\n", A[1], A[4], A[7]);
        printf("[ %10.4f%10.4f%10.4f ]\n", A[2], A[5], A[8]);
}

static inline void nx_dmat3_svd(const double *A, double *U, double *S, double *Vt)
{
        double AA[9] = { A[0], A[1], A[2], A[3], A[4], A[5], A[6], A[7], A[8] };
        nx_dsvd_usvt(U, 3, S, Vt, 3, 3, 3, AA, 3);
}

static inline double nx_dmat3_frob_norm(const double *A)
{
        double sq_sum = 0.0;
        for (int i = 0; i < 9; ++i)
                sq_sum += A[i]*A[i];
        return sqrt(sq_sum);
}

static inline void nx_dmat3_sub(double *A, const double *B)
{
        for (int i = 0; i < 9; ++i)
                A[i] -= B[i];
}


static inline void nx_dmat34_print(const double *A, const char *label)
{
        if (label)
                printf("%s\n---------\n", label);

        printf("[ %10.4f%10.4f%10.4f|%10.4f ]\n", A[0], A[3], A[6], A[9]);
        printf("[ %10.4f%10.4f%10.4f|%10.4f ]\n", A[1], A[4], A[7], A[10]);
        printf("[ %10.4f%10.4f%10.4f|%10.4f ]\n", A[2], A[5], A[8], A[11]);
}


__NX_END_DECL

#endif
