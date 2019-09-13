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
#include <memory>

#include "virg/nexus/nx_point_match_2d.h"
#include "virg/nexus/nx_point_match_2d_stats.h"


namespace virg {
namespace nexus {

class VGPointCorrespondence2D {
public:
        VGPointCorrespondence2D() : m_stats(nx_point_match_2d_stats_new(),
                                            nx_point_match_2d_stats_free) { }

        VGPointCorrespondence2D clone() const;
        void clear() { m_matches.clear(); reset_stats(); }
        void reserve(int n_matches) { m_matches.reserve(n_matches); }

        int add_match(uint64_t id,  float x, float y, float sigma,
                      uint64_t idp, float xp, float yp, float sigma_p,
                      float match_cost, bool is_inlier = false);
        int add_keypoint_match(const struct NXKeypoint *k,
                               const struct NXKeypoint *kp,
                               float sigma0, float match_cost,
                               bool is_inlier = false);
        int size() const { return static_cast<int>(m_matches.size()); }
        struct NXPointMatch2D&       operator[](int idx)       { return m_matches[idx]; }
        const struct NXPointMatch2D& operator[](int idx) const { return m_matches[idx]; }
        struct NXPointMatch2D*       matches()       { return m_matches.data(); }
        const struct NXPointMatch2D* matches() const { return m_matches.data(); }
        bool is_inlier (int idx) const                  { return static_cast<bool>(m_matches[idx].is_inlier); }
        void set_inlier(int idx, bool is_inlier = true) { m_matches[idx].is_inlier = static_cast<NXBool>(is_inlier); }

        void sort_by_match_cost();

        const NXPointMatch2DStats* stats() const { return m_stats.get(); }
        void normalize();
        void denormalize();
        void denormalize_homography(double* h) {
                nx_point_match_2d_stats_denormalize_homography(m_stats.get(), h);
        }

private:
        void reset_stats();

        std::shared_ptr<struct NXPointMatch2DStats> m_stats;
        std::vector<struct NXPointMatch2D> m_matches;
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
                        .idp = idp,
                        .is_inlier = static_cast<NXBool>(is_inlier) };
        m_matches.push_back(pm);
        return static_cast<int>(m_matches.size());
}

inline int VGPointCorrespondence2D::add_keypoint_match(const struct NXKeypoint *k,
                                                       const struct NXKeypoint *kp,
                                                       float sigma0,
                                                       float match_cost,
                                                       bool is_inlier)
{
        m_matches.push_back(nx_point_match_2d_from_keypoints(k, kp,
                                                             sigma0,
                                                             match_cost,
                                                             static_cast<NXBool>(is_inlier)));
        return static_cast<int>(m_matches.size());
}

}
}

#endif
