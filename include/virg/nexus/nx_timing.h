/**
 * @file nx_timing.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2015 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_TIMING_H
#define VIRG_NEXUS_NX_TIMING_H

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

struct NXTimer;

struct NXTimer *nx_timer_new();
void nx_timer_free(struct NXTimer *t);

void nx_timer_start(struct NXTimer *t);
void nx_timer_stop (struct NXTimer *t);

double nx_timing_measure_in_sec(const struct NXTimer *t);
double nx_timing_lap_in_sec    (const struct NXTimer *t);

__NX_END_DECL

#endif
