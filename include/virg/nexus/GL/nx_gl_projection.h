/**
 * @file nx_gl_projection.h
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
#ifndef VIRG_NEXUS_NX_GL_PROJECTION_H
#define VIRG_NEXUS_NX_GL_PROJECTION_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/GL/nx_gl.h"

__NX_BEGIN_DECL

void nx_gl_projection_sym_perspective_from_camera(double *P,
                                                  double fx, double fy,
                                                  double near, double far,
                                                  double vp_w, double vp_h);

void nx_gl_projection_to_mvp(GLfloat *mvp, const double *M,
                             const double *V, const double *P);

__NX_END_DECL

#endif
