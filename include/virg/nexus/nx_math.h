/**
 * @file nx_math.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Izmir Institute of Technology. All rights reserved.
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

inline int nx_min_i(int x, int y);
inline float nx_min_s(float x, float y);
inline double nx_min_d(double x, double y);

inline int nx_max_i(int x, int y);
inline float nx_max_s(float x, float y);
inline double nx_max_d(double x, double y);

double nx_erf(double x);

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

__NX_END_DECL

#endif
