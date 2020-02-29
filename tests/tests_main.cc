/**
 * @file tests_main.cc
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
#include "gtest/gtest.h"

#include "virg/nexus/nx_options.h"
#include "virg/nexus/nx_log.h"

bool IS_VALGRIND_RUN = false;

int main(int argc, char** argv)
{
        ::testing::InitGoogleTest(&argc, argv);

        struct NXOptions *opt = nx_options_new("b", "--valgrind", "Expect to be run with valgrind", NX_FALSE);
        nx_options_set_from_args(opt, argc, argv);

        NXBool is_valgrind_run = nx_options_get_bool(opt, "--valgrind");

        if (is_valgrind_run) {
                NX_LOG(NX_LOG_TAG, "VALGRIND RUN DETECTED");
                IS_VALGRIND_RUN = true;
        }

        nx_options_free(opt);

        return RUN_ALL_TESTS();
}
