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

void VGPointCorrespondence2D::reset_stats()
{
        m_stats.m[0] = 0.0f;
        m_stats.m[1] = 0.0f;
        m_stats.mp[0] = 0.0f;
        m_stats.mp[1] = 0.0f;
        m_stats.d = 1.0f;
        m_stats.dp = 1.0f;
}

void VGPointCorrespondence2D::normalize()
{
        m_stats = nx_point_match_2d_normalize(this->size(),
                                              this->matches());
}

void VGPointCorrespondence2D::denormalize()
{
        nx_point_match_2d_denormalize(this->size(), this->matches(), m_stats);
        reset_stats();
}

void VGPointCorrespondence2D::sort_by_match_cost()
{
        sort(begin(m_matches), end(m_matches),
             [](const struct NXPointMatch2D& pm0, const struct NXPointMatch2D& pm1) {
                     return pm0.match_cost < pm1.match_cost;
             });
}

}
}
