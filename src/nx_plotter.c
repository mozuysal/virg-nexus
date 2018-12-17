/**
 * @file nx_plotter.c
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
#define _POSIX_C_SOURCE 2

#include "virg/nexus/nx_plotter.h"

#include <stdarg.h>
#include <string.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_message.h"
#include "virg/nexus/nx_string.h"

static struct NXPlotter *g_plotter = NULL;

struct NXPlotter
{
        FILE *pgplot;
        NXBool is_pipe;
};

static void nx_plotter_init(struct NXPlotter *plotter)
{
        nx_plotter_send_command(plotter,
                                "set border 3 lw 3\n"
                                "set xtics out nomirror\n"
                                "set ytics out nomirror\n");
}

struct NXPlotter *nx_plotter_new(FILE *stream)
{
        struct NXPlotter *plotter = NX_NEW(1, struct NXPlotter);

        if (stream != NULL) {
                plotter->pgplot = stream;
                plotter->is_pipe = NX_FALSE;
        } else {
                plotter->pgplot = popen("gnuplot", "w");
                if (plotter->pgplot == NULL)
                        NX_FATAL(NX_LOG_TAG, "Error opening pipe to Gnuplot");
                plotter->is_pipe = NX_TRUE;
        }

        nx_plotter_init(plotter);

        return plotter;
}

void nx_plotter_free(struct NXPlotter *plotter)
{
        if (plotter != NULL) {
                if (plotter->is_pipe)
                        pclose(plotter->pgplot);
                nx_free(plotter);
        }
}

void nx_plotter_send_command(struct NXPlotter *plotter, const char *cmd, ...)
{
        va_list args;
        va_start(args, cmd);
        vfprintf(plotter->pgplot, cmd, args);
        va_end(args);

        fflush(plotter->pgplot);
}

void nx_plotter_set_terminal_window(struct NXPlotter *plotter, int width, int height, const char *title, int id)
{
        nx_plotter_send_command(plotter, "set terminal qt %d size %d,%d enhanced title \"%s\"\n",
                                id, width, height, title);
}

void nx_plotter_set_terminal_svg(struct NXPlotter *plotter, int width, int height, const char* title, const char *filename)
{
        nx_plotter_send_command(plotter, "set terminal svg size %d,%d enhanced name \"%s\"\n"
                                "set output \"%s\"\n",
                                width, height, title, filename);
}

void nx_plotter_set_terminal_pdf(struct NXPlotter *plotter, double width_in_cm, double height_in_cm, const char *filename)
{
        nx_plotter_send_command(plotter, "set terminal pdfcairo enhanced color size %lfcm,%lfcm\n"
                                "set output \"%s\"\n",
                                width_in_cm, height_in_cm, filename);
}

void nx_plotter_set_terminal_eps(struct NXPlotter *plotter, double width_in_cm, double height_in_cm, const char *filename)
{
        nx_plotter_send_command(plotter, "set terminal epscairo enhanced color size %lfcm,%lfcm\n"
                                "set output \"%s\"\n",
                                width_in_cm, height_in_cm, filename);
}

void nx_plotter_set_terminal_ascii(struct NXPlotter *plotter, int width,  int height, NXBool linefeed)
{
        nx_plotter_send_command(plotter, "set terminal dumb size %d,%d enhanced %sfeed\n",
                                width, height, (linefeed ? "" : "no"));
}

void nx_plotter_set_terminal(struct NXPlotter *plotter, enum NXTerminalType type, double width, double height, const char *name, int id)
{
        switch (type) {
        case NX_TERMINAL_WINDOW: nx_plotter_set_terminal_window(plotter, width, height, name, id); return;
        case NX_TERMINAL_SVG:    nx_plotter_set_terminal_svg(plotter, width, height, "VIRG", name); return;
        case NX_TERMINAL_PDF:    nx_plotter_set_terminal_pdf(plotter, width, height, name); return;
        case NX_TERMINAL_EPS:    nx_plotter_set_terminal_eps(plotter, width, height, name); return;
        case NX_TERMINAL_ASCII:  nx_plotter_set_terminal_ascii(plotter, width, height, NX_TRUE); return;
        }
}

void nx_plotter_set_title(struct NXPlotter *plotter, const char *title)
{
        nx_plotter_send_command(plotter, "set title \"%s\"\n", title);
}

void nx_plotter_set_label(struct NXPlotter *plotter, const char *axis, const char *label)
{
        nx_plotter_send_command(plotter, "set title %slabel \"%s\"\n", axis, label);
}

void nx_plotter_set_title_and_labels(struct NXPlotter *plotter, const char *title, const char *xlabel, const char *ylabel)
{
        nx_plotter_send_command(plotter, "set title \"%s\"\n"
                                "set xlabel \"%s\"\n"
                                "set ylabel \"%s\"\n",
                                title, xlabel, ylabel);
}

static char *split_plot_format(const char *fmt, int *n)
{
        *n = 0;
        char *colon = strchr(fmt, ':');
        if (colon == NULL)
                return NULL;

        const char *p = fmt;
        while (p < colon) {
                switch (*p) {
                case 'i':
                case 'l':
                case 'f':
                case 'd':
                case 's':
                        break;
                default:
                        NX_FATAL(NX_LOG_TAG, "Unknown character in plot format: %c", *p);
                }
                ++p;
                ++*n;
        }

        return colon+1;
}

struct NXPlotData {
        char fmt;
        union {
                int *i;
                long *l;
                float *f;
                double *d;
                char **s;
        } ptr;
};

struct NXPlotData *nx_plot_data_parse(const char *fmt, int n_data, va_list args)
{
        struct NXPlotData *pd = NX_NEW(n_data, struct NXPlotData);
        for (int i = 0; i < n_data; ++i) {
                pd[i].fmt = fmt[i];
                switch (pd[i].fmt) {
                case 'i': pd[i].ptr.i = va_arg(args, int *); break;
                case 'l': pd[i].ptr.l = va_arg(args, long *); break;
                case 'f': pd[i].ptr.f = va_arg(args, float *); break;
                case 'd': pd[i].ptr.d = va_arg(args, double *); break;
                case 's': pd[i].ptr.s = va_arg(args, char **); break;
                };
        }

        return pd;
}

static void nx_plotter_vsend_data(struct NXPlotter *plotter, int n, int n_data, struct NXPlotData *plot_data)
{
        for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n_data; ++j) {
                        struct NXPlotData *d = plot_data + j;
                        switch (d->fmt) {
                        case 'i': fprintf(plotter->pgplot, "%d ", d->ptr.i[i]); break;
                        case 'l': fprintf(plotter->pgplot, "%ld ", d->ptr.l[i]); break;
                        case 'f': fprintf(plotter->pgplot, "%e ", d->ptr.f[i]); break;
                        case 'd': fprintf(plotter->pgplot, "%le ", d->ptr.d[i]); break;
                        case 's': fprintf(plotter->pgplot, "%s ", d->ptr.s[i]); break;
                        };
                }
                fprintf(plotter->pgplot, "\n");
        }
        fprintf(plotter->pgplot, "e\n");
        fflush(plotter->pgplot);
}

void nx_plotter_single_plot(struct NXPlotter *plotter, int n, const char *fmt, ...)
{
        int n_data = 0;
        const char *plot_statements = split_plot_format(fmt, &n_data);
        if (n_data < 1)
                NX_FATAL(NX_LOG_TAG, "single plot requires at least a single data argument");

        nx_plotter_send_command(plotter, "plot '-' %s\n", plot_statements);

        va_list args;
        va_start(args, fmt);
        struct NXPlotData *plot_data = nx_plot_data_parse(fmt, n_data, args);
        nx_plotter_vsend_data(plotter, n, n_data, plot_data);
        nx_free(plot_data);
        va_end(args);
}

void nx_plotter_plot(struct NXPlotter *plotter, int n_plots, ...)
{
        struct NXPlotData **pd_arr = NX_NEW(n_plots, struct NXPlotData *);
        int *n_data_arr = NX_NEW(n_plots, int);
        int *n_arr = NX_NEW(n_plots, int);

        fprintf(plotter->pgplot, "plot ");
        va_list args;
        va_start(args, n_plots);
        for (int i = 0; i < n_plots; ++i) {
                n_arr[i] = va_arg(args, int);
                char *fmt = va_arg(args, char *);
                n_data_arr[i] = 0;
                const char *plot_statements = split_plot_format(fmt, &n_data_arr[i]);
                fprintf(plotter->pgplot, "   '-' %s", plot_statements);
                if (i != n_plots - 1)
                        fprintf(plotter->pgplot, ", \\\n");

                pd_arr[i] = nx_plot_data_parse(fmt, n_data_arr[i], args);
        }
        fprintf(plotter->pgplot, "\n");

        for (int i = 0; i < n_plots; ++i) {
                nx_plotter_vsend_data(plotter, n_arr[i], n_data_arr[i], pd_arr[i]);
                nx_free(pd_arr[i]);
        }
        va_end(args);

        nx_free(n_data_arr);
        nx_free(n_arr);
        nx_free(pd_arr);
}

struct NXPlotter *nx_plotter_instance()
{
        if (g_plotter == NULL)
                g_plotter = nx_plotter_new(NULL);
        return g_plotter;
}

void nx_plotter_instance_free()
{
        nx_plotter_free(g_plotter);
        g_plotter = NULL;
}
