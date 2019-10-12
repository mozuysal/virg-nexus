/**
 * @file nx_rotation_3d.c
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
#include "virg/nexus/nx_rotation_3d.h"

#include <math.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_vec234.h"

void nx_rotation_3d_RzRyRx(double *R, double alpha, double beta, double gamma)
{
        NX_ASSERT_PTR(R);

        const double ca = cos(alpha);
        const double sa = sin(alpha);
        const double cb = cos(beta);
        const double sb = sin(beta);
        const double cg = cos(gamma);
        const double sg = sin(gamma);
        const double sbsa = sb*sa;
        const double sbca = sb*ca;

        R[0] = cg*cb;
        R[1] = sg*cb;
        R[2] = -sb;
        R[3] = -sg*ca + cg*sbsa;
        R[4] =  cg*ca + sg*sbsa;
        R[5] =  cb*sa;
        R[6] =  sg*sa + cg*sbca;
        R[7] = -cg*sa + sg*sbca;
        R[8] =  cb*ca;
}

void nx_rotation_3d_RxRyRz(double *R, double alpha, double beta, double gamma)
{
        NX_ASSERT_PTR(R);

        const double ca = cos(alpha);
        const double sa = sin(alpha);
        const double cb = cos(beta);
        const double sb = sin(beta);
        const double cg = cos(gamma);
        const double sg = sin(gamma);
        const double sbsa = sb*sa;
        const double sbca = sb*ca;

        R[0] =  cg*cb;
        R[1] =  cg*sbsa + sg*ca;
        R[2] = -cg*sbca + sg*sa;
        R[3] = -sg*cb;
        R[4] = -sg*sbsa + cg*ca;
        R[5] =  sg*sbca + cg*sa;
        R[6] =  sb;
        R[7] = -cb*sa;
        R[8] =  cb*ca;
}

void nx_rotation_3d_angle_axis(double *R, double theta, const double *axis)
{
        NX_ASSERT_PTR(R);

        double v[3] = { axis[0], axis[1], axis[2] };
        nx_dvec3_to_unit(&v[0]);

        double c = cos(theta);
        double s = sin(theta);
        R[0] = c + v[0]*v[0]*(1 - c);
        R[1] = v[1]*v[0]*(1 - c) + v[2]*s;
        R[2] = v[2]*v[0]*(1 - c) - v[1]*s;
        R[3] = v[1]*v[0]*(1 - c) - v[2]*s;;
        R[4] = c + v[1]*v[1]*(1 - c);
        R[5] = v[2]*v[1]*(1 - c) + v[0]*s;;
        R[6] = v[0]*v[2]*(1 - c) + v[1]*s;;
        R[7] = v[1]*v[2]*(1 - c) - v[0]*s;;
        R[8] = c + v[2]*v[2]*(1 - c);
}
