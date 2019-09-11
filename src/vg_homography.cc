/**
 * @file vg_homography.cc
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
