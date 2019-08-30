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
#include "virg/nexus/nx_harris_detector.h"

namespace virg {
namespace nexus {

VGHarrisDetector::VGHarrisDetector()
        : m_sigma_win(1.2f),
          m_k(0.06f),
          m_threshold(0.000005f)
{
        m_keys.reserve(1000);
}

VGHarrisDetector::~VGHarrisDetector()
{
}

int VGHarrisDetector::detect(VGImage& image, int max_n_keys, bool adapt_threshold)
{
        struct NXImage* dimg[3] = { m_dimg[0].nx_img(),
                                    m_dimg[1].nx_img(),
                                    m_dimg[2].nx_img() };
        nx_harris_deriv_images(dimg, image.nx_img(), m_sigma_win);
        nx_harris_score_image(m_simg.nx_img(), dimg, m_k);

        m_keys.resize(2*max_n_keys);
        int n_keys = nx_harris_detect_keypoints(2*max_n_keys, &m_keys[0], m_simg.nx_img(), m_threshold);

        if (adapt_threshold) {
                // NX_LOG(VG_LOG_TAG, "adapting threshold with %d keys %d max %.10f threshold", n_keys, max_n_keys, m_threshold);
                if (n_keys > 1.7f*max_n_keys)
                        m_threshold *= 4.0f;
                else if (n_keys > 1.4f*max_n_keys)
                        m_threshold *= 1.9f;
                else if (n_keys > 1.1f*max_n_keys)
                        m_threshold *= 1.2f;
                else if (n_keys > max_n_keys)
                        m_threshold *= 1.02f;
                else if (n_keys < 0.5f*max_n_keys)
                        m_threshold *= 0.25f;
                else if (n_keys < 0.9f*max_n_keys)
                        m_threshold *= 0.6f;
                else if (n_keys < 0.95f*max_n_keys)
                        m_threshold *= 0.9f;
                else if (n_keys < 0.99f*max_n_keys)
                        m_threshold *= 0.97f;
                // NX_LOG(VG_LOG_TAG, "   adapted to %.10f", m_threshold);
        }

        if (n_keys > max_n_keys)
                n_keys = max_n_keys;
        m_keys.resize(n_keys);
        return n_keys;
}

}
}
