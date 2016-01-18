#include <gtest/gtest.h>
#include <util/io.h>
#include <grammar/tokenize.h>
#include <test/phony-errors.h>

#include "test-common.h"

using namespace test;
typedef GrammarTest Tokenize;

static void assign(grammar::TokenizeResult& lhs, grammar::TokenizeResult&& rhs)
{
    lhs.tokens = std::move(rhs.tokens);
    lhs.iterator = rhs.iterator;
    lhs.lineno_inc = rhs.lineno_inc;
}

TEST_F(Tokenize, Empty)
{
    std::string s(util::read_file("test/grammar/empty.fls"));
    auto result(grammar::tokenize(s.begin(), s.end(), "", 1));
    ASSERT_EQ(0, result.tokens.size());
    ASSERT_EQ(0, result.lineno_inc);
    ASSERT_FALSE(error::hasError());
}

TEST_F(Tokenize, MultiLinesFile)
{
    int lineno = 101;
    std::string s(util::read_file("test/grammar/break.fls"));
    auto end = s.end();
    auto result(grammar::tokenize(s.begin(), end, "", lineno));
    ASSERT_EQ(4, result.tokens.size());
    ASSERT_EQ(1, result.lineno_inc);
    ASSERT_FALSE(error::hasError());

    int i = 0;
    ASSERT_EQ("x", result.tokens[i].image);
    ASSERT_EQ("identifier", result.tokens[i].type);
    ASSERT_EQ(101, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ(":", result.tokens[i].image);
    ASSERT_EQ("colon", result.tokens[i].type);
    ASSERT_EQ(101, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("y", result.tokens[i].image);
    ASSERT_EQ("identifier", result.tokens[i].type);
    ASSERT_EQ(101, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("*", result.tokens[i].image);
    ASSERT_EQ("operator", result.tokens[i].type);
    ASSERT_EQ(101, result.tokens[i].pos.line);

    lineno += result.lineno_inc;
    ::assign(result, grammar::tokenize(result.iterator, end, "", lineno));
    ASSERT_EQ(1, result.tokens.size());
    ASSERT_EQ(1, result.lineno_inc);
    ASSERT_FALSE(error::hasError());
    i = 0;
    ASSERT_EQ("z", result.tokens[i].image);
    ASSERT_EQ("identifier", result.tokens[i].type);
    ASSERT_EQ(102, result.tokens[i].pos.line);

    lineno += result.lineno_inc;
    ::assign(result, grammar::tokenize(result.iterator, end, "", lineno));
    ASSERT_EQ(4, result.tokens.size());
    ASSERT_EQ(1, result.lineno_inc);
    ASSERT_FALSE(error::hasError());
    i = 0;
    ASSERT_EQ("x", result.tokens[i].image);
    ASSERT_EQ("identifier", result.tokens[i].type);
    ASSERT_EQ(103, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ(":", result.tokens[i].image);
    ASSERT_EQ("colon", result.tokens[i].type);
    ASSERT_EQ(103, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("y", result.tokens[i].image);
    ASSERT_EQ("identifier", result.tokens[i].type);
    ASSERT_EQ(103, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("+", result.tokens[i].image);
    ASSERT_EQ("operator", result.tokens[i].type);
    ASSERT_EQ(103, result.tokens[i].pos.line);
}

TEST_F(Tokenize, Literals)
{
    int lineno = 500;
    std::string s("1 1. 1.1 .1");
    auto result(grammar::tokenize(s.begin(), s.end(), "", lineno));
    ASSERT_EQ(4, result.tokens.size());
    ASSERT_EQ(0, result.lineno_inc);
    ASSERT_FALSE(error::hasError());

    int i = 0;
    ASSERT_EQ("1", result.tokens[i].image);
    ASSERT_EQ("integer", result.tokens[i].type);
    ASSERT_EQ(lineno, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("1.", result.tokens[i].image);
    ASSERT_EQ("float", result.tokens[i].type);
    ASSERT_EQ(lineno, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("1.1", result.tokens[i].image);
    ASSERT_EQ("float", result.tokens[i].type);
    ASSERT_EQ(lineno, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ(".1", result.tokens[i].image);
    ASSERT_EQ("float", result.tokens[i].type);
    ASSERT_EQ(lineno, result.tokens[i].pos.line);
}

TEST_F(Tokenize, RegExp)
{
    int lineno = 600;
    std::string s("/a/ /@/b");
    auto result(grammar::tokenize(s.begin(), s.end(), "", lineno));
    ASSERT_EQ(2, result.tokens.size());
    ASSERT_EQ(0, result.lineno_inc);
    ASSERT_FALSE(error::hasError());

    int i = 0;
    ASSERT_EQ("/a/", result.tokens[i].image);
    ASSERT_EQ("regex", result.tokens[i].type);
    ASSERT_EQ(lineno, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("/@/b", result.tokens[i].image);
    ASSERT_EQ("regex", result.tokens[i].type);
    ASSERT_EQ(lineno, result.tokens[i].pos.line);

    s = "/0\n1";
    grammar::tokenize(s.begin(), s.end(), "", lineno);
    ASSERT_TRUE(error::hasError());
    auto recs(getUnexpectedEotRecs());
    ASSERT_EQ(1, recs.size());
    EXPECT_EQ(lineno, recs[0].pos.line);
    EXPECT_EQ("regex", recs[0].token_type);
    clearErr();

    s = "/\n1";
    ::assign(result, grammar::tokenize(s.begin(), s.end(), "", lineno));
    ASSERT_EQ(1, result.tokens.size());
    ASSERT_EQ(1, result.lineno_inc);
    ASSERT_FALSE(error::hasError());
    i = 0;
    ASSERT_EQ("/", result.tokens[i].image);
    ASSERT_EQ("operator", result.tokens[i].type);
    ASSERT_EQ(lineno, result.tokens[i].pos.line);

    s = "/ 1/";
    ::assign(result, grammar::tokenize(s.begin(), s.end(), "", lineno));
    ASSERT_EQ(3, result.tokens.size());
    ASSERT_EQ(0, result.lineno_inc);
    ASSERT_FALSE(error::hasError());
    i = 0;
    ASSERT_EQ("/", result.tokens[i].image);
    ASSERT_EQ("operator", result.tokens[i].type);
    ASSERT_EQ(lineno, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("1", result.tokens[i].image);
    ASSERT_EQ("integer", result.tokens[i].type);
    ASSERT_EQ(lineno, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("/", result.tokens[i].image);
    ASSERT_EQ("operator", result.tokens[i].type);
    ASSERT_EQ(lineno, result.tokens[i].pos.line);
}

TEST_F(Tokenize, MultiCharsOperators)
{
    std::string s("++--**//||");
    auto result(grammar::tokenize(s.begin(), s.end(), "", 200));
    ASSERT_EQ(7, result.tokens.size());
    ASSERT_EQ(0, result.lineno_inc);
    ASSERT_FALSE(error::hasError());
    int i = 0;
    ASSERT_EQ("++", result.tokens[i].image);
    ASSERT_EQ("operator", result.tokens[i].type);
    ASSERT_EQ(200, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("-", result.tokens[i].image);
    ASSERT_EQ("operator", result.tokens[i].type);
    ASSERT_EQ(200, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("-", result.tokens[i].image);
    ASSERT_EQ("operator", result.tokens[i].type);
    ASSERT_EQ(200, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("*", result.tokens[i].image);
    ASSERT_EQ("operator", result.tokens[i].type);
    ASSERT_EQ(200, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("*", result.tokens[i].image);
    ASSERT_EQ("operator", result.tokens[i].type);
    ASSERT_EQ(200, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("//", result.tokens[i].image);
    ASSERT_EQ("regex", result.tokens[i].type);
    ASSERT_EQ(200, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("||", result.tokens[i].image);
    ASSERT_EQ("operator", result.tokens[i].type);
    ASSERT_EQ(200, result.tokens[i].pos.line);
}

TEST_F(Tokenize, Strings)
{
    int lineno = 200;
    std::string s(util::read_file("test/grammar/test-tokenize-str.fls"));
    auto end = s.end();
    auto result(grammar::tokenize(s.begin(), end, "", lineno));
    ASSERT_EQ(4, result.tokens.size());
    ASSERT_EQ(1, result.lineno_inc);
    ASSERT_FALSE(error::hasError());

    int i = 0;
    ASSERT_EQ("''", result.tokens[i].image);
    ASSERT_EQ("string", result.tokens[i].type);
    ASSERT_EQ(200, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("\"\"", result.tokens[i].image);
    ASSERT_EQ("string", result.tokens[i].type);
    ASSERT_EQ(200, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("'\\''", result.tokens[i].image);
    ASSERT_EQ("string", result.tokens[i].type);
    ASSERT_EQ(200, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("\"\\\"\"", result.tokens[i].image);
    ASSERT_EQ("string", result.tokens[i].type);
    ASSERT_EQ(200, result.tokens[i].pos.line);

    lineno += result.lineno_inc;
    ::assign(result, grammar::tokenize(result.iterator, end, "", lineno));
    ASSERT_EQ(2, result.tokens.size());
    ASSERT_EQ(1, result.lineno_inc);
    ASSERT_FALSE(error::hasError());
    i = 0;
    ASSERT_EQ("'\"'", result.tokens[i].image);
    ASSERT_EQ("string", result.tokens[i].type);
    ASSERT_EQ(201, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("\"'\"", result.tokens[i].image);
    ASSERT_EQ("string", result.tokens[i].type);
    ASSERT_EQ(201, result.tokens[i].pos.line);

    lineno += result.lineno_inc;
    ::assign(result, grammar::tokenize(result.iterator, end, "", lineno));
    ASSERT_EQ(3, result.tokens.size());
    ASSERT_EQ(3, result.lineno_inc);
    ASSERT_FALSE(error::hasError());
    i = 0;
    ASSERT_TRUE(6 < result.tokens[i].image.size());
    ASSERT_EQ('\'', result.tokens[i].image[0]);
    ASSERT_EQ('\'', result.tokens[i].image[1]);
    ASSERT_EQ('\'', result.tokens[i].image[2]);
    ASSERT_EQ('\n', result.tokens[i].image[3]);
    ASSERT_EQ('\'', result.tokens[i].image[4]);
    ASSERT_EQ('T', result.tokens[i].image[5]);
    ASSERT_EQ('x', result.tokens[i].image[result.tokens[i].image.size() - 6]);
    ASSERT_EQ('\'', result.tokens[i].image[result.tokens[i].image.size() - 5]);
    ASSERT_EQ('\n', result.tokens[i].image[result.tokens[i].image.size() - 4]);
    ASSERT_EQ('\'', result.tokens[i].image[result.tokens[i].image.size() - 3]);
    ASSERT_EQ('\'', result.tokens[i].image[result.tokens[i].image.size() - 2]);
    ASSERT_EQ('\'', result.tokens[i].image[result.tokens[i].image.size() - 1]);
    ASSERT_EQ("triple_quoted_string", result.tokens[i].type);
    ASSERT_EQ(202, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("+", result.tokens[i].image);
    ASSERT_EQ("operator", result.tokens[i].type);
    ASSERT_EQ(204, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("\"el\"", result.tokens[i].image);
    ASSERT_EQ("string", result.tokens[i].type);
    ASSERT_EQ(204, result.tokens[i].pos.line);

    lineno += result.lineno_inc;
    ::assign(result, grammar::tokenize(result.iterator, end, "", lineno));
    ASSERT_EQ(3, result.tokens.size());
    ASSERT_EQ(3, result.lineno_inc);
    ASSERT_FALSE(error::hasError());
    i = 0;
    ASSERT_TRUE(6 < result.tokens[i].image.size());
    ASSERT_EQ('"', result.tokens[i].image[0]);
    ASSERT_EQ('"', result.tokens[i].image[1]);
    ASSERT_EQ('"', result.tokens[i].image[2]);
    ASSERT_EQ('\n', result.tokens[i].image[3]);
    ASSERT_EQ('"', result.tokens[i].image[4]);
    ASSERT_EQ('j', result.tokens[i].image[5]);
    ASSERT_EQ('g', result.tokens[i].image[result.tokens[i].image.size() - 6]);
    ASSERT_EQ('"', result.tokens[i].image[result.tokens[i].image.size() - 5]);
    ASSERT_EQ('\n', result.tokens[i].image[result.tokens[i].image.size() - 4]);
    ASSERT_EQ('"', result.tokens[i].image[result.tokens[i].image.size() - 3]);
    ASSERT_EQ('"', result.tokens[i].image[result.tokens[i].image.size() - 2]);
    ASSERT_EQ('"', result.tokens[i].image[result.tokens[i].image.size() - 1]);
    ASSERT_EQ("triple_quoted_string", result.tokens[i].type);
    ASSERT_EQ(205, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("+", result.tokens[i].image);
    ASSERT_EQ("operator", result.tokens[i].type);
    ASSERT_EQ(207, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("'psy'", result.tokens[i].image);
    ASSERT_EQ("string", result.tokens[i].type);
    ASSERT_EQ(207, result.tokens[i].pos.line);

    lineno += result.lineno_inc;
    ::assign(result, grammar::tokenize(result.iterator, end, "", lineno));
    ASSERT_EQ(3, result.tokens.size());
    ASSERT_EQ(1, result.lineno_inc);
    ASSERT_FALSE(error::hasError());
    i = 0;
    ASSERT_EQ(10, result.tokens[i].image.size());
    ASSERT_EQ('\'', result.tokens[i].image[0]);
    ASSERT_EQ('\'', result.tokens[i].image[1]);
    ASSERT_EQ('\'', result.tokens[i].image[2]);
    ASSERT_EQ('\\', result.tokens[i].image[3]);
    ASSERT_EQ('\\', result.tokens[i].image[4]);
    ASSERT_EQ('\\', result.tokens[i].image[5]);
    ASSERT_EQ('\'', result.tokens[i].image[6]);
    ASSERT_EQ('\'', result.tokens[i].image[7]);
    ASSERT_EQ('\'', result.tokens[i].image[8]);
    ASSERT_EQ('\'', result.tokens[i].image[9]);
    ASSERT_EQ("triple_quoted_string", result.tokens[i].type);
    ASSERT_EQ(208, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ("+", result.tokens[i].image);
    ASSERT_EQ("operator", result.tokens[i].type);
    ASSERT_EQ(208, result.tokens[i].pos.line);
    ++i;
    ASSERT_EQ(10, result.tokens[i].image.size());
    ASSERT_EQ('"', result.tokens[i].image[0]);
    ASSERT_EQ('"', result.tokens[i].image[1]);
    ASSERT_EQ('"', result.tokens[i].image[2]);
    ASSERT_EQ('\\', result.tokens[i].image[3]);
    ASSERT_EQ('\\', result.tokens[i].image[4]);
    ASSERT_EQ('\\', result.tokens[i].image[5]);
    ASSERT_EQ('"', result.tokens[i].image[6]);
    ASSERT_EQ('"', result.tokens[i].image[7]);
    ASSERT_EQ('"', result.tokens[i].image[8]);
    ASSERT_EQ('"', result.tokens[i].image[9]);
    ASSERT_EQ("triple_quoted_string", result.tokens[i].type);
    ASSERT_EQ(208, result.tokens[i].pos.line);
}

TEST_F(Tokenize, InvChars)
{
    int lineno = 300;
    std::string s(util::read_file("samples/errors/inv-chars.fls"));
    auto end = s.end();
    auto result(grammar::tokenize(s.begin(), end, "", lineno));
    ASSERT_TRUE(error::hasError());

    auto recs(getInvalidCharRecs());
    ASSERT_EQ(1, recs.size());
    EXPECT_EQ(300, recs[0].pos.line);
    EXPECT_EQ(int('`'), recs[0].character);

    lineno += result.lineno_inc;
    ::assign(result, grammar::tokenize(result.iterator, end, "", lineno));
    recs = getInvalidCharRecs();
    ASSERT_EQ(2, recs.size());
    EXPECT_EQ(300, recs[0].pos.line);
    EXPECT_EQ(int('`'), recs[0].character);
    EXPECT_EQ(301, recs[1].pos.line);
    EXPECT_EQ(int('?'), recs[1].character);

    lineno += result.lineno_inc;
    ::assign(result, grammar::tokenize(result.iterator, end, "", lineno));
    recs = getInvalidCharRecs();
    ASSERT_EQ(2, recs.size());

    lineno += result.lineno_inc;
    ::assign(result, grammar::tokenize(result.iterator, end, "", lineno));
    recs = getInvalidCharRecs();
    ASSERT_EQ(3, recs.size());
    EXPECT_EQ(303, recs[2].pos.line);
    EXPECT_EQ(int(';'), recs[2].character);
}

TEST_F(Tokenize, AfterInvChars)
{
    int lineno = 700;
    std::string s("console? log");
    auto end = s.end();
    auto result(grammar::tokenize(s.begin(), end, "", lineno));
    ASSERT_TRUE(error::hasError());

    auto recs(getInvalidCharRecs());
    ASSERT_EQ(1, recs.size());
    EXPECT_EQ(700, recs[0].pos.line);
    EXPECT_EQ(int('?'), recs[0].character);
}

TEST_F(Tokenize, UnfinishedString)
{
    int lineno = 400;
    std::string s("\"");

    grammar::tokenize(s.begin(), s.end(), "", lineno);
    ASSERT_TRUE(error::hasError());
    auto recs(getUnexpectedEotRecs());
    ASSERT_EQ(1, recs.size());
    EXPECT_EQ(400, recs[0].pos.line);
    EXPECT_EQ("string", recs[0].token_type);
    clearErr();

    s = "'a";
    grammar::tokenize(s.begin(), s.end(), "", lineno);
    recs = getUnexpectedEotRecs();
    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, recs.size());
    EXPECT_EQ(400, recs[0].pos.line);
    EXPECT_EQ("string", recs[0].token_type);
    clearErr();

    s = "'''";
    grammar::tokenize(s.begin(), s.end(), "", lineno);
    recs = getUnexpectedEotRecs();
    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, recs.size());
    EXPECT_EQ(400, recs[0].pos.line);
    EXPECT_EQ("triple_quoted_string", recs[0].token_type);
}

TEST_F(Tokenize, AfterComment)
{
    int lineno = 800;
    std::string s("a # Ei\nb");
    auto result(grammar::tokenize(s.begin(), s.end(), "", lineno));
    ASSERT_EQ(1, result.tokens.size());
    ASSERT_EQ(1, result.lineno_inc);
    ASSERT_FALSE(error::hasError());
    ASSERT_EQ("a", result.tokens[0].image);
    ASSERT_EQ("identifier", result.tokens[0].type);
    ASSERT_EQ(800, result.tokens[0].pos.line);

    ASSERT_TRUE(result.iterator != s.end());
    ASSERT_EQ('b', *result.iterator);
}
