/**
 * @file nx_affine_warp_processor.c
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2015 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_affine_warp_processor.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef USE_SSE
# include <smmintrin.h>
#endif

#ifdef _OPENMP
#  include <omp.h>
#else
# define omp_get_thread_num() 0
# define omp_get_max_threads() 1
#endif

#include <virg/nexus/nx_assert.h>
#include <virg/nexus/nx_alloc.h>
#include <virg/nexus/nx_filter.h>
#include <virg/nexus/nx_image.h>
#include <virg/nexus/nx_math.h>
#include <virg/nexus/nx_uniform_sampler.h>

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

enum NXAWPBgMode {
        NX_AWP_BG_FIXED = 0,
        NX_AWP_BG_REPEAT = 1,
        NX_AWP_BG_NOISE = 2
};

struct NXAffineWarpProcessor
{
        const struct NXImage *image;

        struct NXImage *skew_rotation_buffer;
        struct NXImage *subsample_buffer;
        struct NXImage *result_buffer;

        double forward_t[6];
        double inverse_t[6];

        enum NXAWPBgMode bg_mode;
        uchar bg_color;
        float post_blur_sigma;
};

static inline void nx_affine_warp_processor_reset_transforms(struct NXAffineWarpProcessor *wp);
static inline void nx_affine_warp_processor_update_transforms(struct NXAffineWarpProcessor *wp, const double *center_in, const double *center_out, double scale_x, double scale_y, double angle);
static inline void nx_affine_warp_processor_update_inverse_transform(struct NXAffineWarpProcessor *wp, const double *center_in, const double *center_out, double scale_x, double scale_y, double angle);
static inline void nx_affine_warp_processor_update_forward_transform(struct NXAffineWarpProcessor *wp, const double *center_in, const double *center_out, double scale_x, double scale_y, double angle);
static inline void nx_affine_warp_processor_transformed_buffer_size(struct NXAffineWarpProcessor *wp, int *width, int *height,
                                                                    double scale_x, double scale_y, double angle, const double *current_t);
static inline void nx_affine_warp_processor_resize_buffers(struct NXAffineWarpProcessor *wp,
                                                           double scale, double planar_angle, double tilt, double tilt_angle);
static inline void nx_affine_warp_processor_resize_skew_rotation_buffer(struct NXAffineWarpProcessor *wp, double tilt_angle);
static inline void nx_affine_warp_processor_resize_subsample_buffer(struct NXAffineWarpProcessor *wp, double tilt);
static inline void nx_affine_warp_processor_resize_result_buffer(struct NXAffineWarpProcessor *wp, double scale, double planar_angle);
static inline void nx_affine_warp_processor_resize_buffer(struct NXAffineWarpProcessor *wp, const struct NXImage *in_buffer,
                                                          struct NXImage *out_buffer, double scale_x, double scale_y, double angle);

static inline void compute_skew_rotation_buffer(const struct NXImage* img,
                                                struct NXImage* buffer,
                                                float tilt_angle,
                                                float tilt);
static inline void compute_subsample_buffer(struct NXImage* in_buffer,
                                            struct NXImage* out_buffer,
                                            float tilt, float scale);
static inline void compute_result_buffer(struct NXImage* in_buffer,
                                         struct NXImage* res_buffer,
                                         float scale,
                                         float planar_angle,
                                         float post_blur_sigma);
static inline void fill_warp_buffer_bg(const struct NXImage* image,
                                       struct NXImage* warp_buffer,
                                       float t0, float t1, float t2,
                                       float t3, float t4, float t5,
                                       enum NXAWPBgMode bg_mode, int bg_color);

struct NXAffineWarpProcessor *nx_affine_warp_processor_new()
{
        struct NXAffineWarpProcessor *wp = NX_NEW(1, struct NXAffineWarpProcessor);
        wp->image = NULL;
        wp->skew_rotation_buffer = nx_image_alloc();
        wp->subsample_buffer = nx_image_alloc();
        wp->result_buffer = nx_image_alloc();

        nx_affine_warp_processor_reset_transforms(wp);

        wp->bg_mode = NX_AWP_BG_NOISE;
        wp->bg_color = 0;
        wp->post_blur_sigma = 0.0f;

        return wp;
}

void nx_affine_warp_processor_free(struct NXAffineWarpProcessor *wp)
{
        if (wp) {
                nx_image_free(wp->skew_rotation_buffer);
                nx_image_free(wp->subsample_buffer);
                nx_image_free(wp->result_buffer);
                nx_free(wp);
        }
}

void nx_affine_warp_processor_warp(struct NXAffineWarpProcessor *wp, const struct NXImage *img, struct NXAffineWarpParam param)
{
        NX_ASSERT_PTR(wp);
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(img->data.v);
        NX_IMAGE_ASSERT_GRAYSCALE_UCHAR(img);

        wp->image = img;
        nx_affine_warp_processor_resize_buffers(wp, param.scale, param.planar_angle, param.tilt, param.tilt_angle);

        compute_skew_rotation_buffer(img, wp->skew_rotation_buffer, param.tilt_angle, param.tilt);
        compute_subsample_buffer(wp->skew_rotation_buffer, wp->subsample_buffer, param.tilt, param.scale);
        compute_result_buffer(wp->subsample_buffer, wp->result_buffer, param.scale, param.planar_angle, wp->post_blur_sigma);

        double *t = &wp->inverse_t[0];
        fill_warp_buffer_bg(img, wp->result_buffer,
                            t[0], t[1], t[2], t[3], t[4], t[5],
                            wp->bg_mode, wp->bg_color);
}

const struct NXImage *nx_affine_warp_processor_warp_result(struct NXAffineWarpProcessor *wp)
{
        NX_ASSERT_PTR(wp);
        return wp->result_buffer;
}

const double *nx_affine_warp_processor_forward_transform(struct NXAffineWarpProcessor *wp)
{
        NX_ASSERT_PTR(wp);
        return &wp->forward_t[0];
}

const double *nx_affine_warp_processor_inverse_transform(struct NXAffineWarpProcessor *wp)
{
        NX_ASSERT_PTR(wp);
        return &wp->inverse_t[0];
}

void nx_affine_warp_processor_set_bg_fixed(struct NXAffineWarpProcessor *wp, uchar bg_color)
{
        NX_ASSERT_PTR(wp);
        wp->bg_mode = NX_AWP_BG_FIXED;
        wp->bg_color = bg_color;
}

void nx_affine_warp_processor_set_bg_repeat(struct NXAffineWarpProcessor *wp)
{
        NX_ASSERT_PTR(wp);
        wp->bg_mode = NX_AWP_BG_REPEAT;
}

void nx_affine_warp_processor_set_bg_noise(struct NXAffineWarpProcessor *wp)
{
        NX_ASSERT_PTR(wp);
        wp->bg_mode = NX_AWP_BG_NOISE;
}

void nx_affine_warp_processor_set_post_blur_sigma(struct NXAffineWarpProcessor *wp, float sigma)
{
        NX_ASSERT_PTR(wp);
        wp->post_blur_sigma = sigma;
}

static inline void transform_set_identity(double *t)
{
        t[0] = 1.0; t[2] = 0.0; t[4] = 0.0;
        t[1] = 0.0; t[3] = 1.0; t[5] = 0.0;
}

static inline void transform_apply(const double *t, double *p)
{
        double xp = p[0]*t[0] + p[1]*t[2] + t[4];
        double yp = p[0]*t[1] + p[1]*t[3] + t[5];

        p[0] = xp;
        p[1] = yp;
}

static inline void transform_combine(double *r, const double *t0, const double *t1)
{
        double r0 = t0[0]*t1[0] + t0[2]*t1[1];
        double r1 = t0[1]*t1[0] + t0[3]*t1[1];
        double r2 = t0[0]*t1[2] + t0[2]*t1[3];
        double r3 = t0[1]*t1[2] + t0[3]*t1[3];
        double r4 = t0[0]*t1[4] + t0[2]*t1[5] + t0[4];
        double r5 = t0[1]*t1[4] + t0[3]*t1[5] + t0[5];

        r[0] = r0;
        r[1] = r1;
        r[2] = r2;
        r[3] = r3;
        r[4] = r4;
        r[5] = r5;
}

void nx_affine_warp_processor_reset_transforms(struct NXAffineWarpProcessor *wp)
{
        transform_set_identity(&wp->forward_t[0]);
        transform_set_identity(&wp->inverse_t[0]);
}

void nx_affine_warp_processor_update_transforms(struct NXAffineWarpProcessor *wp, const double *center_in,
                                                const double *center_out, double scale_x, double scale_y, double angle)
{
        nx_affine_warp_processor_update_forward_transform(wp, center_in, center_out, scale_x, scale_y, angle);
        nx_affine_warp_processor_update_inverse_transform(wp, center_in, center_out, scale_x, scale_y, angle);
}

void nx_affine_warp_processor_update_forward_transform(struct NXAffineWarpProcessor *wp, const double *center_in,
                                                       const double *center_out, double scale_x, double scale_y, double angle)
{
        double c = cos(angle);
        double s = sin(angle);

        double cx = scale_x*c;
        double sx = scale_x*s;
        double cy = scale_y*c;
        double sy = scale_y*s;
        double dx = center_out[0] - center_in[0]*cx + center_in[1]*sx;
        double dy = center_out[1] - center_in[0]*sy - center_in[1]*cy;

        double stage_t[6] = { cx, sy, -sx, cy, dx, dy };
        transform_combine(&wp->forward_t[0], &stage_t[0], &wp->forward_t[0]);
}

void nx_affine_warp_processor_update_inverse_transform(struct NXAffineWarpProcessor *wp, const double *center_in,
                                                       const double *center_out, double scale_x, double scale_y, double angle)
{
        double c = cos(angle);
        double s = sin(angle);

        double cx = c/scale_x;
        double sx = s/scale_x;
        double cy = c/scale_y;
        double sy = s/scale_y;
        double dx = center_in[0] - center_out[0]*cx - center_out[1]*sx;
        double dy = center_in[1] + center_out[0]*sy - center_out[1]*cy;

        double stage_t[6] = { cx, -sy, sx, cy, dx, dy };
        transform_combine(&wp->inverse_t[0], &wp->inverse_t[0], &stage_t[0]);
}

static inline double min4(double a, double b, double c, double d)
{
        double ab = MIN(a,b);
        double cd = MIN(c,d);
        return MIN(ab,cd);
}

static inline double max4(double a, double b, double c, double d)
{
        double ab = MAX(a,b);
        double cd = MAX(c,d);
        return MAX(ab,cd);
}

void nx_affine_warp_processor_transformed_buffer_size(struct NXAffineWarpProcessor *wp, int *width, int *height,
                                                      double scale_x, double scale_y, double angle, const double *current_t)
{
        const double BUFFER_BORDER_SIZE = 1.0;

        double tl[2] = { 0.0, 0.0 };
        double tr[2] = { (double)*width, 0.0 };
        double bl[2] = { 0.0, (double)*height };
        double br[2] = { (double)*width, (double)*height };

        transform_apply(current_t, &tl[0]);
        transform_apply(current_t, &tr[0]);
        transform_apply(current_t, &bl[0]);
        transform_apply(current_t, &br[0]);

        double c = cos(angle);
        double s = sin(angle);
        double required_t[6] = { scale_x*c, scale_y*s, -scale_x*s, scale_y*c, 0.0, 0.0 };

        transform_apply(required_t, &tl[0]);
        transform_apply(required_t, &tr[0]);
        transform_apply(required_t, &bl[0]);
        transform_apply(required_t, &br[0]);

        double x_min = min4(tl[0], tr[0], bl[0], br[0]);
        double x_max = max4(tl[0], tr[0], bl[0], br[0]);
        double y_min = min4(tl[1], tr[1], bl[1], br[1]);
        double y_max = max4(tl[1], tr[1], bl[1], br[1]);

        double rw = x_max - x_min + 2*BUFFER_BORDER_SIZE;
        double rh = y_max - y_min + 2*BUFFER_BORDER_SIZE;

        *width = (int)rw;
        *height = (int)rh;
}

void nx_affine_warp_processor_resize_buffers(struct NXAffineWarpProcessor *wp,
                                             double scale, double planar_angle, double tilt, double tilt_angle)
{
        nx_affine_warp_processor_reset_transforms(wp);

        nx_affine_warp_processor_resize_skew_rotation_buffer(wp, tilt_angle);
        nx_affine_warp_processor_resize_subsample_buffer(wp, tilt);
        nx_affine_warp_processor_resize_result_buffer(wp, scale, planar_angle);

        /* fprintf(stderr, "Buffer sizes\n%d %d\n%d %d\n%d %d\n", */
                /* wp->skew_rotation_buffer->width, wp->skew_rotation_buffer->height, */
                /* wp->subsample_buffer->width, wp->subsample_buffer->height, */
                /* wp->result_buffer->width, wp->result_buffer->height); */
}

