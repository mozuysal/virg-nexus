/**
 * @file nx_quaternion.c
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
#include "virg/nexus/nx_quaternion.h"

void nx_quaternion_from_R(double *q, const double *R)
{
        NX_ASSERT_PTR(q);
        NX_ASSERT_PTR(R);

        double d_sum = R[0] + R[4] + R[8];

        if (d_sum > 0) {
                double t = d_sum + 1;
                double s = 0.5 / sqrt(t);

                q[0] = s * t;
                q[3] = (R[1] - R[3]) * s;
                q[2] = (R[6] - R[2]) * s;
                q[1] = (R[5] - R[7]) * s;
        } else if (R[0] > R[4] && R[0] > R[8]) {
                double t = R[0] - R[4] - R[8] + 1;
                double s = 0.5 / sqrt(t);

                q[1] = s * t;
                q[2] = (R[1] + R[3]) * s;
                q[3] = (R[6] + R[2]) * s;
                q[0] = (R[5] - R[7]) * s;
        } else if (R[4] > R[8]) {
                double t = - R[0] + R[4] - R[8] + 1;
                double s = 0.5 / sqrt(t);

                q[2] = s * t;
                q[1] = (R[1] + R[3]) * s;
                q[0] = (R[6] - R[2]) * s;
                q[3] = (R[5] + R[7]) * s;
        } else {
                double t = - R[0] - R[4] + R[8] + 1;
                double s = 0.5 / sqrt(t);

                q[3] = s * t;
                q[0] = (R[1] - R[3]) * s;
                q[1] = (R[6] + R[2]) * s;
                q[2] = (R[5] + R[7]) * s;
        }
}

void nx_quaternion_to_R(const double *q, double *R)
{
        NX_ASSERT_PTR(q);
        NX_ASSERT_PTR(R);

        const double q1_2 = q[1] * q[1];
        const double q2_2 = q[2] * q[2];
        const double q3_2 = q[3] * q[3];

        const double q1_q2 = q[1] * q[2];
        const double q0_q3 = q[0] * q[3];
        const double q1_q3 = q[1] * q[3];
        const double q0_q2 = q[0] * q[2];
        const double q0_q1 = q[0] * q[1];
        const double q2_q3 = q[2] * q[3];

        R[0] = 1.0 - 2.0 * (q2_2 + q3_2);
        R[1] = 2.0 * (q1_q2 + q0_q3);
        R[2] = 2.0 * (q1_q3 - q0_q2);
        R[3] = 2.0 * (q1_q2 - q0_q3);
        R[4] = 1.0 - 2.0 * (q1_2 + q3_2);
        R[5] = 2.0 * (q2_q3 + q0_q1);
        R[6] = 2.0 * (q1_q3 + q0_q2);
        R[7] = 2.0 * (q2_q3 - q0_q1);
        R[8] = 1.0 - 2.0 * (q1_2 + q2_2);
}

void nx_quaternion_rotate(const double *q, double *v)
{
        NX_ASSERT_PTR(q);
        NX_ASSERT_PTR(v);

        const double q0_2 = q[0]*q[0];
        const double q1_2 = q[1]*q[1];
        const double q2_2 = q[2]*q[2];
        const double q3_2 = q[3]*q[3];
        const double q12  = q[1]*q[2];
        const double q01  = q[0]*q[1];
        const double q02  = q[0]*q[2];
        const double q03  = q[0]*q[3];
        const double q13  = q[1]*q[3];
        const double q23  = q[2]*q[3];

        const double q0_2_q1_2 = q0_2 - q1_2;
        const double q2_2_q3_2 = q2_2 - q3_2;

        double QTQ[9];

        QTQ[0] = q0_2 + q1_2 - q2_2 - q3_2;
        QTQ[1] = 2*(q12 + q03);
        QTQ[2] = 2*(q13 - q02);
        QTQ[3] = 2*(q12 - q03);
        QTQ[4] = q0_2_q1_2 + q2_2_q3_2;
        QTQ[5] = 2*(q23 + q01);
        QTQ[6] = 2*(q13 + q02);
        QTQ[7] = 2*(q23 - q01);
        QTQ[8] = q0_2_q1_2 - q2_2_q3_2;

        QTQ[0] *= v[0];
        QTQ[1] *= v[0];
        QTQ[2] *= v[0];
        QTQ[3] *= v[1];
        QTQ[4] *= v[1];
        QTQ[5] *= v[1];
        QTQ[6] *= v[2];
        QTQ[7] *= v[2];
        QTQ[8] *= v[2];

        v[0] = QTQ[0] + QTQ[3] + QTQ[6];
        v[1] = QTQ[1] + QTQ[4] + QTQ[7];
        v[2] = QTQ[2] + QTQ[5] + QTQ[8];
}

