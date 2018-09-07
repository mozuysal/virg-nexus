/**
 * @file nx_camera.h
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
