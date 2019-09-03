/**
 * @file vg_harris_detector.cc
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
#include "virg/nexus/vg_harris_detector.hpp"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_harris_detector.h"

namespace virg {
namespace nexus {

VGHarrisDetector::VGHarrisDetector()
        : m_sigma_win(1.2f),
          m_k(0.06f),
          m_threshold(0.000005f)
{}

VGHarrisDetector::~VGHarrisDetector()
{
}

float VGHarrisDetector::adapt_threshold(float threshold, int n_keys,
                                        int max_n_keys)
{
        if (n_keys > 1.7f*max_n_keys)
                threshold *= 4.0f;
        else if (n_keys > 1.4f*max_n_keys)
                threshold *= 1.9f;
        else if (n_keys > 1.1f*max_n_keys)
                threshold *= 1.2f;
        else if (n_keys > max_n_keys)
                threshold *= 1.02f;
        else if (n_keys < 0.5f*max_n_keys)
                threshold *= 0.25f;
        else if (n_keys < 0.9f*max_n_keys)
                threshold *= 0.6f;
        else if (n_keys < 0.95f*max_n_keys)
                threshold *= 0.9f;
        else if (n_keys < 0.99f*max_n_keys)
                threshold *= 0.97f;

        return threshold;
}

void VGHarrisDetector::update_score_image(const VGImage& image)
{
        struct NXImage* dimg[3] = { m_dimg[0].nx_img(),
                                    m_dimg[1].nx_img(),
                                    m_dimg[2].nx_img() };
        nx_harris_deriv_images(dimg, image.nx_img(), m_sigma_win);
        nx_harris_score_image(m_simg.nx_img(), dimg, m_k);
}

int VGHarrisDetector::detect(const VGImage& image, std::vector<NXKeypoint> &keys, int max_n_keys, bool adapt_threshold)
{
        NX_ASSERT(max_n_keys > 0);

        update_score_image(image);

        keys.resize(2*max_n_keys);
        int n_keys = nx_harris_detect_keypoints(2*max_n_keys, &keys[0], m_simg.nx_img(), m_threshold);

        if (adapt_threshold) {
                // NX_LOG(VG_LOG_TAG, "adapting threshold with %d keys %d max %.10f threshold", n_keys, max_n_keys, m_threshold);
                m_threshold = this->adapt_threshold(m_threshold, n_keys, max_n_keys);
                // NX_LOG(VG_LOG_TAG, "   adapted to %.10f", m_threshold);
        }

        if (n_keys > max_n_keys)
                n_keys = max_n_keys;
        keys.resize(n_keys);
        return n_keys;
}

int VGHarrisDetector::detect_pyr(const VGImagePyr& pyr, std::vector<NXKeypoint> &keys,
                                 int n_key_levels, int max_n_keys, bool adapt_threshold)
{
        NX_ASSERT(n_key_levels > 0);
        NX_ASSERT(pyr.n_levels() >= n_key_levels);
        NX_ASSERT(max_n_keys > 0);

        int total_n_keys = 0;
        int buffer_sz = 2*max_n_keys;
        keys.resize(buffer_sz);
        while (n_key_levels > 0 && buffer_sz > 0) {
                const int level = n_key_levels-1;
                const int k = total_n_keys;

                update_score_image(pyr[level]);
                int n_keys = nx_harris_detect_keypoints(buffer_sz,
                                                        &keys[k],
                                                        m_simg.nx_img(),
                                                        m_threshold);

                for (int i = k; i < k+n_keys; ++i) {
                        keys[i].level = level;
                        keys[i].scale = pyr.level_scale(level);
                        keys[i].sigma = pyr.level_sigma(level);
                        keys[i].id = i;

                }

                buffer_sz -= n_keys;
                total_n_keys += n_keys;
                --n_key_levels;
        }

        if (adapt_threshold) {
                // NX_LOG(VG_LOG_TAG, "adapting threshold with %d keys %d max %.10f threshold", total_n_keys, max_n_keys, m_threshold);
                m_threshold = this->adapt_threshold(m_threshold, total_n_keys, max_n_keys);
                // NX_LOG(VG_LOG_TAG, "   adapted to %.10f", m_threshold);
        }

        if (total_n_keys > max_n_keys)
                total_n_keys = max_n_keys;
        keys.resize(total_n_keys);
        return total_n_keys;
}


}
}