void nx_affine_warp_processor_resize_skew_rotation_buffer(struct NXAffineWarpProcessor *wp, double tilt_angle)
{
        nx_affine_warp_processor_resize_buffer(wp, wp->image, wp->skew_rotation_buffer, 1.0, 1.0, tilt_angle);
}

void nx_affine_warp_processor_resize_subsample_buffer(struct NXAffineWarpProcessor *wp, double tilt)
{
        nx_affine_warp_processor_resize_buffer(wp, wp->skew_rotation_buffer, wp->subsample_buffer, 1.0 / tilt, 1.0, 0.0);
}

void nx_affine_warp_processor_resize_result_buffer(struct NXAffineWarpProcessor *wp, double scale, double planar_angle)
{
        nx_affine_warp_processor_resize_buffer(wp, wp->subsample_buffer, wp->result_buffer, scale, scale, planar_angle);
}

void nx_affine_warp_processor_resize_buffer(struct NXAffineWarpProcessor *wp, const struct NXImage *in_buffer,
                                            struct NXImage *out_buffer, double scale_x, double scale_y, double angle)
{
        NX_ASSERT_PTR(wp);
        NX_ASSERT_PTR(in_buffer);
        NX_ASSERT_PTR(out_buffer);

        int wi = in_buffer->width;
        int hi = in_buffer->height;
        int wo = wp->image->width;
        int ho = wp->image->height;

