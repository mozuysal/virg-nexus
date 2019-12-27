/**
 * @file tests_gnuplot.cc
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
 * You should have received a copy of the GNU General Public License along with
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>

#include "gtest/gtest.h"

#include "virg/nexus/nx_gnuplot.h"

using namespace std;

namespace {

class NXGNUPlotTest : public ::testing::Test {
protected:
  NXGNUPlotTest() {}

  virtual void SetUp() { gnuplot = nullptr; }

  virtual void TearDown() { nx_gnuplot_free(gnuplot); }

  struct NXGNUPlot *gnuplot;
};

TEST_F(NXGNUPlotTest, PDFSineTest) {
        gnuplot = nx_gnuplot_new(nullptr, NX_FALSE);
        nx_gnuplot_set_terminal_pdf(gnuplot, 20.0, 20.0, "/tmp/sine.pdf");
        nx_gnuplot_send_command(gnuplot, "plot sin(x);");
        nx_gnuplot_flush(gnuplot);
}

} // namespace
