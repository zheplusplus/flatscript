#include <gtest/gtest.h>
#include <grammar/syntax.h>
#include <test/phony-errors.h>

#include "test-common.h"

using namespace test;
typedef GrammarTest Syntax;

TEST_F(Syntax, Empty)
{
    auto result(grammar::build("", std::string()));
    result->compile(nulScope());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (BLOCK_END)
    ;
    DataTree::verify();
}

TEST_F(Syntax, SingleStatement)
{
    auto result(grammar::build("", "a + b"));
    result->compile(nulScope());
    ASSERT_FALSE(error::hasError());

    misc::position line(1);
    DataTree::expectOne()
        (BLOCK_BEGIN)
            (line, ARITHMETICS)
                (line, BINARY_OP, "+")
                (line, OPERAND)
                    (line, IDENTIFIER, "a")
                (line, OPERAND)
                    (line, IDENTIFIER, "b")
        (BLOCK_END)
    ;
    DataTree::verify();
}

TEST_F(Syntax, CoupleStatements)
{
    auto result(grammar::build("", "a + b\nc(0)"));
    result->compile(nulScope());
    ASSERT_FALSE(error::hasError());

    misc::position line1(1);
    misc::position line2(2);
    DataTree::expectOne()
        (BLOCK_BEGIN)
            (line1, ARITHMETICS)
                (line1, BINARY_OP, "+")
                (line1, OPERAND)
                    (line1, IDENTIFIER, "a")
                (line1, OPERAND)
                    (line1, IDENTIFIER, "b")
            (line2, ARITHMETICS)
                (line2, CALL_BEGIN)
                    (line2, IDENTIFIER, "c")
                (line2, ARGUMENTS)
                    (line2, INTEGER, "0")
                (line2, CALL_END)
        (BLOCK_END)
    ;
    DataTree::verify();
}

TEST_F(Syntax, AfterComment)
{
    auto result(grammar::build("", "0 # zero\n1"));
    result->compile(nulScope());
    ASSERT_FALSE(error::hasError());

    misc::position line1(1);
    misc::position line2(2);
    DataTree::expectOne()
        (BLOCK_BEGIN)
            (line1, ARITHMETICS)
                (line1, INTEGER, "0")
            (line2, ARITHMETICS)
                (line2, INTEGER, "1")
        (BLOCK_END)
    ;
    DataTree::verify();
}
