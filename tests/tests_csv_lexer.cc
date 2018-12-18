/**
 * @file tests_csv_lexer.cc
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
#include "virg/nexus/nx_csv_lexer.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_string.h"

using namespace std;

namespace {

class NXCSVLexerTest : public ::testing::Test {
protected:
        NXCSVLexerTest() {
        }

        virtual void SetUp() {
        }

        virtual void TearDown() {
        }
};

TEST_F(NXCSVLexerTest, new_free) {
        char *s = nx_strdup("");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVLexerTest, single_record) {
        char *s = nx_strdup("123,abcd,,\"abcd\",123.0");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);

        struct NXCSVToken t = { NX_CTT_INVALID, NULL};
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_FIELD, t.type);
        EXPECT_STREQ("123", t.text);
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_COMMA, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_FIELD, t.type);
        EXPECT_STREQ("abcd", t.text);
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_COMMA, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_COMMA, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_QUOTED_FIELD, t.type);
        EXPECT_STREQ("abcd", t.text);
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_COMMA, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_FIELD, t.type);
        EXPECT_STREQ("123.0", t.text);
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_EOF, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_csv_token_free_data(&t);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVLexerTest, quotes_in_field) {
        char *s = nx_strdup("\"ab\"\"cd\"");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);

        struct NXCSVToken t = { NX_CTT_INVALID, NULL};
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_QUOTED_FIELD, t.type);
        EXPECT_STREQ("ab\"cd", t.text);
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_EOF, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_csv_token_free_data(&t);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVLexerTest, newline_in_field) {
        char *s = nx_strdup("\"ab\ncd\"");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);

        struct NXCSVToken t = { NX_CTT_INVALID, NULL};
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_QUOTED_FIELD, t.type);
        EXPECT_STREQ("ab\ncd", t.text);
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_EOF, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_csv_token_free_data(&t);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVLexerTest, two_records) {
        char *s = nx_strdup("abcd,,\"abcd\"\nabcd,\"abcd\",");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);

        struct NXCSVToken t = { NX_CTT_INVALID, NULL};
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_FIELD, t.type);
        EXPECT_STREQ("abcd", t.text);
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_COMMA, t.type);
        EXPECT_EQ(NULL, t.text);
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_COMMA, t.type);
        EXPECT_EQ(NULL, t.text);
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_QUOTED_FIELD, t.type);
        EXPECT_STREQ("abcd", t.text);

        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_EOR, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_FIELD, t.type);
        EXPECT_STREQ("abcd", t.text);
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_COMMA, t.type);
        EXPECT_EQ(NULL, t.text);
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_QUOTED_FIELD, t.type);
        EXPECT_STREQ("abcd", t.text);
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_COMMA, t.type);
        EXPECT_EQ(NULL, t.text);
        nx_csv_lexer_next_token(clex, &t);
        EXPECT_EQ(NX_CTT_EOF, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_csv_token_free_data(&t);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVLexerTest, unquoted_double_quote_DEATH) {
        char *s = nx_strdup("ab\"cd");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);

        struct NXCSVToken t = { NX_CTT_INVALID, NULL};
        EXPECT_DEATH({nx_csv_lexer_next_token(clex, &t);},"Unquoted fields can not contain \"");
}

} // namespace
