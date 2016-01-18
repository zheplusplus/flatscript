#include "samples-test.h"

using namespace test;

TEST_F(SyntaxSampleTest, MultiLevelLambdas)
{
    parseSampleOk("test/grammar/multi-level-lambdas.fls");

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (misc::position(1), ARITHMETICS)
            (misc::position(1), CALL_BEGIN)
                (misc::position(1), IDENTIFIER, "f")
            (misc::position(1), ARGUMENTS)
                (misc::position(1), FUNC_DEF)
                    (misc::position(1), PARAMETER, "x")
                (BLOCK_BEGIN)
                    (misc::position(2), ARITHMETICS)
                        (misc::position(2), CALL_BEGIN)
                            (misc::position(2), IDENTIFIER, "g")
                        (misc::position(2), ARGUMENTS)
                            (misc::position(2), FUNC_DEF)
                                (misc::position(2), PARAMETER, "y")
                            (BLOCK_BEGIN)
                                (misc::position(3), ARITHMETICS)
                                    (misc::position(3), CALL_BEGIN)
                                        (misc::position(3), IDENTIFIER, "x")
                                    (misc::position(3), ARGUMENTS)
                                        (misc::position(3), IDENTIFIER, "y")
                                    (misc::position(3), CALL_END)
                            (BLOCK_END)
                            (misc::position(4), IDENTIFIER, "m")
                        (misc::position(2), CALL_END)
                (BLOCK_END)
                (misc::position(5), IDENTIFIER, "n")
            (misc::position(1), CALL_END)
        (BLOCK_END)
    ;
    DataTree::verify();
}
