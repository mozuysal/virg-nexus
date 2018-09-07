/**
 * @file nx_camera.c
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2018 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_camera.h"

#include <string.h>
#include <float.h>
#include <math.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_math.h"
#include "virg/nexus/nx_mat234.h"

struct NXCamera
{
        double K[9];
        double R[9];
        double t[3];
        double P[12];
};

static inline void nx_camera_update_projection_matrix(struct NXCamera *c)
{
        double *P = &c->P[0];
        double *K = &c->K[0];
        double *R = &c->R[0];
        double *t = &c->t[0];

        nx_dmat3_mul_ua(P, K, R);

        P[9]  = K[0]*t[0]+K[6]*t[2];
        P[10] = K[4]*t[1]+K[7]*t[2];
        P[11] = t[2];
}

struct NXCamera *nx_camera_alloc()
{
        struct NXCamera *c = (struct NXCamera*)nx_fcalloc(1, sizeof(struct NXCamera));
        nx_dmat3_eye(&c->K[0]);
        nx_dmat3_eye(&c->R[0]);
        nx_dmat3_eye(&c->P[0]);

        return c;
}

struct NXCamera *nx_camera_new(double fx, double fy, double px, double py)
{
        struct NXCamera *c = nx_camera_alloc();
        nx_camera_set_interior(c, fx, fy, px, py);
        return c;
}

void nx_camera_free(struct NXCamera *c)
{
        if (c) {
                nx_free(c);
        }
}

void nx_camera_set_interior(struct NXCamera *c, double fx, double fy, double px, double py)
{
        NX_ASSERT_PTR(c);

        c->K[0] = fx;
        c->K[4] = fy;
        c->K[6] = px;
        c->K[7] = py;
        nx_camera_update_projection_matrix(c);
}

const double *nx_camera_get_K(struct NXCamera *c)
{
        NX_ASSERT_PTR(c);
        return &c->K[0];
}

void nx_camera_set_exterior(struct NXCamera *c, const double *R, const double *t)
{
        NX_ASSERT_PTR(c);
        NX_ASSERT_PTR(R);
        NX_ASSERT_PTR(t);

        memcpy(&c->R[0], R, 9*sizeof(*R));
        memcpy(&c->t[0], t, 3*sizeof(*t));
        nx_camera_update_projection_matrix(c);
}

void nx_camera_set_rotation(struct NXCamera *c, const double *R)
{
        NX_ASSERT_PTR(c);
        NX_ASSERT_PTR(R);

        memcpy(&c->R[0], R, 9*sizeof(*R));
        nx_camera_update_projection_matrix(c);
}

void nx_camera_set_translation(struct NXCamera *c, const double *t)
{
        NX_ASSERT_PTR(c);
        NX_ASSERT_PTR(t);

        memcpy(&c->t[0], t, 3*sizeof(*t));
        nx_camera_update_projection_matrix(c);
}

const double *nx_camera_get_rotation(struct NXCamera *c)
{
        NX_ASSERT_PTR(c);
        return &c->R[0];
}

const double *nx_camera_get_translation(struct NXCamera *c)
{
        NX_ASSERT_PTR(c);
        return &c->t[0];
}

double nx_camera_project(const struct NXCamera *c, int n, const double *X, double *x)
{
        NX_ASSERT_PTR(c);
        NX_ASSERT(n > 0);
        NX_ASSERT_PTR(X);
        NX_ASSERT_PTR(x);

        double min_abs_s = DBL_MAX;
        const double *P = &c->P[0];
        for (int i = 0; i < n; ++i, x += 2, X += 3) {
                double s = (P[2]*X[0] + P[5]*X[1] + P[8]*X[2] + P[11]);
                min_abs_s = nx_min_d(min_abs_s, fabs(s));
                double inv_s = 1.0 / s;
                x[0] = (P[0]*X[0] + P[3]*X[1] + P[6]*X[2] + P[9])*inv_s;
                x[1] = (P[1]*X[0] + P[4]*X[1] + P[7]*X[2] + P[10])*inv_s;
        }

        NX_ASSERT(min_abs_s > 0.0);

        return min_abs_s;
}

void nx_camera_project_h(const struct NXCamera *c, int n, const double *X, double *x)
{
        NX_ASSERT_PTR(c);
        NX_ASSERT(n > 0);
        NX_ASSERT_PTR(X);
        NX_ASSERT_PTR(x);

        const double *P = &c->P[0];
        for (int i = 0; i < n; ++i, x += 3, X += 4) {
                x[0] = P[0]*X[0] + P[3]*X[1] + P[6]*X[2] + P[9]*X[3];
                x[1] = P[1]*X[0] + P[4]*X[1] + P[7]*X[2] + P[10]*X[3];
                x[2] = P[2]*X[0] + P[5]*X[1] + P[8]*X[2] + P[11]*X[3];
        }
}
