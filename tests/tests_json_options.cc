/**
 * @file tests_json_options.cc
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
#include "virg/nexus/nx_json_options.h"

#define make_add_test(type,fmt,value,cmp)                               \
        TEST_F(NXJSONOptionsTest, add_##type) {                         \
                nx_json_options_add(jopt, fmt, #type, value);           \
                EXPECT_##cmp(value, nx_json_options_get_##type(jopt, #type)); \
                EXPECT_FALSE(nx_json_options_is_set(jopt, #type));      \
        }

#define make_add_array_test(type,type_name,fmt)                         \
        TEST_F(NXJSONOptionsTest, add_##type_name##_array) {            \
                nx_json_options_add(jopt, fmt, #fmt "_array");          \
                size_t n = -1;                                          \
                type *arr = nx_json_options_get_##type_name##_array(jopt, #fmt "_array", &n); \
                        EXPECT_TRUE(arr == NULL);                       \
                        EXPECT_EQ(0, n);                                \
                        EXPECT_FALSE(nx_json_options_is_set(jopt, #fmt "_array")); \
        }


#define make_set_test(type,fmt,def_value,value,cmp)                     \
        TEST_F(NXJSONOptionsTest, parse_##type) {                       \
                nx_json_options_add(jopt, fmt, #type, def_value);       \
                nx_json_options_set_##type(jopt, #type, value);         \
                EXPECT_##cmp(value, nx_json_options_get_##type(jopt, #type)); \
                EXPECT_TRUE(nx_json_options_is_set(jopt, #type));      \
        }


using namespace std;

namespace {

class NXJSONOptionsTest : public ::testing::Test {
protected:
        NXJSONOptionsTest() {
        }

        virtual void SetUp() {
                jopt = nx_json_options_alloc();
        }

        virtual void TearDown() {
                nx_json_options_free(jopt);
        }

        struct NXJSONOptions *jopt;
};

TEST_F(NXJSONOptionsTest, alloc) {
        EXPECT_TRUE(jopt != NULL);
}

make_add_test(int,"i",123,EQ)
make_add_test(double,"d",123.0,EQ)
make_add_test(bool,"b",NX_FALSE,EQ)
make_add_test(string,"s","123",STREQ)

make_set_test(int,"i",123,321,EQ)
make_set_test(double,"d",123.0,321.0,EQ)
make_set_test(bool,"b",NX_FALSE,NX_TRUE,EQ)
make_set_test(string,"s","123","321",STREQ)

make_add_array_test(int,int,"i*")
make_add_array_test(double,double,"d*")
make_add_array_test(NXBool,bool,"b*")
make_add_array_test(char*,string,"s*")

} // namespace
