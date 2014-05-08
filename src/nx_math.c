/**
 * @file nx_math.c
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_math.h"

#include <math.h>
#include <float.h>

float nx_mat3_inv_s(float *Minv, const float *M)
{
        float t4  = M[0] * M[4];
        float t6  = M[0] * M[7];
        float t8  = M[3] * M[1];
        float t10 = M[6] * M[1];
        float t12 = M[3] * M[2];
        float t14 = M[6] * M[2];

        float det_M = t4 * M[8] - t6 * M[5] - t8 * M[8] + t10 * M[5] + t12 * M[7] - t14 * M[4];

        float t17 = 1.0f / det_M;
        Minv[0] = (M[4] * M[8] - M[7] * M[5]) * t17;
        Minv[1] = -(M[1] * M[8] - M[7] * M[2]) * t17;
        Minv[2] = (M[1] * M[5] - M[4] * M[2]) * t17;
        Minv[3] = -(M[3] * M[8] - M[6] * M[5]) * t17;
        Minv[4] = (M[0] * M[8] - t14) * t17;
        Minv[5] = -(M[0] * M[5] - t12) * t17;
        Minv[6] = (M[3] * M[7] - M[6] * M[4]) * t17;
        Minv[7] = -(t6 - t10) * t17;
        Minv[8] = (t4 - t8) * t17;

        return det_M;

}

double nx_mat3_inv_d(double *Minv, const double *M)
{
        double t4  = M[0] * M[4];
        double t6  = M[0] * M[7];
        double t8  = M[3] * M[1];
        double t10 = M[6] * M[1];
        double t12 = M[3] * M[2];
        double t14 = M[6] * M[2];

        double det_M = t4 * M[8] - t6 * M[5] - t8 * M[8] + t10 * M[5] + t12 * M[7] - t14 * M[4];

        double t17 = 1.0 / det_M;
        Minv[0] = (M[4] * M[8] - M[7] * M[5]) * t17;
        Minv[1] = -(M[1] * M[8] - M[7] * M[2]) * t17;
        Minv[2] = (M[1] * M[5] - M[4] * M[2]) * t17;
        Minv[3] = -(M[3] * M[8] - M[6] * M[5]) * t17;
        Minv[4] = (M[0] * M[8] - t14) * t17;
        Minv[5] = -(M[0] * M[5] - t12) * t17;
        Minv[6] = (M[3] * M[7] - M[6] * M[4]) * t17;
        Minv[7] = -(t6 - t10) * t17;
        Minv[8] = (t4 - t8) * t17;

        return det_M;
}

float nx_mat3_inv_sd(float *Minv, const float *M)
{
        double Md[9];
        double Md_inv[9];
        double det_Md;

        for (int i = 0; i < 9; ++i)
                Md[i] = M[i];

        det_Md = nx_mat3_inv_d(Md_inv, Md);

        for (int i = 0; i < 9; ++i)
                Minv[i] = Md_inv[i];

        return det_Md;
}

// From http://www.johndcook.com/cpp_erf.html
double nx_erf(double x)
{
    // constants
    double a1 =  0.254829592;
    double a2 = -0.284496736;
    double a3 =  1.421413741;
    double a4 = -1.453152027;
    double a5 =  1.061405429;
    double p  =  0.3275911;

    // Save the sign of x
    int sign = 1;
    if (x < 0)
        sign = -1;
    x = fabs(x);

    // A&S formula 7.1.26
    // A&S refers to Handbook of Mathematical Functions by Abramowitz and Stegun.
    double t = 1.0/(1.0 + p*x);
    double y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);

    return sign*y;
}