        nx_affine_warp_processor_transformed_buffer_size(wp, &wo, &ho, scale_x, scale_y, angle, wp->forward_t);
        nx_image_resize(out_buffer, wo, ho, -1, NX_IMAGE_GRAYSCALE, in_buffer->dtype);
        nx_image_set_zero(out_buffer);

        double center_in[2] = { wi/2.0, hi/2.0 };
        double center_out[2] = { wo/2.0, ho/2.0 };
        nx_affine_warp_processor_update_transforms(wp, &center_in[0], &center_out[0], scale_x, scale_y, angle);
}

inline static void fill_inverse_transform(float* t, float cx_in, float cy_in,
                                          float cx_out, float cy_out,
                                          float scale_x, float scale_y, float angle);

static void warp_buffer_affine_bilinear(const struct NXImage* in_buffer,
                                        struct NXImage* out_buffer,
                                        const float* t);

static void warp_processor_blur(struct NXImage *image,
                                float sigma_x, float sigma_y);

void compute_skew_rotation_buffer(const struct NXImage* img,
                                  struct NXImage* buffer,
                                  float tilt_angle,
                                  float tilt)
{
        int iw = img->width;
        int ih = img->height;

        int bw = buffer->width;
        int bh = buffer->height;

        float t[9];
        fill_inverse_transform(t, iw/2.0f, ih/2.0f, bw/2.0f, bh/2.0f, 1.0f, 1.0f, tilt_angle);
        warp_buffer_affine_bilinear(img, buffer, t);

        float sigma_x = 0.8 * sqrt(tilt*tilt-1);
        warp_processor_blur(buffer, sigma_x, 0.0f);
}

