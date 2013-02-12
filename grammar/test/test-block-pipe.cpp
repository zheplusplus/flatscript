#include <gtest/gtest.h>

#include <test/phony-errors.h>

#include "test-common.h"
#include "../yy-misc.h"

using namespace test;

TEST(Syntax, BlockPipe)
{
    yyparse();
    grammar::builder.buildAndClear().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
            (misc::position(1), NAME_DEF, "tasks")
                (misc::position(1), BINARY_OP, "[ pipeline ]")
                (misc::position(1), OPERAND)
                    (misc::position(1), IDENTIFIER, "x")
                (misc::position(1), OPERAND)
                    (BLOCK_BEGIN)
                        (misc::position(2), ARITHMETICS)
                            (misc::position(2), CALL_BEGIN)
                                (misc::position(2), BINARY_OP, "[ . ]")
                                (misc::position(2), OPERAND)
                                    (misc::position(2), PIPE_RESULT)
                                (misc::position(2), OPERAND)
                                    (misc::position(2), IDENTIFIER, "push")
                            (misc::position(2), ARGUMENTS)
                                (misc::position(2), INTEGER, "0")
                            (misc::position(2), CALL_END)
                    (BLOCK_END)
            (misc::position(3), ARITHMETICS)
                (misc::position(3), CALL_BEGIN)
                    (misc::position(3), IDENTIFIER, "execute")
                (misc::position(3), ARGUMENTS)
                    (misc::position(3), IDENTIFIER, "tasks")
                (misc::position(3), CALL_END)
            (misc::position(5), ARITHMETICS)
                (misc::position(5), CALL_BEGIN)
                    (misc::position(5), IDENTIFIER, "execute")
                (misc::position(5), ARGUMENTS)
                    (misc::position(5), BINARY_OP, "[ pipeline ]")
                    (misc::position(5), OPERAND)
                        (misc::position(5), IDENTIFIER, "x")
                    (misc::position(5), OPERAND)
                        (BLOCK_BEGIN)
                            (misc::position(6), ARITHMETICS)
                                (misc::position(6), CALL_BEGIN)
                                    (misc::position(6), BINARY_OP, "[ . ]")
                                    (misc::position(6), OPERAND)
                                        (misc::position(6), PIPE_RESULT)
                                    (misc::position(6), OPERAND)
                                        (misc::position(6), IDENTIFIER, "push")
                                (misc::position(6), ARGUMENTS)
                                    (misc::position(6), INTEGER, "1")
                                (misc::position(6), CALL_END)
                        (BLOCK_END)
                (misc::position(5), CALL_END)
        (BLOCK_END)
    ;
}
