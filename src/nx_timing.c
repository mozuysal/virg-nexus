/**
 * @file nx_timing.c
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
