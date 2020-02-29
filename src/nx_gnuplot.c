/**
 * @file nx_gnuplot.c
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
#define _POSIX_C_SOURCE 2

#include "virg/nexus/nx_gnuplot.h"

#include <stdarg.h>
#include <string.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_string.h"

struct NXGNUPlot
{
        FILE *pgplot;
        NXBool is_pipe;
};

static void nx_gnuplot_init(struct NXGNUPlot *gnuplot)
{
        nx_gnuplot_send_command(gnuplot,
                                "set border 3 lw 3;\n"
                                "set xtics out nomirror;\n"
                                "set ytics out nomirror;\n");
}

struct NXGNUPlot *nx_gnuplot_new(FILE *stream, NXBool persist)
{
        struct NXGNUPlot *gnuplot = NX_NEW(1, struct NXGNUPlot);

        if (stream != NULL) {
                gnuplot->pgplot = stream;
                gnuplot->is_pipe = NX_FALSE;
        } else {
                if (persist)
                        gnuplot->pgplot = popen("gnuplot -persist", "w");
                else
                        gnuplot->pgplot = popen("gnuplot", "w");

                if (gnuplot->pgplot == NULL)
                        NX_FATAL(NX_LOG_TAG, "Error opening pipe to Gnuplot");
                gnuplot->is_pipe = NX_TRUE;
        }

        nx_gnuplot_init(gnuplot);

        return gnuplot;
}

void nx_gnuplot_free(struct NXGNUPlot *gnuplot)
{
        if (gnuplot != NULL) {
                if (gnuplot->is_pipe)
                        pclose(gnuplot->pgplot);
                nx_free(gnuplot);
        }
}

void nx_gnuplot_send_command(struct NXGNUPlot *gnuplot, const char *cmd, ...)
{
        NX_ASSERT_PTR(gnuplot);
        NX_ASSERT_PTR(cmd);

        va_list args;
        va_start(args, cmd);
        vfprintf(gnuplot->pgplot, cmd, args);
        va_end(args);
}

void nx_gnuplot_send_data_frame(struct NXGNUPlot *gnuplot,
                                const struct NXDataFrame *frm,
                                const char *label)
{
        NX_ASSERT_PTR(gnuplot);
        NX_ASSERT_PTR(frm);

        if (label) {
                fprintf(gnuplot->pgplot, "$%s << EOD\n", label);
                if (!nx_data_frame_fprintf(frm, gnuplot->pgplot, " ")) {
                        NX_ERROR(NX_LOG_TAG, "Error sending data frame to GNUPlot");
                }
                fprintf(gnuplot->pgplot, "EOD\n");
        } else {
                if (!nx_data_frame_fprintf(frm, gnuplot->pgplot, " ")) {
                        NX_ERROR(NX_LOG_TAG, "Error sending data frame to GNUPlot");
                }
                fprintf(gnuplot->pgplot, "e\n");
        }
}

void nx_gnuplot_flush(struct NXGNUPlot *gnuplot)
{
        NX_ASSERT_PTR(gnuplot);
        fflush(gnuplot->pgplot);
}

void nx_gnuplot_set_terminal_window(struct NXGNUPlot *gnuplot, int width, int height,
                                    const char *title, int id)
{
        NX_ASSERT_PTR(gnuplot);
        NX_ASSERT_PTR(title);

        nx_gnuplot_send_command(gnuplot,
                                "set terminal qt %d size %d,%d enhanced title \"%s\";\n"
                                "unset output;\n",
                                id, width, height, title);
}

void nx_gnuplot_set_terminal_svg(struct NXGNUPlot *gnuplot, int width, int height,
                                 const char* title, const char *filename)
{
        NX_ASSERT_PTR(gnuplot);
        NX_ASSERT_PTR(title);
        NX_ASSERT_PTR(filename);

        nx_gnuplot_send_command(gnuplot, "set terminal svg size %d,%d enhanced name \"%s\";\n"
                                "set output \"%s\";\n",
                                width, height, title, filename);
}

void nx_gnuplot_set_terminal_pdf(struct NXGNUPlot *gnuplot, double width_in_cm,
                                 double height_in_cm, const char *filename)
{
        NX_ASSERT_PTR(gnuplot);
        NX_ASSERT_PTR(filename);

        nx_gnuplot_send_command(gnuplot, "set terminal pdfcairo enhanced color size %lfcm,%lfcm\n"
                                "set output \"%s\"\n",
                                width_in_cm, height_in_cm, filename);
}

void nx_gnuplot_set_terminal_eps(struct NXGNUPlot *gnuplot, double width_in_cm,
                                 double height_in_cm, const char *filename)
{
        NX_ASSERT_PTR(gnuplot);
        NX_ASSERT_PTR(filename);

        nx_gnuplot_send_command(gnuplot, "set terminal epscairo enhanced color size %lfcm,%lfcm;\n"
                                "set output \"%s\"\n",
                                width_in_cm, height_in_cm, filename);
}

void nx_gnuplot_set_terminal_ascii(struct NXGNUPlot *gnuplot, int width,
                                   int height, NXBool linefeed)
{
        nx_gnuplot_send_command(gnuplot, "set terminal dumb size %d,%d enhanced %sfeed;\n",
                                width, height, (linefeed ? "" : "no"));
}

void nx_gnuplot_set_terminal(struct NXGNUPlot *gnuplot, enum NXTerminalType type,
                             double width, double height,
                             const char *name, int id)
{
        NX_ASSERT_PTR(gnuplot);
        NX_ASSERT_PTR(name);

        switch (type) {
        case NX_TERMINAL_WINDOW: nx_gnuplot_set_terminal_window(gnuplot, width, height, name, id); return;
        case NX_TERMINAL_SVG:    nx_gnuplot_set_terminal_svg(gnuplot, width, height, "VIRG", name); return;
        case NX_TERMINAL_PDF:    nx_gnuplot_set_terminal_pdf(gnuplot, width, height, name); return;
        case NX_TERMINAL_EPS:    nx_gnuplot_set_terminal_eps(gnuplot, width, height, name); return;
        case NX_TERMINAL_ASCII:  nx_gnuplot_set_terminal_ascii(gnuplot, width, height, NX_TRUE); return;
        }
}

void nx_gnuplot_set_title(struct NXGNUPlot *gnuplot, const char *title)
{
        NX_ASSERT_PTR(gnuplot);
        NX_ASSERT_PTR(title);

        nx_gnuplot_send_command(gnuplot, "set title \"%s\";\n", title);
}

void nx_gnuplot_set_xlabel(struct NXGNUPlot *gnuplot,
                          const char *label)
{
        NX_ASSERT_PTR(gnuplot);
        NX_ASSERT_PTR(label);

        nx_gnuplot_send_command(gnuplot, "set title xlabel \"%s\";\n", label);
}

void nx_gnuplot_set_ylabel(struct NXGNUPlot *gnuplot,
                          const char *label)
{
        NX_ASSERT_PTR(gnuplot);
        NX_ASSERT_PTR(label);

        nx_gnuplot_send_command(gnuplot, "set title ylabel \"%s\";\n", label);
}

void nx_gnuplot_set_title_and_labels(struct NXGNUPlot *gnuplot, const char *title,
                                     const char *xlabel, const char *ylabel)
{
        NX_ASSERT_PTR(gnuplot);
        NX_ASSERT_PTR(title);
        NX_ASSERT_PTR(xlabel);
        NX_ASSERT_PTR(ylabel);

        nx_gnuplot_send_command(gnuplot, "set title \"%s\";\n"
                                "set xlabel \"%s\";\n"
                                "set ylabel \"%s\";\n",
                                title, xlabel, ylabel);
}

