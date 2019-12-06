/**
 * @file nx_timing.h
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
#ifndef VIRG_NEXUS_NX_TIMING_H
#define VIRG_NEXUS_NX_TIMING_H

#include "virg/nexus/nx_config.h"

#include <time.h>

__NX_BEGIN_DECL

struct NXTimer
{
        struct timespec start;
        struct timespec stop;
};

static inline void nx_timer_start(struct NXTimer *t)
{
        clock_gettime(CLOCK_MONOTONIC_RAW, &t->start);
}

static inline void nx_timer_stop(struct NXTimer *t)
{
        clock_gettime(CLOCK_MONOTONIC_RAW, &t->stop);
}

static inline double nx_timer_measure_in_msec(const struct NXTimer *t)
{
        return (t->stop.tv_sec - t->start.tv_sec) * 1e+3
                + (t->stop.tv_nsec - t->start.tv_nsec) * 1e-6;
}

__NX_END_DECL

#endif
