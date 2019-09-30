/**
 * @file vg_fundamental_matrix.hpp
 *
 * Copyright (C) 2019 Mustafa Ozuysal. All rights reserved.
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
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_VG_FUNDAMENTAL_MATRIX_HPP
#define VIRG_NEXUS_VG_FUNDAMENTAL_MATRIX_HPP

#include <cstring>

#include "virg/nexus/nx_epipolar.h"
#include "virg/nexus/vg_point_correspondence_2d.hpp"

namespace virg {
namespace nexus {

class VGFundamentalMatrix {
public:
        VGFundamentalMatrix();

        void set(const double* f_new) { std::memcpy(m_data, f_new, 9*sizeof(m_data[0])); }

        double*       data()       { return &m_data[0]; }
        const double* data() const { return &m_data[0]; }

        void epipolar_line_fwd(double* l, const double* x) {
                l[0] = m_data[0]*x[0] + m_data[3]*x[1] + m_data[6];
                l[1] = m_data[1]*x[0] + m_data[4]*x[1] + m_data[7];
                l[2] = m_data[2]*x[0] + m_data[5]*x[1] + m_data[8];
        }

        double epipolar_distance_fwd(const struct NXPointMatch2D* pm) {
                return nx_fundamental_epipolar_distance_fwd(&m_data[0], pm);
        }

        int estimate_ransac(VGPointCorrespondence2D& corr,
                            double inlier_tolerance,
                            int max_n_iter) {
                return nx_fundamental_estimate_ransac(&m_data[0], corr.size(),
                                                      corr.matches(),
                                                      inlier_tolerance,
                                                      max_n_iter);
        }

        int estimate_from_inliers(VGPointCorrespondence2D& corr,
                                  double inlier_tolerance) {
                nx_fundamental_estimate_inliers(&m_data[0], corr.size(),
                                                corr.matches());
                return nx_fundamental_mark_inliers(&m_data[0], corr.size(),
                                                   corr.matches(),
                                                   inlier_tolerance);
        }
private:
        double m_data[9];
};

}
}

#endif
