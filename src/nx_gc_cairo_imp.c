/**
 * @file nx_gc_cairo_imp.c
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
#include "virg/nexus/nx_gc.h"

#include <cairo/cairo.h>
#include <cairo/cairo-pdf.h>
#include <cairo/cairo-svg.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_math.h"

struct NXGC {
        cairo_t* ctx;
        enum NXGCType type;

        struct NXImage*  image;
        cairo_surface_t* surface;
};

static inline void set_initial_properties(struct NXGC* gc)
{
        cairo_select_font_face(gc->ctx, "sans-serif", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_antialias(gc->ctx, CAIRO_ANTIALIAS_BEST);
        nx_gc_set_color(gc, 0.0, 0.0, 0.0, 1.0);
}

struct NXGC* nx_gc_new(struct NXImage* img)
{

        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(img->data.v);
        NX_IMAGE_ASSERT_RGBA(img);
        NX_ASSERT(img->dtype == NX_IMAGE_UCHAR);

        struct NXGC* gc = NX_NEW(1, struct NXGC);

        gc->image = img;
        gc->type  = NX_GC_IMAGE;
        gc->surface = cairo_image_surface_create_for_data(img->data.uc,
                                                          CAIRO_FORMAT_ARGB32,
                                                          img->width,
                                                          img->height,
                                                          img->row_stride);

        if (!gc->surface) {
                nx_free(gc);
                NX_FATAL(NX_LOG_TAG, "Could not create Cairo surface for new GC from image.");
        }

        gc->ctx = cairo_create(gc->surface);
        if (!gc->ctx) {
                cairo_surface_destroy(gc->surface);
                nx_free(gc);
                NX_FATAL(NX_LOG_TAG, "Could not create Cairo context for new GC from image.");
        }

        set_initial_properties(gc);

        return gc;
}

struct NXGC* nx_gc_new_pdf(const char* filename, double width, double height)
{
        NX_ASSERT_PTR(filename);

        struct NXGC* gc = NX_NEW(1, struct NXGC);

        gc->image = NULL;
        gc->type = NX_GC_PDF;

        gc->surface = cairo_pdf_surface_create(filename, width, height);
        if (!gc->surface) {
                nx_free(gc);
                NX_FATAL(NX_LOG_TAG, "Could not create Cairo surface for new PDF GC for file %s.", filename);
        }

        gc->ctx = cairo_create(gc->surface);
        if (!gc->ctx) {
                cairo_surface_destroy(gc->surface);
                nx_free(gc);
                NX_FATAL(NX_LOG_TAG, "Could not create Cairo context for new PDF GC for file %s.", filename);
        }

        set_initial_properties(gc);

        return gc;
}

struct NXGC* nx_gc_new_svg(const char* filename, double width, double height)
{
        NX_ASSERT_PTR(filename);

        struct NXGC* gc = NX_NEW(1, struct NXGC);

        gc->image = NULL;
        gc->type = NX_GC_SVG;

        gc->surface = cairo_svg_surface_create(filename, width, height);
        if (!gc->surface) {
                nx_free(gc);
                NX_FATAL(NX_LOG_TAG, "Could not create Cairo surface for new SVG GC for file %s.", filename);
        }

        gc->ctx = cairo_create(gc->surface);
        if (!gc->ctx) {
                cairo_surface_destroy(gc->surface);
                nx_free(gc);
                NX_FATAL(NX_LOG_TAG, "Could not create Cairo context for new SVG GC for file %s.", filename);
        }

        set_initial_properties(gc);

        return gc;
}

void nx_gc_free(struct NXGC* gc)
{
        if (gc) {
                nx_gc_flush(gc);
                if (gc->surface)
                        cairo_surface_destroy(gc->surface);

                if (gc->ctx)
                        cairo_destroy(gc->ctx);
        }
        nx_free(gc);
}


void nx_gc_clear(struct NXGC* gc)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_paint(gc->ctx);
}

void nx_gc_new_path(struct NXGC* gc)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_new_path(gc->ctx);
}

void nx_gc_stroke(struct NXGC* gc)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_stroke(gc->ctx);
}

void nx_gc_fill(struct NXGC* gc)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_fill(gc->ctx);
}

void nx_gc_flush(struct NXGC* gc)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_surface_flush(gc->surface);
}

void nx_gc_set_color(struct NXGC* gc, double red, double green,
                     double blue, double alpha)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        if(gc->type == NX_GC_IMAGE)
                cairo_set_source_rgba(gc->ctx, blue, green, red, alpha);
        else
                cairo_set_source_rgba(gc->ctx, red, green, blue, alpha);
}

void nx_gc_set_line_width(struct NXGC* gc, double width)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_set_line_width(gc->ctx, width);
}

void nx_gc_set_line_dash(struct NXGC* gc, int n_dashes,
                         double* dashes, double offset)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_set_dash(gc->ctx, dashes, n_dashes, offset);
}

void nx_gc_move_to(struct NXGC* gc, double x, double y)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_move_to(gc->ctx, x, y);
}

void nx_gc_line_to(struct NXGC* gc, double x, double y)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_line_to(gc->ctx, x, y);
}

void nx_gc_rel_move_to(struct NXGC* gc, double dx, double dy)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_rel_move_to(gc->ctx, dx, dy);
}

void nx_gc_rel_line_to(struct NXGC* gc, double dx, double dy)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_rel_line_to(gc->ctx, dx, dy);
}

void nx_gc_line(struct NXGC* gc, double x_s, double y_s, double x_e, double y_e)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_move_to(gc->ctx, x_s, y_s);
        cairo_line_to(gc->ctx, x_e, y_e);
}

void nx_gc_rectangle(struct NXGC* gc, double x, double y, double width, double height)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_rectangle(gc->ctx, x, y, width, height);
}

void nx_gc_arc(struct NXGC* gc, double x, double y,
               double radius, double angle_s, double angle_e)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_arc(gc->ctx, x, y, radius, angle_s, angle_e);
}

void nx_gc_circle(struct NXGC* gc, double x, double y, double radius)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_arc(gc->ctx, x, y, radius, 0.0, 2.0*NX_PI);
}

void nx_gc_curve_to(struct NXGC* gc, double x_e, double y_e,
                    double c1x, double c1y, double c2x, double c2y)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_curve_to(gc->ctx, x_e, y_e, c1x, c1y, c2x, c2y);
}

void nx_gc_rel_curve_to(struct NXGC* gc, double dx_e, double dy_e,
                        double c1dx, double c1dy, double c2dx, double c2dy)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_curve_to(gc->ctx, dx_e, dy_e, c1dx, c1dy, c2dx, c2dy);
}

void nx_gc_curve(struct NXGC* gc, double x_s, double y_s, double x_e, double y_e,
                 double c1x, double c1y, double c2x, double c2y)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_move_to(gc->ctx, x_s, y_s);
        cairo_curve_to(gc->ctx, x_e, y_e, c1x, c1y, c2x, c2y);
}

void nx_gc_draw_text(struct NXGC* gc, double x, double y, const char* txt, double font_sz)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_set_font_size(gc->ctx, font_sz);
        cairo_move_to(gc->ctx, x, y);
        cairo_show_text(gc->ctx, txt);
}

void nx_gc_set_text_size(struct NXGC* gc, const char* txt, double font_sz,
                         double* width, double* height)
{
        NX_ASSERT_PTR(gc);
        NX_ASSERT_PTR(gc->ctx);
        NX_ASSERT_PTR(gc->surface);

        cairo_text_extents_t extents;
        cairo_set_font_size(gc->ctx, font_sz);
        cairo_text_extents(gc->ctx, txt, &extents);

        *width  = extents.width;
        *height = extents.height;
}
