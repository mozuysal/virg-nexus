/**
 * @file vg_image_annotator.hpp
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
