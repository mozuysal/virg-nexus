/**
 * @file nx_pinhole.h
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
#ifndef VIRG_NEXUS_NX_PINHOLE_H
#define VIRG_NEXUS_NX_PINHOLE_H

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

void nx_pinhole_projection_from_krt(double *P, const double *K,
                                    const double *R, const double *t);

double nx_pinhole_project  (const double *P, int n, const float *X,  float *x);
void   nx_pinhole_project_h(const double *P, int n, const float *Xh, float *xh);

__NX_END_DECL

#endif
