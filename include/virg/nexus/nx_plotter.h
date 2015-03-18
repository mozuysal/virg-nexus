/**
 * @file nx_plotter.h
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
#ifndef VIRG_NEXUS_NX_PLOTTER_H
#define VIRG_NEXUS_NX_PLOTTER_H

#include <stdio.h>

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

enum NXTerminalType {
        NX_TERMINAL_WINDOW,
        NX_TERMINAL_SVG,
        NX_TERMINAL_PDF,
        NX_TERMINAL_EPS,
        NX_TERMINAL_ASCII
};

struct NXPlotter;

struct NXPlotter *nx_plotter_new(FILE *stream);
void nx_plotter_free(struct NXPlotter *plotter);
void nx_plotter_send_command(struct NXPlotter *plotter, const char *cmd, ...);

void nx_plotter_set_terminal_window(struct NXPlotter *plotter, int width, int height, const char *title, int id);
void nx_plotter_set_terminal_svg(struct NXPlotter *plotter, int width, int height, const char* title, const char *filename);
void nx_plotter_set_terminal_pdf(struct NXPlotter *plotter, double width_in_cm, double height_in_cm, const char *filename);
void nx_plotter_set_terminal_eps(struct NXPlotter *plotter, double width_in_cm, double height_in_cm, const char *filename);
void nx_plotter_set_terminal_ascii(struct NXPlotter *plotter, int width,  int height, NXBool linefeed);
void nx_plotter_set_terminal(struct NXPlotter *plotter, enum NXTerminalType type, double width, double height, const char *name, int id);

void nx_plotter_set_title(struct NXPlotter *plotter, const char *title);
void nx_plotter_set_label(struct NXPlotter *plotter, const char *axis, const char *label);
void nx_plotter_set_title_and_labels(struct NXPlotter *plotter, const char *title, const char *xlabel, const char *ylabel);

void nx_plotter_single_plot(struct NXPlotter *plotter, int n, const char *fmt, ...);
void nx_plotter_plot(struct NXPlotter *plotter, int n_plots, ...);

struct NXPlotter *nx_plotter_instance();
void nx_plotter_instance_free();

#define NX_PLOT_SET_TERMINAL(type,width,height,name,id) nx_plotter_set_terminal(nx_plotter_instance(),(type),(width),(height),(name),(id))
#define NX_PLOT_SINGLE(n,fmt,...) nx_plotter_single_plot(nx_plotter_instance(),(n),(fmt),__VA_ARGS__)
#define NX_PLOT(n_plots,...) nx_plotter_plot(nx_plotter_instance(),(n_plots),__VA_ARGS__)

__NX_END_DECL

#endif
