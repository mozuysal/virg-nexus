/**
 * @file vg_graphics_context.cc
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
#include "virg/nexus/vg_graphics_context.hpp"

using std::string;
using std::shared_ptr;

namespace virg {
namespace nexus {

const double VGGraphicsContext::A4_WIDTH_IN_PTS = NX_GC_MM_TO_POINTS*NX_GC_A4_WIDTH;
const double VGGraphicsContext::A4_HEIGHT_IN_PTS = NX_GC_MM_TO_POINTS*NX_GC_A4_HEIGHT;

VGGraphicsContext::VGGraphicsContext(struct NXGC* nx_gc)
        : m_gc(nx_gc, nx_gc_free)
{}

VGGraphicsContext::~VGGraphicsContext()
{}

VGGraphicsContext VGGraphicsContext::new_from_image(VGImage rgba)
{
        VGGraphicsContext gc(nx_gc_new(rgba.nx_img()));
        return gc;
}

VGGraphicsContext VGGraphicsContext::new_pdf(const string& filename,
                                             double width, double height)
{
        VGGraphicsContext gc(nx_gc_new_pdf(filename.c_str(), width, height));
        return gc;
}

VGGraphicsContext VGGraphicsContext::new_svg(const string& filename,
                                             double width, double height)
{
        VGGraphicsContext gc(nx_gc_new_svg(filename.c_str(), width, height));
        return gc;
}

}
}
