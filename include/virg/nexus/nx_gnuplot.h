/**
 * @file nx_gnuplot.h
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
 * You should have received a copy of the GNU General Public License along with
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef NX_GNUPLOT_H
#define NX_GNUPLOT_H

#include <stdio.h>

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_data_frame.h"

__NX_BEGIN_DECL

enum NXTerminalType {
        NX_TERMINAL_WINDOW,
        NX_TERMINAL_SVG,
        NX_TERMINAL_PDF,
        NX_TERMINAL_EPS,
        NX_TERMINAL_ASCII
};

struct NXGNUPlot;

struct NXGNUPlot *nx_gnuplot_new(FILE *stream, NXBool persist);
void nx_gnuplot_free(struct NXGNUPlot *gnuplot);

void nx_gnuplot_send_command(struct NXGNUPlot *gnuplot, const char *cmd, ...)  __attribute__((format (printf, 2, 3)));
/**
 * Sends the contents of a data frame to GNUPlot either defining a labelled data
 * block or as inline data.
 *
 * @param gnuplot Pointer to GNUPlot structure
 * @param df      Data frame to be sent to GNUPlot
 * @param label   If non-NULL the data frame is set as a data block with name $label, otherwise it is sent as inline data.
 */
void nx_gnuplot_send_data_frame(struct NXGNUPlot *gnuplot,
                                const struct NXDataFrame *df,
                                const char *label);
void nx_gnuplot_flush(struct NXGNUPlot *gnuplot);

void nx_gnuplot_set_terminal_window(struct NXGNUPlot *gnuplot, int width,
                                    int height, const char *title, int id);
void nx_gnuplot_set_terminal_svg(struct NXGNUPlot *gnuplot, int width,
                                 int height, const char* title,
                                 const char *filename);
void nx_gnuplot_set_terminal_pdf(struct NXGNUPlot *gnuplot, double width_in_cm,
                                 double height_in_cm, const char *filename);
void nx_gnuplot_set_terminal_eps(struct NXGNUPlot *gnuplot, double width_in_cm,
                                 double height_in_cm, const char *filename);
void nx_gnuplot_set_terminal_ascii(struct NXGNUPlot *gnuplot, int width,  int height,
                                   NXBool linefeed);
void nx_gnuplot_set_terminal(struct NXGNUPlot *gnuplot, enum NXTerminalType type,
                             double width, double height,
                             const char *name, int id);

void nx_gnuplot_set_title(struct NXGNUPlot *gnuplot, const char *title);
void nx_gnuplot_set_xlabel(struct NXGNUPlot *gnuplot, const char *label);
void nx_gnuplot_set_ylabel(struct NXGNUPlot *gnuplot, const char *label);
void nx_gnuplot_set_title_and_labels(struct NXGNUPlot *gnuplot,
                                     const char *title,
                                     const char *xlabel, const char *ylabel);

__NX_END_DECL

#endif
