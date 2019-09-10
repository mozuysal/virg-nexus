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
        nx_point_match_2d_stats_reset(m_stats.get());
}

void VGPointCorrespondence2D::normalize()
{
        nx_point_match_2d_stats_normalize_matches(m_stats.get(),
                                                  this->size(),
                                                  this->matches());
}

void VGPointCorrespondence2D::denormalize()
{
        nx_point_match_2d_stats_denormalize_matches(m_stats.get(),
                                                    this->size(),
                                                    this->matches());
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
