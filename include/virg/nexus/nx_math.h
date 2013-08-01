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

static inline int nx_min_i(int x, int y);
static inline float nx_min_s(float x, float y);
static inline double nx_min_d(double x, double y);

static inline int nx_max_i(int x, int y);
static inline float nx_max_s(float x, float y);
static inline double nx_max_d(double x, double y);

float nx_mat3_inv_s(float *Minv, const float *M);
double nx_mat3_inv_d(double *Minv, const double *M);
float nx_mat3_inv_sd(float *Minv, const float *M);

double nx_erf(double x);

/* ----------------------------- inline definitions ------------------------------- */
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