void compute_subsample_buffer(struct NXImage* in_buffer,
                              struct NXImage* out_buffer,
                              float tilt, float scale)
{
        int iw = in_buffer->width;
        int ih = in_buffer->height;

        int ow = out_buffer->width;
        int oh = out_buffer->height;

        float t[9];
        fill_inverse_transform(t, iw/2.0f, ih/2.0f, ow/2.0f, oh/2.0f, 1.0f/tilt, 1.0f, 0.0f);
        warp_buffer_affine_bilinear(in_buffer, out_buffer, t);

        if (scale > 1.0) {
                float sigma = 0.8 * sqrt(scale*scale-1);
                warp_processor_blur(out_buffer, sigma, sigma);
        }
}

void compute_result_buffer(struct NXImage* in_buffer,
                           struct NXImage* res_buffer,
                           float scale,
                           float planar_angle,
                           float post_blur_sigma)
{
        int iw = in_buffer->width;
        int ih = in_buffer->height;

        int rw = res_buffer->width;
        int rh = res_buffer->height;

        float t[9];
        fill_inverse_transform(t, iw/2.0f, ih/2.0f, rw/2.0f, rh/2.0f, scale, scale, planar_angle);
        warp_buffer_affine_bilinear(in_buffer, res_buffer, t);

        warp_processor_blur(res_buffer, post_blur_sigma, post_blur_sigma);
}

