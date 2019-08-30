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

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_image_io.h"

namespace virg {
namespace nexus {

class VGImage {
public:
        enum Type { GRAYSCALE, RGBA };
        enum DataType { UCHAR, FLOAT32 };
        enum LoadMode { LOAD_AS_IS = -1, LOAD_GRAYSCALE = 0, LOAD_RGBA };

        static const int STRIDE_DEFAULT = NX_IMAGE_STRIDE_DEFAULT;

        VGImage();
        VGImage(const VGImage& img) = delete;
        VGImage(const VGImage&& img) = delete;
        ~VGImage();

        VGImage& operator=(const VGImage& img) = delete;
        VGImage& operator=(const VGImage&& img) = delete;

        void create(int width, int height, int row_stride,
                    Type type, DataType dtype);
        void create_gray(int width, int height, DataType dtype);
        void create_rgba(int width, int height, DataType dtype);
        void create_like(const VGImage& src);
        void copy_of(const VGImage& img);
        void swap_with(VGImage& img);
        template <typename T> void wrap(T* data, int width, int height,
                                        int row_stride, Type type,
                                        DataType dtype, bool own_memory);
        void convert_type_to(Type type);
        void release();

        int width () const { return m_img->width; }
        int height() const { return m_img->height; }
        enum Type type() const { return type_from_nx_image_type(m_img->type); }
        enum DataType data_type() const { return data_type_from_nx_image_data_type(m_img->dtype); }
        int n_channels() const { return m_img->n_channels; }
        int row_stride() const { return m_img->row_stride; }

        template <typename T> inline       T* data() ;
        template <typename T> inline const T* data() const ;

        struct NXImage*       nx_img()       { return m_img; }
        const struct NXImage* nx_img() const { return m_img; }

        void set_zero();
        void normalize_to_zero_one(bool symmetric_around_zero);
        void axpy(float a, float y);

        void scaled_of(const VGImage& src, float scale_f);
        void downsampled_from(const VGImage& src);
        void filter_box(const VGImage& src, int sum_radius_x, int sum_radius_y);
        void filter_box_y(const VGImage& src, int sum_radius);
        void filter_box_x(const VGImage& src, int sum_radius);
        void smooth(const VGImage& src, float sigma_x, float sigma_y);
        void deriv_x_of(const VGImage& src);
        void deriv_y_of(const VGImage& src);

        void xsave(const std::string& filename);
        bool save(const std::string& filename);
        void xload(const std::string& filename, enum LoadMode mode);
        bool load(const std::string& filename, enum LoadMode mode);

        static inline Type     type_from_nx_image_type(enum NXImageType nx_type);
        static inline DataType data_type_from_nx_image_data_type(enum NXImageDataType nx_data_type);
        static inline enum NXImageType type_to_nx_image_type(Type vg_type);
        static inline enum NXImageDataType data_type_to_nx_image_data_type(DataType vg_data_type);
        static inline enum NXImageLoadMode load_mode_to_nx_load_mode(LoadMode mode);

private:
        struct NXImage * m_img;
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
