/**
 * @file nx_quaternion.h
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
#ifndef VIRG_NEXUS_NX_QUATERNION_H
#define VIRG_NEXUS_NX_QUATERNION_H

#include <math.h>

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_vec234.h"

__NX_BEGIN_DECL

static inline void   nx_quaternion_scale(double *q, const double s);
static inline void   nx_quaternion_add  (double *q, const double *r);
static inline void   nx_quaternion_sum  (double *q, const double *r, const double *s);
static inline void   nx_quaternion_mul  (double *q, const double *r);
static inline void   nx_quaternion_prod (double *q, const double *r, const double *s);
static inline void   nx_quaternion_conj (double *q);
static inline double nx_quaternion_dot  (const double *q, const double *r);
static inline double nx_quaternion_to_unit(double *q);

void nx_quaternion_rotate(const double *q, double *v);
void nx_quaternion_to_R  (const double *q, double *R);
void nx_quaternion_from_R(double *q, const double *R);

static inline void nx_quaternion_to_Rt(const double *q, double *R, double *t);
static inline void nx_quaternion_from_axis_angle(double *q, const double *axis,
                                                 double angle);

/* --------------------------------- Definitions ---------------------------------- */
static inline void nx_quaternion_scale(double *q, const double s)
{
        NX_ASSERT_PTR(q);

        q[0] *= s;
        q[1] *= s;
        q[2] *= s;
        q[3] *= s;
}

static inline void nx_quaternion_add(double *q, const double *r)
{
        NX_ASSERT_PTR(q);
        NX_ASSERT_PTR(r);

        q[0] += r[0];
        q[1] += r[1];
        q[2] += r[2];
        q[3] += r[3];
}

static inline void nx_quaternion_sum(double *q, const double *r, const double *s)
{
        NX_ASSERT_PTR(q);
        NX_ASSERT_PTR(r);
        NX_ASSERT_PTR(s);

        q[0] = r[0] + s[0];
        q[1] = r[1] + s[1];
        q[2] = r[2] + s[2];
        q[3] = r[3] + s[3];
}

static inline void nx_quaternion_mul(double *q, const double *r)
{
        NX_ASSERT_PTR(q);
        NX_ASSERT_PTR(r);

        double q0 = q[0]*r[0] - q[1]*r[1] - q[2]*r[2] - q[3]*r[3];
        double q1 = q[0]*r[1] + q[1]*r[0] + q[2]*r[3] - q[3]*r[2];
        double q2 = q[0]*r[2] - q[1]*r[3] + q[2]*r[0] + q[3]*r[1];
        double q3 = q[0]*r[3] + q[1]*r[2] - q[2]*r[1] + q[3]*r[0];

        q[0] = q0;
        q[1] = q1;
        q[2] = q2;
        q[3] = q3;
}

static inline void nx_quaternion_prod(double *q, const double *r, const double *s)
{
        NX_ASSERT_PTR(q);
        NX_ASSERT_PTR(r);
        NX_ASSERT_PTR(s);

        q[0] = r[0]*s[0] - r[1]*s[1] - r[2]*s[2] - r[3]*s[3];
        q[1] = r[0]*s[1] + r[1]*s[0] + r[2]*s[3] - r[3]*s[2];
        q[2] = r[0]*s[2] - r[1]*s[3] + r[2]*s[0] + r[3]*s[1];
        q[3] = r[0]*s[3] + r[1]*s[2] - r[2]*s[1] + r[3]*s[0];
}

static inline void nx_quaternion_conj(double *q)
{
        NX_ASSERT_PTR(q);

        q[1] = -q[1];
        q[2] = -q[2];
        q[3] = -q[3];
}

static inline double nx_quaternion_dot(const double *q, const double *r)
{
        NX_ASSERT_PTR(q);
        NX_ASSERT_PTR(r);

        return r[0]*q[0]+r[1]*q[1]+r[2]*q[2]+r[3]*q[3];
}

static inline double nx_quaternion_to_unit(double *q)
{
        NX_ASSERT_PTR(q);

        double nrm = sqrt(nx_quaternion_dot(q, q));
        nx_quaternion_scale(q, 1.0 / nrm);
        return nrm;
}

static inline void nx_quaternion_to_Rt(const double *q, double *R, double *t)
{
        NX_ASSERT_PTR(q);
        NX_ASSERT_PTR(R);
        NX_ASSERT_PTR(t);

        nx_quaternion_to_R(q, R);
        t[0] = q[4];
        t[1] = q[5];
        t[2] = q[6];
}

static inline void nx_quaternion_from_axis_angle(double *q,
                                                 const double *axis,
                                                 double angle)
{
        NX_ASSERT_PTR(q);

        if (angle == 0.0) {
                q[0] = 1.0;
                q[1] = 0.0;
                q[2] = 0.0;
                q[3] = 0.0;
        } else {
                NX_ASSERT_PTR(axis);

                double ca2 = cos(angle*0.5);
                double sa2 = sin(angle*0.5);

                double n[3] = { axis[0], axis[1], axis[2] };
                nx_dvec3_to_unit(&n[0]);

                q[0] = ca2;
                q[1] = n[0] * sa2;
                q[2] = n[1] * sa2;
                q[3] = n[2] * sa2;
        }
}

__NX_END_DECL

#endif