inline static void fill_inverse_transform(float* t, float cx_in, float cy_in,
                                          float cx_out, float cy_out,
                                          float scale_x, float scale_y, float angle)
{
        float c = cos(angle);
        float s = sin(angle);
        float cx = c/scale_x;
        float sx = s/scale_x;
        float cy = c/scale_y;
        float sy = s/scale_y;
        float dx = cx_in - cx_out*cx - cy_out*sx;
        float dy = cy_in + cx_out*sy - cy_out*cy;
        t[0] = cx;
        t[1] = -sy;
        t[2] = 0.0f;
        t[3] = sx;
        t[4] = cy;
        t[5] = 0.0f;
        t[6] = dx;
        t[7] = dy;
        t[8] = 1.0f;
}

static void warp_buffer_affine_bilinear(const struct NXImage* in_buffer,
                                        struct NXImage* out_buffer,
                                        const float* t)
{
#ifdef USE_SSE
        const float LAST_X = in_buffer->width - 2;
        const float LAST_Y = in_buffer->height - 2;
        __m128 BOUNDS = _mm_set_ps(LAST_Y, 0.0f, LAST_X, 0.0f);
        __m128 ONES = _mm_set1_ps(1.0f);
        __m128 T01 = _mm_set_ps(0.0f, 0.0f, t[1], t[0]);
        __m128 T34 = _mm_set_ps(0.0f, 0.0f, t[4], t[3]);
        __m128 T67 = _mm_set_ps(0.0f, 0.0f, t[7], t[6]);

        int32_t pixels[4] __attribute__ ((aligned (16)));

#ifdef _OPENMP
#pragma omp parallel for private(pixels) schedule(dynamic, 4)
#endif
        for (int y = 0; y < out_buffer->height; ++y) {
                uchar *drow = out_buffer->data.uc + y*out_buffer->row_stride;

                __m128 XY = _mm_add_ps(T67, _mm_mul_ps(T34, _mm_set1_ps(y)));
                for (int x = 0; x < out_buffer->width; ++x, XY = _mm_add_ps(XY, T01)) {
                        __m128 XPYP = _mm_unpacklo_ps(XY, XY);
                        __m128 XPIYPI = _mm_floor_ps(XPYP);
                        __m128 C1 = _mm_blend_ps(XPIYPI, BOUNDS, 10);
                        __m128 C2 = _mm_blend_ps(XPIYPI, BOUNDS, 5);
                        int inside = _mm_test_all_ones(_mm_castps_si128(_mm_cmpge_ps(C1, C2)));

                        if (!inside) {
                                continue;
                        }

                        __m128 XMYM = _mm_sub_ps(XPYP, XPIYPI);
                        __m128 ONEMXY = _mm_sub_ps(ONES, XMYM);
                        __m128 WEIGHTS_Y = _mm_movehl_ps(XMYM, ONEMXY); // 1-(y-yi) 1-(y-yi) (y-yi) (y-yi)
                        __m128 WEIGHTS_X = _mm_castpd_ps(_mm_movedup_pd(_mm_castps_pd(_mm_move_ss(ONEMXY, XMYM)))); // 1-(x-xi) (x-xi) 1-(x-xi) (x-xi)

                        int xpi = _mm_cvt_ss2si(XPIYPI);
                        int ypi = _mm_cvt_ss2si(_mm_shuffle_ps(XPIYPI, XPIYPI, 0x0002));
                        const uchar *p0 = in_buffer->data.uc + ypi*in_buffer->row_stride + xpi;
                        const uchar *p1 = p0 + in_buffer->row_stride;
                        pixels[0] = p0[0];
                        pixels[1] = p0[1];
                        pixels[2] = p1[0];
                        pixels[3] = p1[1];
                        __m128 PIXELS = _mm_cvtepi32_ps(_mm_load_si128((const __m128i*)pixels));

                        __m128 R = _mm_dp_ps(PIXELS, _mm_mul_ps(WEIGHTS_X, WEIGHTS_Y), 0xF1);
                        int I = _mm_cvt_ss2si(R);
                        if (I < 0)
                                I = 0;
                        else if (I > 255)
                                I = 255;

                        drow[x] = I;
                }
        }
#else
        const int LAST_X = in_buffer->width - 1;
        const int LAST_Y = in_buffer->height - 1;

#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 4)
#endif
        for (int y = 0; y < out_buffer->height; ++y) {
                uchar *drow = out_buffer->data.uc + y*out_buffer->row_stride;

                float xp = y*t[3] + t[6];
                float yp = y*t[4] + t[7];
                for (int x = 0; x < out_buffer->width; ++x, xp += t[0], yp += t[1]) {
                        int xpi = xp;
                        int ypi = yp;

                        if (xpi < 0 || xpi >= LAST_X
                            || ypi < 0 || ypi >= LAST_Y) {
                                continue;
                        }

                        const uchar *p0 = in_buffer->data.uc + ypi*in_buffer->row_stride + xpi;
                        const uchar *p1 = p0 + in_buffer->row_stride;

                        float u = xp-xpi;
                        float v = yp-ypi;
                        float up = 1.0f - u;
                        float vp = 1.0f - v;

                        int I = vp*(up*p0[0] + u*p0[1])
                                + v*(up*p1[0] + u*p1[1]);

                        if (I < 0)
                                I = 0;
                        else if (I > 255)
                                I = 255;

                        drow[x] = I;
                }
        }
#endif
}

