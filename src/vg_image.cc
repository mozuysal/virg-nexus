/**
 * @file vg_image.cc
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
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/vg_image.hpp"

#include <algorithm>

using std::shared_ptr;

namespace virg {
namespace nexus {

VGImage::VGImage()
        : m_img(nx_image_alloc(), nx_image_free)
{}

VGImage::VGImage(struct NXImage* nx_img, bool own_memory)
{
        this->wrap(nx_img, own_memory);
}

VGImage::~VGImage()
{}

VGImage& VGImage::create(int width, int height,
                         int row_stride, VGImage::Type type,
                         VGImage::DataType dtype)
{
        nx_image_resize(m_img.get(), width, height, row_stride,
                        VGImage::type_to_nx_image_type(type),
                        VGImage::data_type_to_nx_image_data_type(dtype));
        return *this;
}

VGImage& VGImage::create_gray(int width, int height, VGImage::DataType dtype)
{
        this->create(width, height, VGImage::STRIDE_DEFAULT, VGImage::GRAYSCALE, dtype);
        return *this;
}

VGImage& VGImage::create_rgba(int width, int height, VGImage::DataType dtype)
{
        this->create(width, height, VGImage::STRIDE_DEFAULT, VGImage::RGBA, dtype);
        return *this;
}

VGImage& VGImage::create_like(const VGImage& src)
{
        this->create(src.width(), src.height(), src.row_stride(),
                     src.type(), src.data_type());
        return *this;
}

VGImage& VGImage::copy_of(const VGImage& img)
{
        if (&img != this)
                nx_image_copy(m_img.get(), img.m_img.get());
        return *this;
}

VGImage VGImage::clone() const
{
        VGImage image;
        image.copy_of(*this);
        return image;
}

template <>
VGImage& VGImage::wrap<uchar>(uchar* data, int width, int height,
                              int row_stride, VGImage::Type type,
                              VGImage::DataType dtype, bool own_memory)
{
        nx_image_wrap(m_img.get(), static_cast<void*>(data), width, height, row_stride,
                      VGImage::type_to_nx_image_type(type),
                      VGImage::data_type_to_nx_image_data_type(dtype),
                      static_cast<NXBool>(own_memory));
        return *this;
}

VGImage& VGImage::wrap(struct NXImage* img, bool own_memory)
{
        if (own_memory) {
                shared_ptr<struct NXImage> ptr(img, nx_image_free);
                m_img = ptr;
        } else {
                shared_ptr<struct NXImage> ptr(img, [](struct NXImage */* ptr */) {});
                m_img = ptr;
        }
        return *this;
}

template <>
VGImage& VGImage::wrap<float>(float* data, int width, int height,
                              int row_stride, VGImage::Type type,
                              VGImage::DataType dtype, bool own_memory)
{
        nx_image_wrap(m_img.get(), static_cast<void*>(data), width, height, row_stride,
                      VGImage::type_to_nx_image_type(type),
                      VGImage::data_type_to_nx_image_data_type(dtype),
                      static_cast<NXBool>(own_memory));
        return *this;
}

VGImage& VGImage::swap_with(VGImage& img)
{
        std::swap(m_img, img.m_img);
        return *this;
}


VGImage& VGImage::release()
{
        nx_image_release(m_img.get());
        return *this;
}


VGImage& VGImage::convert_type_to(VGImage::Type type)
{
        nx_image_convert_type(m_img.get(), VGImage::type_to_nx_image_type(type));
        return *this;
}


VGImage& VGImage::apply_colormap(VGImage& color, VGImage& gray,
                                 VGColorMap::Type map)
{
        nx_image_apply_colormap(color.nx_img(), gray.nx_img(),
                                VGColorMap::type_to_nx_type(map));
        return *this;
}

VGImage& VGImage::set_zero()
{
        nx_image_set_zero(m_img.get());
        return *this;
}

VGImage& VGImage::normalize_to_zero_one(bool symmetric_around_zero)
{
        nx_image_normalize_to_zero_one(m_img.get(), static_cast<NXBool>(symmetric_around_zero));
        return *this;
}

VGImage& VGImage::axpy(float a, float y)
{
        nx_image_axpy(m_img.get(), a, y);
        return *this;
}

VGImage& VGImage::scaled_of(const VGImage& src, float scale_f)
{
        nx_image_scale(m_img.get(), src.m_img.get(), scale_f);
        return *this;
}

VGImage& VGImage::downsampled_from(const VGImage& src)
{
        nx_image_downsample(m_img.get(), src.m_img.get());
        return *this;
}

VGImage& VGImage::smooth(const VGImage& src, float sigma_x,
                         float sigma_y, float kernel_truncation_factor)
{
        nx_image_smooth(m_img.get(), src.m_img.get(), sigma_x,
                        sigma_y, kernel_truncation_factor, NULL);
        return *this;
}

VGImage& VGImage::filter_box_x(const VGImage& src, int sum_radius)
{
        nx_image_filter_box_x(m_img.get(), src.m_img.get(), sum_radius, NULL);
        return *this;
}

VGImage& VGImage::filter_box_y(const VGImage& src, int sum_radius)
{
        nx_image_filter_box_y(m_img.get(), src.m_img.get(), sum_radius, NULL);
        return *this;
}

VGImage& VGImage::filter_box(const VGImage& src, int sum_radius_x, int sum_radius_y)
{
        nx_image_filter_box(m_img.get(), src.m_img.get(), sum_radius_x, sum_radius_y, NULL);
        return *this;
}

VGImage& VGImage::deriv_x_of(const VGImage& src)
{
        nx_image_deriv_x(m_img.get(), src.m_img.get());
        return *this;
}

VGImage& VGImage::deriv_y_of(const VGImage& src)
{
        nx_image_deriv_y(m_img.get(), src.m_img.get());
        return *this;
}

void VGImage::xsave(const std::string& filename) const
{
        nx_image_xsave(m_img.get(), filename.c_str());
}

bool VGImage::save(const std::string& filename) const
{
        return NX_OK == nx_image_save(m_img.get(), filename.c_str());
}

void VGImage::xload(const std::string& filename, enum VGImage::LoadMode mode)
{
        nx_image_xload(m_img.get(), filename.c_str(), VGImage::load_mode_to_nx_load_mode(mode));
}

bool VGImage::load(const std::string& filename, enum VGImage::LoadMode mode)
{
        return NX_OK == nx_image_load(m_img.get(), filename.c_str(), VGImage::load_mode_to_nx_load_mode(mode));
}

}
}
