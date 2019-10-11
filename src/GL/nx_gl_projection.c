/**
 * @file nx_gl_projection.c
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
#include "virg/nexus/GL/nx_gl_projection.h"

#include <string.h>

void nx_gl_projection_sym_perspective_from_camera(double *P,
                                                  double fx, double fy,
                                                  double near, double far,
                                                  double vp_w, double vp_h)
{
       memset(P, 0, 16*sizeof(P[0]));
       P[0]  = 2.0 * fx / vp_w;
       P[5]  = 2.0 * fy / vp_h;
       P[10] = -(far + near) / (far - near);
       P[11] = -1.0f;
       P[14] = -2.0 * near * far / (far - near);
}
