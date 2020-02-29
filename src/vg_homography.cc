/**
 * @file vg_homography.cc
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
#include "virg/nexus/vg_homography.hpp"

namespace virg {
namespace nexus {

VGHomography::VGHomography()
        : m_data { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 }
{}

template <> void VGHomography::transfer_fwd<double>(double* xp, const double* x) const
{
        double s = m_data[2]*x[0]+m_data[5]*x[1]+m_data[8];
        double s_inv = 1.0 / s;

        xp[0] = (m_data[0]*x[0]+m_data[3]*x[1]+m_data[6]) * s_inv;
        xp[1] = (m_data[1]*x[0]+m_data[4]*x[1]+m_data[7]) * s_inv;

}

template <> void VGHomography::transfer_fwd<float>(float* xp, const float* x) const
{
        double s = m_data[2]*x[0]+m_data[5]*x[1]+m_data[8];
        double s_inv = 1.0 / s;

        xp[0] = (m_data[0]*x[0]+m_data[3]*x[1]+m_data[6]) * s_inv;
        xp[1] = (m_data[1]*x[0]+m_data[4]*x[1]+m_data[7]) * s_inv;
}


}
}
