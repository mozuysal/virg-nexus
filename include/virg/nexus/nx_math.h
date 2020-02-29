/**
 * @file nx_math.h
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
#ifndef VIRG_NEXUS_NX_MATH_H
#define VIRG_NEXUS_NX_MATH_H

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

#define NX_PI 3.14159265358979323846

static inline int nx_signum(double d);

static inline int nx_min_i(int x, int y);
static inline float nx_min_s(float x, float y);
static inline double nx_min_d(double x, double y);

static inline int nx_max_i(int x, int y);
static inline float nx_max_s(float x, float y);
static inline double nx_max_d(double x, double y);

double nx_erf(double x);

static inline int nx_compare_double_incr(const void *d0p, const void *d1p);
static inline int nx_compare_int_incr   (const void *i0p, const void *i1p);

/* ----------------------------- inline definitions ------------------------------- */
static inline int nx_signum(double d)
{
        return (d > 0.0) - (d < 0.0);
}

int nx_min_i(int x, int y)
{
        return (x < y) ? x : y;
}

float nx_min_s(float x, float y)
{
        return (x < y) ? x : y;
}

double nx_min_d(double x, double y)
{
        return (x < y) ? x : y;
}

int nx_max_i(int x, int y)
{
        return (x > y) ? x : y;
}

float nx_max_s(float x, float y)
{
        return (x > y) ? x : y;
}

double nx_max_d(double x, double y)
{
        return (x > y) ? x : y;
}

int nx_compare_double_incr(const void *d0p, const void *d1p)
{
        double d0 = *(double *)d0p;
        double d1 = *(double *)d1p;

        if (d0 < d1) {
                return -1;
        } else if (d0 > d1) {
                return +1;
        } else {
                return 0;
        }
}

int nx_compare_int_incr(const void *i0p, const void *i1p)
{
        int i0 = *(int *)i0p;
        int i1 = *(int *)i1p;

        return i0 - i1;
}

__NX_END_DECL

#endif
