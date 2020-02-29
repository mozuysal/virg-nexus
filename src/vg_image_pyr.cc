/**
 * @file vg_image_pyr.cc
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
#include "virg/nexus/vg_image_pyr.hpp"

using std::shared_ptr;

namespace virg {
namespace nexus {

VGImagePyr::VGImagePyr()
        : m_pyr(nx_image_pyr_alloc(), nx_image_pyr_free),
          m_builder(nx_image_pyr_builder_alloc(), nx_image_pyr_builder_free)
{}

VGImagePyr::~VGImagePyr()
{}

VGImagePyr::Type VGImagePyr::type() const
{
        switch (m_builder->type) {
        case NX_IMAGE_PYR_BUILDER_FAST: return VGImagePyr::FAST;
        case NX_IMAGE_PYR_BUILDER_FINE: return VGImagePyr::FINE;
        case NX_IMAGE_PYR_BUILDER_SCALED: return VGImagePyr::SCALED;
        default:
                NX_FATAL(VG_LOG_TAG, "Invalid VGImagePyr type!");
        }
}


VGImagePyr VGImagePyr::build_fast_from(const VGImage& image, int n_levels, float sigma0)
{
        VGImagePyr pyr;

        shared_ptr<struct NXImagePyrBuilder> bptr(nx_image_pyr_builder_new_fast(n_levels, sigma0),
                                                  nx_image_pyr_builder_free);
        pyr.m_builder = bptr;
        shared_ptr<struct NXImagePyr> ptr(nx_image_pyr_builder_build0(pyr.m_builder.get(), image.nx_img()),
                                          nx_image_pyr_free);
        pyr.m_pyr = ptr;

        return pyr;
}

VGImagePyr VGImagePyr::build_fine_from(const VGImage& image, int n_octaves, int n_octave_steps, float sigma0)
{
        VGImagePyr pyr;

        shared_ptr<struct NXImagePyrBuilder> bptr(nx_image_pyr_builder_new_fine(n_octaves, n_octave_steps, sigma0),
                                                  nx_image_pyr_builder_free);
        pyr.m_builder = bptr;
        shared_ptr<struct NXImagePyr> ptr(nx_image_pyr_builder_build0(pyr.m_builder.get(), image.nx_img()),
                                          nx_image_pyr_free);
        pyr.m_pyr = ptr;

        return pyr;
}

VGImagePyr VGImagePyr::build_scaled_from(const VGImage& image, int n_levels, float scale_factor, float sigma0)
{
        VGImagePyr pyr;

        shared_ptr<struct NXImagePyrBuilder> bptr(nx_image_pyr_builder_new_scaled(n_levels, scale_factor, sigma0),
                                                  nx_image_pyr_builder_free);
        pyr.m_builder = bptr;
        shared_ptr<struct NXImagePyr> ptr(nx_image_pyr_builder_build0(pyr.m_builder.get(), image.nx_img()),
                                          nx_image_pyr_free);
        pyr.m_pyr = ptr;

        return pyr;
}

void VGImagePyr::rebuild()
{
        nx_image_pyr_builder_update(m_builder.get(), m_pyr.get());
}

void VGImagePyr::rebuild_from(const VGImage& image)
{
        nx_image_pyr_builder_build(m_builder.get(), m_pyr.get(), image.nx_img());
}

}
}
