/**
 * @file nx_camera.h
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
#ifndef VIRG_NEXUS_NX_CAMERA_H
#define VIRG_NEXUS_NX_CAMERA_H

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

struct NXCamera;

struct NXCamera *nx_camera_alloc();

struct NXCamera *nx_camera_new(double fx, double fy, double px, double pv);

void nx_camera_free(struct NXCamera *c);

void nx_camera_set_interior(struct NXCamera *c, double fx, double fy, double px, double pv);

const double *nx_camera_get_K(struct NXCamera *c);

void nx_camera_set_exterior(struct NXCamera *c, const double *R, const double *t);

void nx_camera_set_rotation(struct NXCamera *c, const double *R);

void nx_camera_set_translation(struct NXCamera *c, const double *t);

const double *nx_camera_get_rotation(struct NXCamera *c);

const double *nx_camera_get_translation(struct NXCamera *c);

double nx_camera_project(const struct NXCamera *c, int n, const double *X, double *x);

void nx_camera_project_h(const struct NXCamera *c, int n, const double *X, double *x);

__NX_END_DECL

#endif
