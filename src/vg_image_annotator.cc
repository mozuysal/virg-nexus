/**
 * @file vg_image_annotator.cc
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
#include "virg/nexus/vg_image_annotator.hpp"

namespace virg {
namespace nexus {

enum MarkType { MARK_NONE = -1, MARK_POINT= 0, MARK_SQUARE,
                MARK_DIAMOND, MARK_PLUS, MARK_CROSS };

VGImageAnnotator::VGImageAnnotator(const VGImage& image)
        : m_canvas(image.clone().convert_type_to(VGImage::RGBA)),
          m_gc(VGGraphicsContext::new_from_image(m_canvas))
{}

void VGImageAnnotator::set_image(const VGImage& image)
{
        m_canvas = image.clone();
        m_canvas.convert_type_to(VGImage::RGBA);

        m_gc = VGGraphicsContext::new_from_image(m_canvas);
}


void VGImageAnnotator::draw_mark(MarkType type, double x, double y,
                                 double size, VGColor color)
{
        m_gc.set_color(color);
        m_gc.set_line_width(1.5);
        m_gc.new_path();

        double r = size / 2.0;

        switch (type) {
        case MARK_POINT:
                m_gc.circle(x, y, r);
                break;
        case MARK_SQUARE:
                m_gc.rectangle(x-r, y-r, size, size);
                break;
        case MARK_DIAMOND:
                m_gc.move_to(x, y-r);
                m_gc.line_to(r, r);
                m_gc.line_to(-r, r);
                m_gc.line_to(-r, -r);
                m_gc.line_to(r, -r);
                break;
        case MARK_PLUS:
                m_gc.move_to(x, y-r);
                m_gc.line_to(0.0, size);
                m_gc.move_to(x-r, y);
                m_gc.line_to(size, 0.0);
                break;
        case MARK_CROSS:
                m_gc.move_to(x-r, y-r);
                m_gc.line_to(size, size);
                m_gc.move_to(x+r, y-r);
                m_gc.line_to(-size, size);
                break;
        default:
                break;
        }

        m_gc.stroke();
}

void VGImageAnnotator::draw_edge(double xs, double ys, double xe, double ye,
                                 double width, VGColor color, MarkType end_marks,
                                 double mark_size)
{
        m_gc.set_color(color);
        m_gc.set_line_width(width);
        m_gc.new_path();

        m_gc.line(xs, ys, xe, ye);
        draw_mark(end_marks, xs, ys, mark_size, color);
        draw_mark(end_marks, xe, ye, mark_size, color);

        m_gc.stroke();
}

void VGImageAnnotator::draw_roi(double x_tl, double y_tl,
                                double width, double height,
                                double line_width, VGColor color)
{
        m_gc.set_color(color);
        m_gc.set_line_width(width);
        m_gc.new_path();

        m_gc.rectangle(x_tl, y_tl, width, height);

        m_gc.stroke();
}

void VGImageAnnotator::draw_keypoint(struct NXKeypoint *key, VGColor color)
{
        const double BASE_SIZE = 2.0;

        double x = nx_keypoint_xs0(key);
        double y = nx_keypoint_ys0(key);
        double size = key->sigma*key->scale*BASE_SIZE;
        draw_mark(MARK_POINT, x, y, size, color);
}

VGColor VGImageAnnotator::color_from_keypoint_level(int level)
{
        switch (level) {
        case 0: return VGColor::red();
        case 1: return VGColor::blue();
        case 2: return VGColor::green();
        case 3: return VGColor::yellow();
        case 4: return VGColor::purple();
        case 5: return VGColor::cyan();
        case 6: return VGColor::white();
        default:
                return VGColor::random();
        }
}

void VGImageAnnotator::draw_keypoints(int n_keys, struct NXKeypoint *key,
                                      bool single_color)
{
        for (int i = 0; i < n_keys; ++i) {
                struct NXKeypoint* key = key + i;
                VGColor color = single_color ? VGColor::white()
                        : color_from_keypoint_level(key->level);
                draw_keypoint(key, color);
        }
}

}
}