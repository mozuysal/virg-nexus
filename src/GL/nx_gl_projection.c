/**
 * @file nx_gl_projection.c
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
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/GL/nx_gl_projection.h"

#include <string.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_mat234.h"

void nx_gl_projection_sym_perspective_from_camera(double *P,
                                                  double fx, double fy,
                                                  double near, double far,
                                                  double vp_w, double vp_h)
{
        NX_ASSERT_PTR(P);

        memset(P, 0, 16*sizeof(P[0]));
        P[0]  = 2.0 * fx / vp_w;
        P[5]  = 2.0 * fy / vp_h;
        P[10] = -(far + near) / (far - near);
        P[11] = -1.0f;
        P[14] = -2.0 * near * far / (far - near);
}

void nx_gl_projection_to_mvp(GLfloat *mvp, const double *M,
                             const double *V, const double *P)
{
        NX_ASSERT_PTR(mvp);
        NX_ASSERT_PTR(P);

        double T1[16];
        const double *PV = NULL;
        if (V) {
                nx_dmat4_mul(&T1[0], P, V);
                PV = &T1[0];
        } else {
                PV = P;
        }

        double T2[16];
        const double *PVM = NULL;
        if (M) {
                nx_dmat4_mul(&T2[0], PV, M);
                PVM = &T2[0];
        } else {
                PVM = PV;
        }

        for (int i = 0; i < 16; ++i)
                mvp[i] = PVM[i];
}
