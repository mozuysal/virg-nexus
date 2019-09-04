/**
 * @file vg_gc.hpp
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2019 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_VG_GC_HPP
#define VIRG_NEXUS_VG_GC_HPP

#include <memory>
#include <string>

#include "virg/nexus/nx_gc.h"

#include "virg/nexus/vg_image.hpp"
#include "virg/nexus/vg_color.hpp"

namespace virg {
namespace nexus {

class VGGraphicsContext {
public:
        static const double A4_WIDTH_IN_PTS;
        static const double A4_HEIGHT_IN_PTS;

        ~VGGraphicsContext();

        static VGGraphicsContext new_from_image(VGImage rgba);
        static VGGraphicsContext new_pdf(const std::string& filename,
                                         double width, double height);
        static VGGraphicsContext new_svg(const std::string& filename,
                                         double width, double height);

        void clear   () { nx_gc_clear(m_gc.get()); }
        void new_path() { nx_gc_new_path(m_gc.get()); }
        void stroke  () { nx_gc_stroke(m_gc.get()); }
        void fill    () { nx_gc_fill(m_gc.get()); }
        void flush   () { nx_gc_flush(m_gc.get()); }

        void set_color     (double red, double green, double blue, double alpha) { nx_gc_set_color(m_gc.get(), red, green, blue, alpha); }
        void set_color     (VGColor color) { nx_gc_set_color(m_gc.get(), color.r(), color.g(), color.b(), color.a()); }
        void set_line_width(double width) { nx_gc_set_line_width(m_gc.get(), width); }
        void set_line_dash (int n_dashes, double* dashes, double offset) { nx_gc_set_line_dash(m_gc.get(), n_dashes, dashes, offset); }

        void move_to    (double x, double y) { nx_gc_move_to(m_gc.get(), x, y); }
        void line_to    (double x, double y) { nx_gc_line_to(m_gc.get(), x, y); }
        void rel_move_to(double dx, double dy) { nx_gc_rel_move_to(m_gc.get(), dx, dy); }
        void rel_line_to(double dx, double dy) { nx_gc_rel_line_to(m_gc.get(), dx, dy); }
        void line       (double x_s, double y_s, double x_e, double y_e) { nx_gc_line(m_gc.get(), x_s, y_s, x_e, y_e); }
        void rectangle  (double x, double y, double width, double height) { nx_gc_rectangle(m_gc.get(), x, y, width, height); }

        void arc         (double x, double y, double radius, double angle_s, double angle_e) { nx_gc_arc(m_gc.get(), x, y, radius, angle_s, angle_e); }
        void circle      (double x, double y, double radius) { nx_gc_circle(m_gc.get(), x, y, radius); }
        void curve_to    (double x_e, double y_e, double c1x, double c1y, double c2x, double c2y) { nx_gc_curve_to(m_gc.get(), x_e, y_e, c1x, c1y, c2x, c2y); }
        void rel_curve_to(double dx_e, double dy_e, double c1dx, double c1dy, double c2dx, double c2dy) { nx_gc_rel_curve_to(m_gc.get(), dx_e, dy_e, c1dx, c1dy, c2dx, c2dy); }
        void curve       (double x_s, double y_s, double x_e, double y_e, double c1x, double c1y, double c2x, double c2y) { nx_gc_curve(m_gc.get(), x_s, y_s, x_e, y_e, c1x, c1y, c2x, c2y); }

        void draw_text    (double x, double y, const std::string& txt, double font_sz) { nx_gc_draw_text(m_gc.get(), x, y, txt.c_str(), font_sz); }
        void set_text_size(const std::string& txt, double font_sz, double* width, double* height) { nx_gc_set_text_size(m_gc.get(), txt.c_str(), font_sz, width, height); }

private:
        explicit VGGraphicsContext(struct NXGC* nx_gc);

        std::shared_ptr<struct NXGC> m_gc;
};

}
}

#endif
