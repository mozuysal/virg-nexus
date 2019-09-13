/**
 * @file vg_fundamental_matrix.cc
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
#include "virg/nexus/vg_fundamental_matrix.hpp"

namespace virg {
namespace nexus {

VGFundamentalMatrix::VGFundamentalMatrix()
        : m_data { 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0 }
{}

}
}
