/**
 * @file vg_image.hpp
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
#ifndef VIRG_NEXUS_VG_IMAGE_HPP
#define VIRG_NEXUS_VG_IMAGE_HPP

#include <string>
#include <memory>

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_image_io.h"

#include "virg/nexus/vg_color_map.hpp"

namespace virg {
namespace nexus {

class VGImage {
public:
        enum Type { GRAYSCALE, RGBA };
        enum DataType { UCHAR, FLOAT32 };
        enum LoadMode { LOAD_AS_IS = -1, LOAD_GRAYSCALE = 0, LOAD_RGBA };

        static const int STRIDE_DEFAULT = NX_IMAGE_STRIDE_DEFAULT;

        VGImage();
        VGImage(struct NXImage* nx_img, bool own_memory);
        ~VGImage();

        VGImage& create(int width, int height, int row_stride,
                       Type type, DataType dtype);
        VGImage& create_gray(int width, int height, DataType dtype);
        VGImage& create_rgba(int width, int height, DataType dtype);
        VGImage& create_like(const VGImage& src);
        VGImage& copy_of(const VGImage& img);
        VGImage  clone() const;
        VGImage& swap_with(VGImage& img);
        VGImage& convert_type_to(Type type);
        VGImage& release();

        int width () const { return m_img->width; }
        int height() const { return m_img->height; }
        enum Type type() const { return type_from_nx_image_type(m_img->type); }
        enum DataType data_type() const { return data_type_from_nx_image_data_type(m_img->dtype); }
        int n_channels() const { return m_img->n_channels; }
        int row_stride() const { return m_img->row_stride; }

        template <typename T> inline       T* data() ;
        template <typename T> inline const T* data() const ;
        template <typename T> inline       T* data(int y) ;
        template <typename T> inline const T* data(int y) const ;

        struct NXImage*       nx_img()       { return m_img.get(); }
        const struct NXImage* nx_img() const { return m_img.get(); }
        VGImage& wrap(struct NXImage* img, bool own_memory);
        template <typename T> VGImage& wrap(T* data, int width, int height,
                                            int row_stride, Type type,
                                            DataType dtype, bool own_memory);

        VGImage& set_zero();
        VGImage& normalize_to_zero_one(bool symmetric_around_zero);
        VGImage& axpy(float a, float y);
        VGImage& apply_colormap(VGImage& color, VGImage& gray,
                                VGColorMap::Type map);

        VGImage& scaled_of(const VGImage& src, float scale_f);
        VGImage& downsampled_from(const VGImage& src);
        VGImage& filter_box(const VGImage& src, int sum_radius_x, int sum_radius_y);
        VGImage& filter_box_y(const VGImage& src, int sum_radius);
        VGImage& filter_box_x(const VGImage& src, int sum_radius);
        VGImage& smooth(const VGImage& src, float sigma_x, float sigma_y);
        VGImage& deriv_x_of(const VGImage& src);
        VGImage& deriv_y_of(const VGImage& src);

        void xsave(const std::string& filename) const;
        bool save(const std::string& filename) const;
        void xload(const std::string& filename, enum LoadMode mode);
        bool load(const std::string& filename, enum LoadMode mode);

        static inline Type     type_from_nx_image_type(enum NXImageType nx_type);
        static inline DataType data_type_from_nx_image_data_type(enum NXImageDataType nx_data_type);
        static inline enum NXImageType type_to_nx_image_type(Type vg_type);
        static inline enum NXImageDataType data_type_to_nx_image_data_type(DataType vg_data_type);
        static inline enum NXImageLoadMode load_mode_to_nx_load_mode(LoadMode mode);

private:
        std::shared_ptr<struct NXImage> m_img;
};

template <> inline const uchar* VGImage::data<uchar>() const
{
        return m_img->data.uc;
}

template <> inline const float* VGImage::data<float>() const
{
        return m_img->data.f32;
}

template <> inline uchar* VGImage::data<uchar>()
{
        return m_img->data.uc;
}

template <> inline float* VGImage::data<float>()
{
        return m_img->data.f32;
}

template <> inline uchar* VGImage::data<uchar>(int y)
{
        return m_img->data.uc + y*m_img->row_stride;
}

template <> inline const uchar* VGImage::data<uchar>(int y) const
{
        return m_img->data.uc + y*m_img->row_stride;
}

template <> inline float* VGImage::data<float>(int y)
{
        return m_img->data.f32 + y*m_img->row_stride;
}

template <> inline const float* VGImage::data<float>(int y) const
{
        return m_img->data.f32 + y*m_img->row_stride;
}

inline VGImage::Type VGImage::type_from_nx_image_type(enum NXImageType nx_type) {
        switch(nx_type) {
        case NX_IMAGE_GRAYSCALE: return GRAYSCALE;
        case NX_IMAGE_RGBA: return RGBA;
        default:
                NX_FATAL(VG_LOG_TAG, "Unknown NXImageType!");
        }
}

inline VGImage::DataType VGImage::data_type_from_nx_image_data_type(enum NXImageDataType nx_data_type) {
        switch(nx_data_type) {
        case NX_IMAGE_UCHAR: return UCHAR;
        case NX_IMAGE_FLOAT32: return FLOAT32;
        default:
                NX_FATAL(VG_LOG_TAG, "Unknown NXImageDataType!");
        }
}

inline enum NXImageType VGImage::type_to_nx_image_type(VGImage::Type vg_type)
{
        switch(vg_type) {
        case GRAYSCALE: return NX_IMAGE_GRAYSCALE;
        case RGBA: return NX_IMAGE_RGBA;
        default:
                NX_FATAL(VG_LOG_TAG, "Unknown VGImage::Type!");
        }
}

inline enum NXImageDataType VGImage::data_type_to_nx_image_data_type(VGImage::DataType vg_data_type)
{
        switch(vg_data_type) {
        case UCHAR: return NX_IMAGE_UCHAR;
        case FLOAT32: return NX_IMAGE_FLOAT32;
        default:
                NX_FATAL(VG_LOG_TAG, "Unknown VGImage::DataType!");
        }
}

inline enum NXImageLoadMode VGImage::load_mode_to_nx_load_mode(VGImage::LoadMode mode)
{
        switch(mode) {
        case LOAD_AS_IS: return NX_IMAGE_LOAD_AS_IS;
        case LOAD_GRAYSCALE: return NX_IMAGE_LOAD_GRAYSCALE;
        case LOAD_RGBA: return NX_IMAGE_LOAD_RGBA;
        default:
                NX_FATAL(VG_LOG_TAG, "Unknown VGImage::LoadMode!");
        }
}


}
}

#endif
