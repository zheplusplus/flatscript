#include <gtest/gtest.h>

#include <test/phony-errors.h>

#include "test-common.h"
#include "../yy-misc.h"

using namespace test;

TEST(Syntax, LookupAndSlice)
{
    yyparse();
    grammar::builder.buildAndClear().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
            (misc::position(1), ARITHMETICS)
                (misc::position(1), BINARY_OP, "[]")
                (misc::position(1), OPERAND)
                    (misc::position(1), IDENTIFIER, "x")
                (misc::position(1), OPERAND)
                    (misc::position(1), INTEGER, "0")
            (misc::position(2), ARITHMETICS)
                (misc::position(2), BINARY_OP, "[]")
                (misc::position(2), OPERAND)
                    (misc::position(2), IDENTIFIER, "y")
                (misc::position(2), OPERAND)
                    (misc::position(2), IDENTIFIER, "x")
            (misc::position(3), ARITHMETICS)
                (misc::position(3), BINARY_OP, "[]")
                (misc::position(3), OPERAND)
                    (misc::position(3), BINARY_OP, "[]")
                    (misc::position(3), OPERAND)
                        (misc::position(3), IDENTIFIER, "z")
                    (misc::position(3), OPERAND)
                        (misc::position(3), IDENTIFIER, "y")
                (misc::position(3), OPERAND)
                    (misc::position(3), IDENTIFIER, "x")
            (misc::position(4), ARITHMETICS)
                (misc::position(4), CALL_BEGIN)
                    (misc::position(4), BINARY_OP, "[]")
                    (misc::position(4), OPERAND)
                        (misc::position(4), BINARY_OP, "[ . ]")
                        (misc::position(4), OPERAND)
                            (misc::position(4), IDENTIFIER, "a")
                        (misc::position(4), OPERAND)
                            (misc::position(4), IDENTIFIER, "b")
                    (misc::position(4), OPERAND)
                        (misc::position(4), IDENTIFIER, "y")
                (misc::position(4), ARGUMENTS)
                    (misc::position(4), IDENTIFIER, "z")
                (misc::position(4), CALL_END)
            (misc::position(5), ARITHMETICS)
                (misc::position(5), LIST_SLICE_BEGIN)
                    (misc::position(5), LIST_BEGIN)
                        (misc::position(5), IDENTIFIER, "a")
                        (misc::position(5), IDENTIFIER, "b")
                    (misc::position(5), LIST_END)
                    (misc::position(5), INTEGER, "1")
                    (misc::position(5), INTEGER, "2")
                    (misc::position(5), IDENTIFIER, "i")
                (misc::position(5), LIST_SLICE_END)
            (misc::position(6), ARITHMETICS)
                (misc::position(6), LIST_SLICE_BEGIN)
                    (misc::position(6), IDENTIFIER, "m")
                    (misc::position(6), INTEGER, "1")
                    (misc::position(6), UNDEFINED)
                    (misc::position(6), IDENTIFIER, "i")
                (misc::position(6), LIST_SLICE_END)
            (misc::position(7), ARITHMETICS)
                (misc::position(7), LIST_SLICE_BEGIN)
                    (misc::position(7), IDENTIFIER, "m")
                    (misc::position(7), UNDEFINED)
                    (misc::position(7), UNDEFINED)
                    (misc::position(7), IDENTIFIER, "i")
                (misc::position(7), LIST_SLICE_END)
            (misc::position(8), ARITHMETICS)
                (misc::position(8), LIST_SLICE_BEGIN)
                    (misc::position(8), IDENTIFIER, "m")
                    (misc::position(8), UNDEFINED)
                    (misc::position(8), UNDEFINED)
                    (misc::position(8), UNDEFINED)
                (misc::position(8), LIST_SLICE_END)
        (BLOCK_END)
    ;
    DataTree::verify();
}
