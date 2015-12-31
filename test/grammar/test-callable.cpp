#include <gtest/gtest.h>

#include <grammar/yy-misc.h>
#include <test/phony-errors.h>

#include "test-common.h"

using namespace test;

TEST(Syntax, Callable)
{
    grammar::parse();
    grammar::builder.buildAndClear()->compile(nulScope());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
            (misc::position(1), ARITHMETICS)
                (misc::position(1), CALL_BEGIN)
                    (misc::position(1), BINARY_OP, "[ . ]")
                    (misc::position(1), OPERAND)
                        (misc::position(1), IDENTIFIER, "kata")
                    (misc::position(1), OPERAND)
                        (misc::position(1), IDENTIFIER, "gosi")
                (misc::position(1), ARGUMENTS)
                    (misc::position(1), BINARY_OP, "[ . ]")
                    (misc::position(1), OPERAND)
                        (misc::position(1), IDENTIFIER, "no")
                    (misc::position(1), OPERAND)
                        (misc::position(1), IDENTIFIER, "sora")
                (misc::position(1), CALL_END)
            (misc::position(2), ARITHMETICS)
                (misc::position(2), CALL_BEGIN)
                    (misc::position(2), CALL_BEGIN)
                        (misc::position(2), IDENTIFIER, "eternal")
                    (misc::position(2), ARGUMENTS)
                    (misc::position(2), CALL_END)
                (misc::position(2), ARGUMENTS)
                    (misc::position(2), IDENTIFIER, "rite")
                (misc::position(2), CALL_END)
        (BLOCK_END)
    ;
    DataTree::verify();
}
