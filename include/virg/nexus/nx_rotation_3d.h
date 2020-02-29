/**
 * @file nx_rotation_3d.h
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
#ifndef VIRG_NEXUS_NX_ROTATION_3D_H
#define VIRG_NEXUS_NX_ROTATION_3D_H

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

static inline void nx_rotation_3d_invert(double *R)
{
        double t;
        t = R[1]; R[1] = R[3]; R[3] = t;
        t = R[2]; R[2] = R[6]; R[6] = t;
        t = R[5]; R[5] = R[7]; R[7] = t;
}

void nx_rotation_3d_RzRyRx(double *R, double alpha, double beta, double gamma);
void nx_rotation_3d_RxRyRz(double *R, double alpha, double beta, double gamma);

void nx_rotation_3d_axis_angle(double *R, const double *axis, double theta);

__NX_END_DECL

#endif
