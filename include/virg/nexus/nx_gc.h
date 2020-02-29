/**
 * @file nx_gc.h
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
#ifndef VIRG_NEXUS_NX_GC_H
#define VIRG_NEXUS_NX_GC_H

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_image.h"

#define NX_GC_MM_TO_POINTS 2.8346472
#define NX_GC_A4_WIDTH 595.2759120
#define NX_GC_A4_HEIGHT 841.8902184

enum NXGCType {
        NX_GC_IMAGE = 0,
        NX_GC_PDF,
        NX_GC_SVG
};

struct NXGC;

struct NXGC* nx_gc_new(struct NXImage* img);

/**
 * Creates a new graphics context with a PDF surface.
 *
 * @param filename Name of the PDF file to generate
 * @param width Width of the PDF page in points (1 point = 1/72.0 inch)
 * @param height Height of the PDF page in points (1 point = 1/72.0 inch)
 *
 * @return pointer to the newly created graphics context
 */
struct NXGC* nx_gc_new_pdf(const char* filename, double width, double height);

/**
 * Creates a new graphics context with an SVG surface.
 *
 * @param filename Name of the SVG file to generate
 * @param width Width of the SVG page in points (1 point = 1/72.0 inch)
 * @param height Height of the SVG page in points (1 point = 1/72.0 inch)
 *
 * @return pointer to the newly created graphics context
 */
struct NXGC* nx_gc_new_svg(const char* filename, double width, double height);
void nx_gc_free(struct NXGC* gc);

void nx_gc_clear(struct NXGC* gc);
void nx_gc_new_path(struct NXGC* gc);
void nx_gc_stroke(struct NXGC* gc);
void nx_gc_fill(struct NXGC* gc);
void nx_gc_flush(struct NXGC* gc);

void nx_gc_set_color(struct NXGC* gc, double red, double green,
                     double blue, double alpha);
void nx_gc_set_line_width(struct NXGC* gc, double width);
void nx_gc_set_line_dash(struct NXGC* gc, int n_dashes,
                         double* dashes, double offset);

void nx_gc_move_to(struct NXGC* gc, double x, double y);
void nx_gc_line_to(struct NXGC* gc, double x, double y);
void nx_gc_rel_move_to(struct NXGC* gc, double dx, double dy);
void nx_gc_rel_line_to(struct NXGC* gc, double dx, double dy);
void nx_gc_line(struct NXGC* gc, double x_s, double y_s, double x_e, double y_e);
void nx_gc_rectangle(struct NXGC* gc, double x, double y, double width, double height);

void nx_gc_arc(struct NXGC* gc, double x, double y,
               double radius, double angle_s, double angle_e);
void nx_gc_circle(struct NXGC* gc, double x, double y, double radius);
void nx_gc_curve_to(struct NXGC* gc, double x_e, double y_e,
                    double c1x, double c1y, double c2x, double c2y);
void nx_gc_rel_curve_to(struct NXGC* gc, double dx_e, double dy_e,
                        double c1dx, double c1dy, double c2dx, double c2dy);
void nx_gc_curve(struct NXGC* gc, double x_s, double y_s, double x_e, double y_e,
                 double c1x, double c1y, double c2x, double c2y);

void nx_gc_draw_text(struct NXGC* gc, double x, double y, const char* txt, double font_sz);
void nx_gc_set_text_size(struct NXGC* gc, const char* txt, double font_sz,
                         double* width, double* height);

__NX_END_DECL

#endif
