/**
 * @file vg_image_annotator.hpp
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
 * along with VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_VG_IMAGE_ANNOTATOR_HPP
#define VIRG_NEXUS_VG_IMAGE_ANNOTATOR_HPP

#include "virg/nexus/nx_keypoint.h"

#include "virg/nexus/vg_color.hpp"
#include "virg/nexus/vg_image.hpp"
#include "virg/nexus/vg_point_correspondence_2d.hpp"
#include "virg/nexus/vg_graphics_context.hpp"

namespace virg {
namespace nexus {

class VGImageAnnotator {
public:
        enum MarkType { MARK_NONE = -1, MARK_POINT = 0, MARK_SQUARE,
                        MARK_DIAMOND, MARK_PLUS, MARK_CROSS };

        explicit VGImageAnnotator(const VGImage& image);
        ~VGImageAnnotator() {}

        static VGImageAnnotator create_match_image(const VGImage& image_left,
                                                   const VGImage& image_right,
                                                   const VGPointCorrespondence2D& corr,
                                                   bool only_inliers);

        int width () const { return m_canvas.width(); }
        int height() const { return m_canvas.height(); }

        void set_image(const VGImage& image);
        const VGImage& get_canvas() { m_gc.flush(); return m_canvas; }

        void draw_mark(MarkType type, double x, double y, double size, VGColor color);
        void draw_edge(double xs, double ys, double xe, double ye, double width,
                       VGColor color, MarkType end_marks = MARK_NONE, double mark_size = 0);
        void draw_roi(double x_tl, double y_tl, double width, double height,
                      double line_width, VGColor color);

        void draw_keypoint (struct NXKeypoint *key, VGColor color);
        void draw_keypoints(int n_keys, struct NXKeypoint *key, bool single_color);
private:
        VGColor color_from_keypoint_level(int level);

        VGImage m_canvas;
        VGGraphicsContext m_gc;
};

}
}

#endif
