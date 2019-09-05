/**
 * @file vg_image_pyr.cc
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
