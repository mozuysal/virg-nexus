/**
 * @file nx_timing.c
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
#include "virg/nexus/nx_timing.h"

#include <sys/time.h>

#include "virg/nexus/nx_alloc.h"

struct NXTimer
{
        struct timeval start;
        struct timeval stop;
};

struct NXTimer *nx_timer_new()
{
        struct NXTimer *t = NX_NEW(1, struct NXTimer);
        return t;
}

void nx_timer_free(struct NXTimer *t)
{
        nx_free(t);
}

void nx_timer_start(struct NXTimer *t)
{
        gettimeofday(&t->start, NULL);
}

void nx_timer_stop(struct NXTimer *t)
{
        gettimeofday(&t->stop, NULL);
}

static inline double nx_timing_diff_in_sec(const struct timeval *ts, const struct timeval *te)
{
        return (double) ((te->tv_usec - ts->tv_usec) * 1.0e-6 +
                         (double) (te->tv_sec - ts->tv_sec));
}

double nx_timing_measure_in_sec(const struct NXTimer *t)
{
        return nx_timing_diff_in_sec(&t->start, &t->stop);
}

double nx_timing_lap_in_sec(const struct NXTimer *t)
{
        struct timeval lap_t;
        gettimeofday(&lap_t, NULL);

        return nx_timing_diff_in_sec(&t->start, &lap_t);
}
