/**
 * @file tests_gnuplot.cc
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
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>

#include "gtest/gtest.h"
#include <memory>

#include "virg/nexus/nx_gnuplot.h"

using namespace std;

namespace {

class NXGNUPlotTest : public ::testing::Test {
protected:
        NXGNUPlotTest() {}

        virtual void SetUp() { gnuplot = nullptr; }

        virtual void TearDown() { nx_gnuplot_free(gnuplot); }

        struct NXDataFrame *make_line_data(int n, double a, double b) {
                struct NXDataFrame *df = nx_data_frame_alloc();
                const int n_cols = 2;
                enum NXDataColumnType types[n_cols] = { NX_DCT_DOUBLE,
                                                        NX_DCT_DOUBLE };
                const char* labels[n_cols] = {"x", "y"};
                nx_data_frame_add_columns(df, n_cols, &types[0], &labels[0]);

                for (int i = 0; i < n; ++i) {
                        double x = i - n/2;
                        double y = a * x + b;
                        nx_data_frame_append_row(df, x, y);
                }

                return df;
        }

        struct NXGNUPlot *gnuplot;
};

TEST_F(NXGNUPlotTest, PDFSineTest) {
        gnuplot = nx_gnuplot_new(nullptr, NX_FALSE);
        nx_gnuplot_set_terminal_pdf(gnuplot, 20.0, 20.0, "/tmp/sine.pdf");
        nx_gnuplot_send_command(gnuplot, "plot sin(x);");
        nx_gnuplot_flush(gnuplot);
}

TEST_F(NXGNUPlotTest, PDFLine) {
        gnuplot = nx_gnuplot_new(nullptr, NX_FALSE);
        nx_gnuplot_set_terminal_pdf(gnuplot, 20.0, 20.0, "/tmp/line.pdf");
        nx_gnuplot_send_command(gnuplot, "plot '-' using 1:2;\n");
        struct NXDataFrame *df = make_line_data(20, 2.0, 1.0);
        nx_gnuplot_send_data_frame(gnuplot, df, NULL);
        nx_gnuplot_flush(gnuplot);
        nx_data_frame_free(df);
}

TEST_F(NXGNUPlotTest, PDFLineLabeled) {
        gnuplot = nx_gnuplot_new(nullptr, NX_FALSE);
        nx_gnuplot_set_terminal_pdf(gnuplot, 20.0, 20.0, "/tmp/line_labeled.pdf");
        struct NXDataFrame *df = make_line_data(20, 2.0, 1.0);
        nx_gnuplot_send_data_frame(gnuplot, df, "Line");
        nx_gnuplot_send_command(gnuplot, "plot $Line using 1:2 with linespoints, "
                                "$Line using 1:2 with circles;\n");
        nx_gnuplot_flush(gnuplot);
        nx_data_frame_free(df);
}

} // namespace
