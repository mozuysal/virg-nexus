/**
 * @file vg_image.cc
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

void VGImage::create(int width, int height,
                     int row_stride, VGImage::Type type,
                     VGImage::DataType dtype)
{
        nx_image_resize(m_img.get(), width, height, row_stride,
                        VGImage::type_to_nx_image_type(type),
                        VGImage::data_type_to_nx_image_data_type(dtype));
}

void VGImage::create_gray(int width, int height, VGImage::DataType dtype)
{
        this->create(width, height, VGImage::STRIDE_DEFAULT, VGImage::GRAYSCALE, dtype);
}

void VGImage::create_rgba(int width, int height, VGImage::DataType dtype)
{
        this->create(width, height, VGImage::STRIDE_DEFAULT, VGImage::RGBA, dtype);
}

void VGImage::create_like(const VGImage& src)
{
        this->create(src.width(), src.height(), src.row_stride(),
                     src.type(), src.data_type());
}

void VGImage::copy_of(const VGImage& img)
{
        if (&img != this)
                nx_image_copy(m_img.get(), img.m_img.get());
}

VGImage VGImage::clone() const
{
        VGImage image;
        image.copy_of(*this);
        return image;
}

template <>
void VGImage::wrap<uchar>(uchar* data, int width, int height,
                          int row_stride, VGImage::Type type,
                          VGImage::DataType dtype, bool own_memory)
{
        nx_image_wrap(m_img.get(), static_cast<void*>(data), width, height, row_stride,
                      VGImage::type_to_nx_image_type(type),
                      VGImage::data_type_to_nx_image_data_type(dtype),
                      static_cast<NXBool>(own_memory));
}

void VGImage::wrap(struct NXImage* img, bool own_memory)
{
        if (own_memory) {
                shared_ptr<struct NXImage> ptr(img, nx_image_free);
                m_img = ptr;
        } else {
                shared_ptr<struct NXImage> ptr(img, [](struct NXImage *ptr) {});
                m_img = ptr;
        }
}

template <>
void VGImage::wrap<float>(float* data, int width, int height,
                          int row_stride, VGImage::Type type,
                          VGImage::DataType dtype, bool own_memory)
{
        nx_image_wrap(m_img.get(), static_cast<void*>(data), width, height, row_stride,
                      VGImage::type_to_nx_image_type(type),
                      VGImage::data_type_to_nx_image_data_type(dtype),
                      static_cast<NXBool>(own_memory));
}

void VGImage::swap_with(VGImage& img)
{
        std::swap(m_img, img.m_img);
}


void VGImage::release()
{
        nx_image_release(m_img.get());
}


void VGImage::convert_type_to(VGImage::Type type)
{
        nx_image_convert_type(m_img.get(), VGImage::type_to_nx_image_type(type));
}


/*
void VGImage::apply_colormap(struct NXImage* color, struct NXImage* gray,
                             enum NXColorMap map);
*/

void VGImage::set_zero()
{
        nx_image_set_zero(m_img.get());
}

void VGImage::normalize_to_zero_one(bool symmetric_around_zero)
{
        nx_image_normalize_to_zero_one(m_img.get(), static_cast<NXBool>(symmetric_around_zero));
}

void VGImage::axpy(float a, float y)
{
        nx_image_axpy(m_img.get(), a, y);
}

void VGImage::scaled_of(const VGImage& src, float scale_f)
{
        nx_image_scale(m_img.get(), src.m_img.get(), scale_f);
}

void VGImage::downsampled_from(const VGImage& src)
{
        nx_image_downsample(m_img.get(), src.m_img.get());
}

void VGImage::smooth(const VGImage& src, float sigma_x, float sigma_y)
{
        nx_image_smooth(m_img.get(), src.m_img.get(), sigma_x, sigma_y, NULL);
}

void VGImage::filter_box_x(const VGImage& src, int sum_radius)
{
        nx_image_filter_box_x(m_img.get(), src.m_img.get(), sum_radius, NULL);
}

void VGImage::filter_box_y(const VGImage& src, int sum_radius)
{
        nx_image_filter_box_y(m_img.get(), src.m_img.get(), sum_radius, NULL);
}

void VGImage::filter_box(const VGImage& src, int sum_radius_x, int sum_radius_y)
{
        nx_image_filter_box(m_img.get(), src.m_img.get(), sum_radius_x, sum_radius_y, NULL);
}

void VGImage::deriv_x_of(const VGImage& src)
{
        nx_image_deriv_x(m_img.get(), src.m_img.get());
}

void VGImage::deriv_y_of(const VGImage& src)
{
        nx_image_deriv_y(m_img.get(), src.m_img.get());
}

void VGImage::xsave(const std::string& filename)
{
        nx_image_xsave(m_img.get(), filename.c_str());
}

bool VGImage::save(const std::string& filename)
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