/**
 * @file vg_point_correspondence_2d.hpp
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
#ifndef VIRG_NEXUS_VG_POINT_CORRESPONDENCE_2D_HPP
#define VIRG_NEXUS_VG_POINT_CORRESPONDENCE_2D_HPP

#include <vector>

#include "virg/nexus/nx_point_match_2d.h"

namespace virg {
namespace nexus {

class VGPointCorrespondence2D {
public:
        VGPointCorrespondence2D() { m_stats[0].reset(); m_stats[1].reset(); }

        void clear() { m_matches.clear(); m_stats[0].reset(); m_stats[1].reset(); }
        void reserve(int n_matches) { m_matches.reserve(n_matches); }

        int add_match(uint64_t id,  float x, float y, float sigma,
                      uint64_t idp, float xp, float yp, float sigma_p,
                      float match_cost, bool is_inlier = false);
        int add_keypoint_match(const struct NXKeypoint *k,
                               const struct NXKeypoint *kp,
                               float sigma0, float match_cost,
                               bool is_inlier = false);
        int size() const { return static_cast<int>(m_matches.size()); }
        struct NXPointMatch2D&       operator[](int idx)       { return m_matches[idx].pm; }
        const struct NXPointMatch2D& operator[](int idx) const { return m_matches[idx].pm; }
        bool is_inlier (int idx) const                  { return m_matches[idx].is_inlier; }
        void set_inlier(int idx, bool is_inlier = true) { m_matches[idx].is_inlier = is_inlier; }

        void normalize();
        void denormalize();
        void sort_by_match_cost();
private:
        struct Match {
                struct NXPointMatch2D pm;
                bool is_inlier;
        };

        struct Stats {
                float x_mean[2];
                float mean_distance;

                void reset() {
                        x_mean[0] = 0.0f;
                        x_mean[1] = 0.0f;
                        mean_distance = 1.0f;
                }
        } m_stats[2];

        std::vector<Match> m_matches;
};

inline int VGPointCorrespondence2D::add_match(uint64_t id,  float x, float y,
                                              float sigma,
                                              uint64_t idp, float xp, float yp,
                                              float sigma_p,
                                              float match_cost,
                                              bool is_inlier)
{
        struct NXPointMatch2D pm { .x = { x, y },
                        .xp = {xp, yp },
                        .match_cost = match_cost,
                        .sigma_x = sigma,
                        .sigma_xp = sigma_p,
                        .id = id,
                        .idp = idp };
        m_matches.push_back(Match { pm, is_inlier });
        return static_cast<int>(m_matches.size());
}

inline int VGPointCorrespondence2D::add_keypoint_match(const struct NXKeypoint *k,
                                                       const struct NXKeypoint *kp,
                                                       float sigma0,
                                                       float match_cost,
                                                       bool is_inlier)
{
        m_matches.push_back(Match {nx_point_match_2d_from_keypoints(k, kp,
                                                                    sigma0,
                                                                    match_cost),
                                is_inlier });
        return static_cast<int>(m_matches.size());
}

}
}

#endif
