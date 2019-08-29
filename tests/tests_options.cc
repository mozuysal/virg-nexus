/**
 * @file tests_options.cc
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2015 Mustafa Ozuysal. All rights reserved.
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
#include "virg/nexus/nx_options.h"

#include "virg/nexus/vg_options.hpp"

using namespace std;
using virg::nexus::VGOptions;

namespace {

class NXOptionsTest : public ::testing::Test {
protected:
        NXOptionsTest() {
        }

        virtual void SetUp() {
                opt = nx_options_alloc();
                args = nx_string_array_new(1);
                nx_string_array_set(args, 0, "prog");
        }

        virtual void TearDown() {
                nx_options_free(opt);
                nx_string_array_free(args);
        }

        struct NXOptions *opt;
        struct NXStringArray *args;
};

TEST_F(NXOptionsTest, alloc) {
        EXPECT_TRUE(opt != NULL);
}

TEST_F(NXOptionsTest, add_int) {
        nx_options_add(opt, "i", "--int", "int help", 123);
        EXPECT_EQ(123, nx_options_get_int(opt, "--int"));
        EXPECT_FALSE(nx_options_is_set(opt, "--int"));
}

TEST_F(NXOptionsTest, add_double) {
        nx_options_add(opt, "d", "--double", "double help", 123.0);
        EXPECT_EQ(123.0, nx_options_get_double(opt, "--double"));
        EXPECT_FALSE(nx_options_is_set(opt, "--double"));
}

TEST_F(NXOptionsTest, add_string) {
        nx_options_add(opt, "s", "-s|--string", "string help", "123");
        EXPECT_STREQ("123", nx_options_get_string(opt, "-s"));
        EXPECT_FALSE(nx_options_is_set(opt, "-s"));
}

TEST_F(NXOptionsTest, add_bool) {
        nx_options_add(opt, "b", "-b", "bool help", NX_TRUE);
        EXPECT_EQ(NX_TRUE, nx_options_get_bool(opt, "-b"));
        EXPECT_FALSE(nx_options_is_set(opt, "-b"));
}

TEST_F(NXOptionsTest, add_rest) {
        nx_options_add(opt, "r", "rest help");
        EXPECT_TRUE(nx_options_get_rest(opt) == NULL);
}

TEST_F(NXOptionsTest, parse_int) {
        nx_options_add(opt, "i", "--int", "int help", 123);
        nx_string_array_append(args, "--int");
        nx_string_array_append(args, "321");
        nx_options_set_from_args(opt, nx_string_array_size(args), nx_string_array_ptr(args));
        EXPECT_EQ(321, nx_options_get_int(opt, "--int"));
        EXPECT_TRUE(nx_options_is_set(opt, "--int"));
}

TEST_F(NXOptionsTest, parse_double) {
        nx_options_add(opt, "d", "--double", "double help", 123.0);
        nx_string_array_append(args, "--double");
        nx_string_array_append(args, "321.0");
        nx_options_set_from_args(opt, nx_string_array_size(args), nx_string_array_ptr(args));
        EXPECT_EQ(321.0, nx_options_get_double(opt, "--double"));
        EXPECT_TRUE(nx_options_is_set(opt, "--double"));
}

TEST_F(NXOptionsTest, parse_string) {
        nx_options_add(opt, "s", "--string", "string help", "123");
        nx_string_array_append(args, "--string");
        nx_string_array_append(args, "321");
        nx_options_set_from_args(opt, nx_string_array_size(args), nx_string_array_ptr(args));
        EXPECT_STREQ("321", nx_options_get_string(opt, "--string"));
        EXPECT_TRUE(nx_options_is_set(opt, "--string"));
}

TEST_F(NXOptionsTest, parse_bool) {
        nx_options_add(opt, "b", "--bool", "bool help", NX_FALSE);
        nx_string_array_append(args, "--bool");
        nx_options_set_from_args(opt, nx_string_array_size(args), nx_string_array_ptr(args));
        EXPECT_TRUE(nx_options_get_bool(opt, "--bool"));
        EXPECT_TRUE(nx_options_is_set(opt, "--bool"));
}

TEST_F(NXOptionsTest, parse_rest) {
        nx_options_add(opt, "r", "rest help");
        nx_string_array_append(args, "rest1");
        nx_string_array_append(args, "rest2");
        nx_string_array_append(args, "rest3");
        nx_options_set_from_args(opt, nx_string_array_size(args), nx_string_array_ptr(args));
        char **rest = nx_options_get_rest(opt);
        EXPECT_TRUE(rest != NULL);
        EXPECT_STREQ("rest1", rest[0]);
        EXPECT_STREQ("rest2", rest[1]);
        EXPECT_STREQ("rest3", rest[2]);
        EXPECT_TRUE(rest[3] == NULL);
}

TEST_F(NXOptionsTest, parse_many) {
        nx_options_add(opt, "SSDIbR",
                       "-o", "Output name", "out.txt",
                       "--list", "List name", "list.txt",
                       "-p|--precision", "Precision of the method", 2.0,
                       "-s", "Size of the buffers", 128,
                       "-v|--verbose", "Toggle verbose output", NX_FALSE,
                       "images");
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

        nx_options_set_from_args(opt, nx_string_array_size(args), nx_string_array_ptr(args));

        EXPECT_STREQ("output.txt", nx_options_get_string(opt, "-o"));
        EXPECT_TRUE(nx_options_is_set(opt, "-o"));

        EXPECT_STREQ("listfile.dat", nx_options_get_string(opt, "--list"));
        EXPECT_TRUE(nx_options_is_set(opt, "--list"));

        EXPECT_EQ(222.0, nx_options_get_double(opt, "--precision"));
        EXPECT_TRUE(nx_options_is_set(opt, "-p"));

        EXPECT_EQ(123, nx_options_get_int(opt, "-s"));
        EXPECT_TRUE(nx_options_is_set(opt, "-s"));

        EXPECT_TRUE(nx_options_get_bool(opt, "--verbose"));
        EXPECT_TRUE(nx_options_is_set(opt, "-v"));

        char **rest = nx_options_get_rest(opt);
        EXPECT_TRUE(rest != NULL);
        EXPECT_STREQ("a", rest[0]);
        EXPECT_STREQ("b", rest[1]);
        EXPECT_STREQ("c", rest[2]);
        EXPECT_TRUE(rest[3] == NULL);
}

TEST_F(NXOptionsTest, parse_twice) {
        nx_options_add(opt, "SSDIbR",
                       "-o", "Output name", "out.txt",
                       "--list", "List name", "list.txt",
                       "-p|--precision", "Precision of the method", 2.0,
                       "-s", "Size of the buffers", 128,
                       "-v|--verbose", "Toggle verbose output", NX_FALSE,
                       "images");
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

        nx_options_set_from_args(opt, nx_string_array_size(args), nx_string_array_ptr(args));
        nx_options_set_from_args(opt, nx_string_array_size(args), nx_string_array_ptr(args));

        EXPECT_STREQ("output.txt", nx_options_get_string(opt, "-o"));
        EXPECT_TRUE(nx_options_is_set(opt, "-o"));

        EXPECT_STREQ("listfile.dat", nx_options_get_string(opt, "--list"));
        EXPECT_TRUE(nx_options_is_set(opt, "--list"));

        EXPECT_EQ(222.0, nx_options_get_double(opt, "--precision"));
        EXPECT_TRUE(nx_options_is_set(opt, "-p"));

        EXPECT_EQ(123, nx_options_get_int(opt, "-s"));
        EXPECT_TRUE(nx_options_is_set(opt, "-s"));

        EXPECT_TRUE(nx_options_get_bool(opt, "--verbose"));
        EXPECT_TRUE(nx_options_is_set(opt, "-v"));

        char **rest = nx_options_get_rest(opt);
        EXPECT_TRUE(rest != NULL);
        EXPECT_STREQ("a", rest[0]);
        EXPECT_STREQ("b", rest[1]);
        EXPECT_STREQ("c", rest[2]);
        EXPECT_TRUE(rest[3] == NULL);
}

TEST_F(NXOptionsTest, parse_smaller) {
        nx_options_add(opt, "ssdibr",
                       "-o", "Output name", "out.txt",
                       "--list", "List name", "list.txt",
                       "-p|--precision", "Precision of the method", 2.0,
                       "-s", "Size of the buffers", 128,
                       "-v|--verbose", "Toggle verbose output", NX_FALSE,
                       "images");
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

        nx_options_set_from_args(opt, nx_string_array_size(args), nx_string_array_ptr(args));

        nx_string_array_resize(args, 1);
        nx_options_set_from_args(opt, nx_string_array_size(args), nx_string_array_ptr(args));

        EXPECT_STREQ("out.txt", nx_options_get_string(opt, "-o"));
        EXPECT_FALSE(nx_options_is_set(opt, "-o"));

        EXPECT_STREQ("list.txt", nx_options_get_string(opt, "--list"));
        EXPECT_FALSE(nx_options_is_set(opt, "--list"));

        EXPECT_EQ(2.0, nx_options_get_double(opt, "--precision"));
        EXPECT_FALSE(nx_options_is_set(opt, "-p"));

        EXPECT_EQ(128, nx_options_get_int(opt, "-s"));
        EXPECT_FALSE(nx_options_is_set(opt, "-s"));

        EXPECT_FALSE(nx_options_get_bool(opt, "--verbose"));
        EXPECT_FALSE(nx_options_is_set(opt, "-v"));

        char **rest = nx_options_get_rest(opt);
        EXPECT_TRUE(rest == NULL);
}

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
