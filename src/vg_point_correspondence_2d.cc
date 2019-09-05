/**
 * @file vg_point_correspondence_2d.cc
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
#include "virg/nexus/vg_point_correspondence_2d.hpp"

#include <algorithm>
#include <cmath>

using std::sqrt;
using std::sort;
using std::begin;
using std::end;

namespace virg {
namespace nexus {

void VGPointCorrespondence2D::normalize()
{
        m_stats[0].x_mean[0] = 0.0f;
        m_stats[0].x_mean[1] = 0.0f;
        m_stats[1].x_mean[0] = 0.0f;
        m_stats[1].x_mean[1] = 0.0f;

        for (const auto& m: m_matches) {
                const auto& pm = m.pm;
                m_stats[0].x_mean[0] += pm.x[0];
                m_stats[0].x_mean[1] += pm.x[1];
                m_stats[1].x_mean[0] += pm.xp[0];
                m_stats[1].x_mean[1] += pm.xp[1];
        }
        m_stats[0].x_mean[0] /= m_matches.size();
        m_stats[0].x_mean[1] /= m_matches.size();
        m_stats[1].x_mean[0] /= m_matches.size();
        m_stats[1].x_mean[1] /= m_matches.size();

        m_stats[0].mean_distance = 0.0f;
        m_stats[1].mean_distance = 0.0f;
        for (auto& m: m_matches) {
                auto& pm = m.pm;
                pm.x[0] -= m_stats[0].x_mean[0];
                pm.x[1] -= m_stats[0].x_mean[1];
                m_stats[0].mean_distance += sqrt(pm.x[0]*pm.x[0]
                                                 + pm.x[1]*pm.x[1]);

                pm.xp[0] -= m_stats[1].x_mean[0];
                pm.xp[1] -= m_stats[1].x_mean[1];
                m_stats[1].mean_distance += sqrt(pm.xp[0]*pm.xp[0]
                                                 + pm.xp[1]*pm.xp[1]);
        }
        m_stats[0].mean_distance /= m_matches.size();
        m_stats[1].mean_distance /= m_matches.size();

        for (auto& m: m_matches) {
                auto& pm = m.pm;
                pm.x[0]     /= m_stats[0].mean_distance;
                pm.x[1]     /= m_stats[0].mean_distance;
                pm.sigma_x  /= m_stats[0].mean_distance;
                pm.xp[0]    /= m_stats[1].mean_distance;
                pm.xp[1]    /= m_stats[1].mean_distance;
                pm.sigma_xp /= m_stats[1].mean_distance;
        }
}

void VGPointCorrespondence2D::denormalize()
{
        for (auto& m: m_matches) {
                auto& pm = m.pm;
                pm.x[0]     = pm.x[0]*m_stats[0].mean_distance + m_stats[0].x_mean[0];
                pm.x[1]     = pm.x[1]*m_stats[0].mean_distance + m_stats[0].x_mean[1];
                pm.sigma_x  *= m_stats[0].mean_distance;
                pm.xp[0]    = pm.xp[0]*m_stats[1].mean_distance + m_stats[1].x_mean[0];
                pm.xp[1]    = pm.xp[1]*m_stats[1].mean_distance + m_stats[1].x_mean[1];
                pm.sigma_xp *= m_stats[1].mean_distance;
        }

        m_stats[0].reset();
        m_stats[1].reset();
}

void VGPointCorrespondence2D::sort_by_match_cost()
{
        sort(begin(m_matches), end(m_matches),
             [](const Match& m0, const Match& m1) {
                     return m0.pm.match_cost < m1.pm.match_cost;
             });
}

}
}
