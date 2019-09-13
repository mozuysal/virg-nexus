/**
 * @file vg_fundamental_matrix.hpp
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
