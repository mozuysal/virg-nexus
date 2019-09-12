/**
 * @file vg_homography.hpp
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
#ifndef VIRG_NEXUS_VG_HOMOGRAPHY_HPP
#define VIRG_NEXUS_VG_HOMOGRAPHY_HPP

#include <cstring>

#include "virg/nexus/nx_homography.h"
#include "virg/nexus/vg_point_correspondence_2d.hpp"

namespace virg {
namespace nexus {

class VGHomography {
public:
        VGHomography();

        void set(double* h_new) { std::memcpy(m_data, h_new, 9*sizeof(m_data[0])); }

        double*       data()       { return &m_data[0]; }
        const double* data() const { return &m_data[0]; }

        template <typename T> void transfer_fwd(T *xp, const T *x) const;

        int estimate_ransac(VGPointCorrespondence2D& corr,
                            double inlier_tolerance,
                            int max_n_iter) {
                return nx_homography_estimate_ransac(&m_data[0], corr.size(),
                                                     corr.matches(),
                                                     inlier_tolerance,
                                                     max_n_iter);
        }

        int estimate_from_inliers(VGPointCorrespondence2D& corr, double inlier_tolerance) {
                nx_homography_estimate_dlt_inliers(&m_data[0], corr.size(),
                                                   corr.matches());
                return nx_homography_mark_inliers(&m_data[0], corr.size(),
                                                  corr.matches(),
                                                  inlier_tolerance);
        }

        double transfer_error_fwd(VGPointCorrespondence2D& corr) const { return nx_homography_transfer_error_fwd(this->data(), corr.size(), corr.matches()); }
        double transfer_error_bwd(VGPointCorrespondence2D& corr) const { return nx_homography_transfer_error_bwd(this->data(), corr.size(), corr.matches()); }
        double transfer_error_sym(VGPointCorrespondence2D& corr) const { return nx_homography_transfer_error_sym(this->data(), corr.size(), corr.matches()); }
private:
        double m_data[9];
};

}
}

#endif
