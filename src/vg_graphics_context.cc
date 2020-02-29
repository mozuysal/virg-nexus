/**
 * @file vg_graphics_context.cc
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