void fill_warp_buffer_bg(const struct NXImage* image,
                         struct NXImage* warp_buffer,
                         float t0, float t1, float t2,
                         float t3, float t4, float t5,
                         enum NXAWPBgMode bg_mode, int bg_color)
{
        const int LAST_X = image->width - 1;
        const int LAST_Y = image->height - 1;

        for (int y = 0; y < warp_buffer->height; ++y) {
                uchar *drow = warp_buffer->data.uc + y*warp_buffer->row_stride;

                float xp = y*t2 + t4;
                float yp = y*t3 + t5;
                for (int x = 0; x < warp_buffer->width; ++x, xp += t0, yp += t1) {
                        NXBool bg = NX_FALSE;

                        int xpi = xp;
                        int ypi = yp;

                        float u = xp-xpi;
                        float v = yp-ypi;
                        float up = 1.0f - u;
                        float vp = 1.0f - v;

                        int idx[2] = { xpi, xpi + 1};
                        int idy[2] = { ypi, ypi + 1};

                        switch (bg_mode) {
                        default:
                        case NX_AWP_BG_FIXED:
                                if (idx[0] <= 0 || idx[1] >= LAST_X
                                    || idy[0] <= 0 || idy[1] >= LAST_Y) {
                                        drow[x] = bg_color;
                                        continue;
                                }
                                break;
                        case NX_AWP_BG_NOISE:
                                if (idx[0] <= 0 || idx[1] >= LAST_X
                                    || idy[0] <= 0 || idy[1] >= LAST_Y) {
                                        drow[x] = 255.0f * NX_UNIFORM_SAMPLE_S;
                                        continue;
                                }
                                break;
                        case NX_AWP_BG_REPEAT:
                                if (idx[0] <= 0) {
                                        idx[0] = 0;
                                        idx[1] = 0;
                                        bg = NX_TRUE;
                                } else if (idx[1] >= LAST_X) {
                                        idx[0] = LAST_X;
                                        idx[1] = LAST_X;
                                        bg = NX_TRUE;
                                }

                                if (idy[0] <= 0) {
                                        idy[0] = 0;
                                        idy[1] = 0;
                                        bg = NX_TRUE;
                                } else if (idy[1] >= LAST_Y) {
                                        idy[0] = LAST_Y;
                                        idy[1] = LAST_Y;
                                        bg = NX_TRUE;
                                }
                                break;
                        }

                        if (bg) {
                                const uchar *p0 = image->data.uc + idy[0]*image->row_stride;
                                const uchar *p1 = image->data.uc + idy[1]*image->row_stride;
                                int I = vp*(up*p0[idx[0]] + u*p0[idx[1]])
                                        + v*(up*p1[idx[0]] + u*p1[idx[1]]);

                                if (I < 0)
                                        I = 0;
                                else if (I > 255)
                                        I = 255;

                                drow[x] = I;
                        }
                }
        }
}

