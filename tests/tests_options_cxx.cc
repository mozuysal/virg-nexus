/**
 * @file tests_options_cxx.cc
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

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_string_array.h"

#include "virg/nexus/vg_options.hpp"

using namespace std;
using virg::nexus::VGOptions;

namespace {

class VGOptionsTest : public ::testing::Test {
protected:
        VGOptionsTest() {
        }

        virtual void SetUp() {
                args = nx_string_array_new(1);
                nx_string_array_set(args, 0, "prog");
        }

        virtual void TearDown() {
                nx_string_array_free(args);
        }

        VGOptions opt;
        struct NXStringArray *args;
};

TEST_F(VGOptionsTest, add_int) {
        opt.add_int("--int", "int help", VGOptions::NOT_REQUIRED, 123);
        EXPECT_EQ(123, opt.get_int("--int"));
        EXPECT_FALSE(opt.is_set("--int"));
}

TEST_F(VGOptionsTest, add_double) {
        opt.add_double("--double", "double help", VGOptions::NOT_REQUIRED, 123.0);
        EXPECT_EQ(123.0, opt.get_double("--double"));
        EXPECT_FALSE(opt.is_set("--double"));
}

TEST_F(VGOptionsTest, add_string) {
        opt.add_string("-s|--string", "string help", VGOptions::NOT_REQUIRED, "123");
        EXPECT_EQ("123", opt.get_string("-s"));
        EXPECT_EQ("123", opt.get_string("--string"));
        EXPECT_FALSE(opt.is_set("-s"));
        EXPECT_FALSE(opt.is_set("--string"));
}

TEST_F(VGOptionsTest, add_bool) {
        opt.add_bool("-b", "bool help", VGOptions::NOT_REQUIRED, true);
        EXPECT_TRUE(opt.get_bool("-b"));
        EXPECT_FALSE(opt.is_set("-b"));
}

TEST_F(VGOptionsTest, add_rest) {
        opt.add_rest("rest help", VGOptions::NOT_REQUIRED);
        EXPECT_EQ(0U, opt.get_rest().size());
}


TEST_F(VGOptionsTest, parse_int) {
        opt.add_int("--int", "int help", VGOptions::NOT_REQUIRED, 123);
        nx_string_array_append(args, "--int");
        nx_string_array_append(args, "321");
        opt.set_from_args(nx_string_array_size(args), nx_string_array_ptr(args));
        EXPECT_EQ(321, opt.get_int("--int"));
        EXPECT_TRUE(opt.is_set("--int"));
}

TEST_F(VGOptionsTest, parse_double) {
        opt.add_double("--double", "double help", VGOptions::NOT_REQUIRED, 123.0);
        nx_string_array_append(args, "--double");
        nx_string_array_append(args, "321.0");
        opt.set_from_args(nx_string_array_size(args), nx_string_array_ptr(args));
        EXPECT_EQ(321.0, opt.get_double("--double"));
        EXPECT_TRUE(opt.is_set("--double"));
}

TEST_F(VGOptionsTest, parse_string) {
        opt.add_string("--string", "string help", VGOptions::NOT_REQUIRED, "123");
        nx_string_array_append(args, "--string");
        nx_string_array_append(args, "321");
        opt.set_from_args(nx_string_array_size(args), nx_string_array_ptr(args));
        EXPECT_EQ("321", opt.get_string("--string"));
        EXPECT_TRUE(opt.is_set("--string"));
}

TEST_F(VGOptionsTest, parse_bool) {
        opt.add_bool("--bool", "bool help", VGOptions::NOT_REQUIRED, false);
        nx_string_array_append(args, "--bool");
        opt.set_from_args(nx_string_array_size(args), nx_string_array_ptr(args));
        EXPECT_TRUE(opt.get_bool("--bool"));
        EXPECT_TRUE(opt.is_set("--bool"));
}

TEST_F(VGOptionsTest, parse_rest) {
        opt.add_rest("rest help", VGOptions::NOT_REQUIRED);
        nx_string_array_append(args, "rest1");
        nx_string_array_append(args, "rest2");
        nx_string_array_append(args, "rest3");
        opt.set_from_args(nx_string_array_size(args), nx_string_array_ptr(args));
        vector<string> rest = opt.get_rest();
        EXPECT_EQ(3U, rest.size());
        EXPECT_EQ("rest1", rest[0]);
        EXPECT_EQ("rest2", rest[1]);
        EXPECT_EQ("rest3", rest[2]);
}


TEST_F(VGOptionsTest, parse_many) {
        opt.add_string("-o", "Output name", VGOptions::REQUIRED, "out.txt");
        opt.add_string("--list", "List name", VGOptions::REQUIRED, "list.txt");
        opt.add_double("-p|--precision", "Precision of the method", VGOptions::REQUIRED, 2.0);
        opt.add_int("-s", "Size of the buffers", VGOptions::REQUIRED, 128);
        opt.add_bool("-v|--verbose", "Toggle verbose output", VGOptions::NOT_REQUIRED);
        opt.add_rest("images", VGOptions::REQUIRED);

        nx_string_array_append(args, "-s");
        nx_string_array_append(args, "123");
        nx_string_array_append(args, "-o");
        nx_string_array_append(args, "output.txt");
        nx_string_array_append(args, "-p");
        nx_string_array_append(args, "222.0");
        nx_string_array_append(args, "-v");
        nx_string_array_append(args, "a");
        nx_string_array_append(args, "b");
        nx_string_array_append(args, "--list");
        nx_string_array_append(args, "listfile.dat");
        nx_string_array_append(args, "c");

        opt.set_from_args(nx_string_array_size(args), nx_string_array_ptr(args));

        EXPECT_EQ("output.txt", opt.get_string("-o"));
        EXPECT_TRUE(opt.is_set("-o"));

        EXPECT_EQ("listfile.dat", opt.get_string("--list"));
        EXPECT_TRUE(opt.is_set("--list"));

        EXPECT_EQ(222.0, opt.get_double("--precision"));
        EXPECT_TRUE(opt.is_set("-p"));

        EXPECT_EQ(123, opt.get_int("-s"));
        EXPECT_TRUE(opt.is_set("-s"));

        EXPECT_TRUE(opt.get_bool("--verbose"));
        EXPECT_TRUE(opt.is_set("-v"));

        vector<string> rest = opt.get_rest();
        EXPECT_EQ(3U, rest.size());
        EXPECT_EQ("a", rest[0]);
        EXPECT_EQ("b", rest[1]);
        EXPECT_EQ("c", rest[2]);
}


TEST_F(VGOptionsTest, parse_twice) {
        opt.add_string("-o", "Output name", VGOptions::REQUIRED, "out.txt");
        opt.add_string("--list", "List name", VGOptions::REQUIRED, "list.txt");
        opt.add_double("-p|--precision", "Precision of the method", VGOptions::REQUIRED, 2.0);
        opt.add_int("-s", "Size of the buffers", VGOptions::REQUIRED, 128);
        opt.add_bool("-v|--verbose", "Toggle verbose output", VGOptions::NOT_REQUIRED);
        opt.add_rest("images", VGOptions::REQUIRED);

        nx_string_array_append(args, "-s");
        nx_string_array_append(args, "123");
        nx_string_array_append(args, "-o");
        nx_string_array_append(args, "output.txt");
        nx_string_array_append(args, "-p");
        nx_string_array_append(args, "222.0");
        nx_string_array_append(args, "-v");
        nx_string_array_append(args, "a");
        nx_string_array_append(args, "b");
        nx_string_array_append(args, "--list");
        nx_string_array_append(args, "listfile.dat");
        nx_string_array_append(args, "c");

        opt.set_from_args(nx_string_array_size(args), nx_string_array_ptr(args));
        opt.set_from_args(nx_string_array_size(args), nx_string_array_ptr(args));

        EXPECT_EQ("output.txt", opt.get_string("-o"));
        EXPECT_TRUE(opt.is_set("-o"));

        EXPECT_EQ("listfile.dat", opt.get_string("--list"));
        EXPECT_TRUE(opt.is_set("--list"));

        EXPECT_EQ(222.0, opt.get_double("--precision"));
        EXPECT_TRUE(opt.is_set("-p"));

        EXPECT_EQ(123, opt.get_int("-s"));
        EXPECT_TRUE(opt.is_set("-s"));

        EXPECT_TRUE(opt.get_bool("--verbose"));
        EXPECT_TRUE(opt.is_set("-v"));

        vector<string> rest = opt.get_rest();
        EXPECT_EQ(3U, rest.size());
        EXPECT_EQ("a", rest[0]);
        EXPECT_EQ("b", rest[1]);
        EXPECT_EQ("c", rest[2]);
}

TEST_F(VGOptionsTest, parse_smaller) {
        opt.add_string("-o", "Output name", VGOptions::NOT_REQUIRED, "out.txt");
        opt.add_string("--list", "List name", VGOptions::NOT_REQUIRED, "list.txt");
        opt.add_double("-p|--precision", "Precision of the method", VGOptions::NOT_REQUIRED, 2.0);
        opt.add_int("-s", "Size of the buffers", VGOptions::NOT_REQUIRED, 128);
        opt.add_bool("-v|--verbose", "Toggle verbose output", VGOptions::NOT_REQUIRED);
        opt.add_rest("images", VGOptions::NOT_REQUIRED);

        nx_string_array_append(args, "-s");
        nx_string_array_append(args, "123");
        nx_string_array_append(args, "-o");
        nx_string_array_append(args, "output.txt");
        nx_string_array_append(args, "-p");
        nx_string_array_append(args, "222.0");
        nx_string_array_append(args, "-v");
        nx_string_array_append(args, "a");
        nx_string_array_append(args, "b");
        nx_string_array_append(args, "--list");
        nx_string_array_append(args, "listfile.dat");
        nx_string_array_append(args, "c");

        opt.set_from_args(nx_string_array_size(args), nx_string_array_ptr(args));

        nx_string_array_resize(args, 1);
        opt.set_from_args(nx_string_array_size(args), nx_string_array_ptr(args));

        EXPECT_EQ("out.txt", opt.get_string("-o"));
        EXPECT_FALSE(opt.is_set("-o"));

        EXPECT_EQ("list.txt", opt.get_string("--list"));
        EXPECT_FALSE(opt.is_set("--list"));

        EXPECT_EQ(2.0, opt.get_double("--precision"));
        EXPECT_FALSE(opt.is_set("-p"));

        EXPECT_EQ(128, opt.get_int("-s"));
        EXPECT_FALSE(opt.is_set("-s"));

        EXPECT_FALSE(opt.get_bool("--verbose"));
        EXPECT_FALSE(opt.is_set("-v"));

        EXPECT_EQ(0U, opt.get_rest().size());
}

} // namespace
