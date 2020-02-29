/**
 * @file vg_point_correspondence_2d.cc
 *
 * Copyright (C) 2019,2020 Mustafa Ozuysal. All rights reserved.
 *
 * This file is part of the VIRG-Nexus Library
 *
 * VIRG-Nexus Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VIRG-Nexus Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/vg_point_correspondence_2d.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>

using std::sqrt;
using std::sort;
using std::begin;
using std::end;
using std::memcpy;

namespace virg {
namespace nexus {

void VGPointCorrespondence2D::reset_stats()
{
        nx_point_match_2d_stats_reset(m_stats.get());
}

VGPointCorrespondence2D VGPointCorrespondence2D::clone() const
{
        VGPointCorrespondence2D corr;
        for (const struct NXPointMatch2D& pm: this->m_matches)
                corr.m_matches.push_back(pm);
        memcpy(corr.m_stats.get(), this->m_stats.get(), sizeof(*corr.m_stats));

        return corr;
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