static const double BLUR_KERNEL_LOSS = 0.003;
static void warp_processor_blur(struct NXImage *image,
                                float sigma_x, float sigma_y)
{
        const int N_THREADS = omp_get_max_threads();
        float *buffers[N_THREADS];
        for (int i = 0; i < N_THREADS; ++i)
                buffers[i] = nx_image_filter_buffer_alloc(image->width, image->height,
                                                          sigma_x, sigma_y);

        int nkx = nx_kernel_size_min_gaussian(sigma_x, BLUR_KERNEL_LOSS);
        int nky = nx_kernel_size_min_gaussian(sigma_y, BLUR_KERNEL_LOSS);
        int nk_max = nx_max_i(nkx, nky);
        int nk_sym = nk_max / 2 + 1;
        float *kernel = NX_NEW_S(nk_sym);

        // Smooth in x-direction
        int nk = nkx / 2 + 1;
        nx_kernel_sym_gaussian(nk, kernel, sigma_x);
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 4)
#endif
        for (int y = 0; y < image->height; ++y) {
                float *buffer = buffers[omp_get_thread_num()];
                uchar *image_row = image->data.uc + y * image->row_stride;
                nx_filter_copy_to_buffer1_uc(image->width, buffer, image_row, nkx / 2, NX_BORDER_MIRROR);
                nx_convolve_sym(image->width, buffer, nk, kernel);
                for (int x = 0; x < image->width; ++x)
                        image_row[x] = (uchar)buffer[x];
        }

        // Smooth in y-direction
        nk = nky / 2 + 1;
        nx_kernel_sym_gaussian(nk, kernel, sigma_y);
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 4)
#endif
        for (int x = 0; x < image->width; ++x) {
                float *buffer = buffers[omp_get_thread_num()];
                uchar *image_col = image->data.uc + x;
                nx_filter_copy_to_buffer_uc(image->height, buffer, image_col, image->row_stride, nky / 2, NX_BORDER_MIRROR);
                nx_convolve_sym(image->height, buffer, nk, kernel);
                for (int y = 0; y < image->height; ++y)
                        image_col[y*image->row_stride] = (uchar)buffer[y];
        }

        nx_free(kernel);
        for (int i = 0; i < N_THREADS; ++i)
                nx_free(buffers[i]);
}
