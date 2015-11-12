#include <gtest/gtest.h>

#include <grammar/yy-misc.h>
#include <test/phony-errors.h>

#include "test-common.h"

using namespace test;

TEST(Syntax, Mix)
{
    grammar::parse();
    grammar::builder.buildAndClear().compile(nulSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
            (misc::position(3), FUNC_DEF, "fib")
                (misc::position(3), PARAMETER, "n")
            (BLOCK_BEGIN)
                (misc::position(4), FUNC_DEF, "add")
                (BLOCK_BEGIN)
                    (misc::position(5), RETURN)
                        (misc::position(5), BINARY_OP, "+")
                        (misc::position(5), OPERAND)
                            (misc::position(5), CALL_BEGIN)
                                (misc::position(5), IDENTIFIER, "fib")
                            (misc::position(5), ARGUMENTS)
                                (misc::position(5), IDENTIFIER, "n")
                            (misc::position(5), CALL_END)
                        (misc::position(5), OPERAND)
                            (misc::position(5), CALL_BEGIN)
                                (misc::position(5), IDENTIFIER, "fib")
                            (misc::position(5), ARGUMENTS)
                                (misc::position(5), BINARY_OP, "-")
                                (misc::position(5), OPERAND)
                                    (misc::position(5), IDENTIFIER, "n")
                                (misc::position(5), OPERAND)
                                    (misc::position(5), INTEGER, "1")
                            (misc::position(5), CALL_END)
                (BLOCK_END)

                (misc::position(6), BRANCH)
                    (misc::position(6), BINARY_OP, "<")
                    (misc::position(6), OPERAND)
                        (misc::position(6), IDENTIFIER, "n")
                    (misc::position(6), OPERAND)
                        (misc::position(6), INTEGER, "2")
                (CONSEQUENCE)
                (BLOCK_BEGIN)
                (BLOCK_END)
                (ALTERNATIVE)
                (BLOCK_BEGIN)
                    (misc::position(7), RETURN)
                            (misc::position(7), CALL_BEGIN)
                                (misc::position(7), IDENTIFIER, "add")
                            (misc::position(7), ARGUMENTS)
                            (misc::position(7), CALL_END)
                (BLOCK_END)

                (misc::position(8), RETURN)
                    (misc::position(8), INTEGER, "1")
            (BLOCK_END)

            (misc::position(15), FUNC_DEF, "print_10")
            (BLOCK_BEGIN)
                (misc::position(16), BRANCH)
                    (misc::position(16), BOOLEAN, "true")
                (CONSEQUENCE)
                (BLOCK_BEGIN)
                    (misc::position(17), ARITHMETICS)
                        (misc::position(17), CALL_BEGIN)
                            (misc::position(17), IDENTIFIER, "write")
                        (misc::position(17), ARGUMENTS)
                            (misc::position(17), PRE_UNARY_OP, "+")
                            (misc::position(17), OPERAND)
                                (misc::position(17), FLOATING, "1234.5")
                        (misc::position(17), CALL_END)
                (BLOCK_END)
                (ALTERNATIVE)
                (BLOCK_BEGIN)
                (BLOCK_END)
            (BLOCK_END)

            (misc::position(1), NAME_DEF, "n")
                (misc::position(1), INTEGER, "10")

                (misc::position(10), BRANCH)
                    (misc::position(10), BINARY_OP, ">")
                    (misc::position(10), OPERAND)
                        (misc::position(10), IDENTIFIER, "n")
                    (misc::position(10), OPERAND)
                        (misc::position(10), INTEGER, "1234567890")
                (CONSEQUENCE)
                (BLOCK_BEGIN)
                    (misc::position(11), ARITHMETICS)
                        (misc::position(11), CALL_BEGIN)
                            (misc::position(11), IDENTIFIER, "write")
                        (misc::position(11), ARGUMENTS)
                            (misc::position(11), CALL_BEGIN)
                                (misc::position(11), IDENTIFIER, "fib")
                            (misc::position(11), ARGUMENTS)
                                (misc::position(11), INTEGER, "10")
                            (misc::position(11), CALL_END)
                        (misc::position(11), CALL_END)
                (BLOCK_END)
                (ALTERNATIVE)
                (BLOCK_BEGIN)
                    (misc::position(13), ARITHMETICS)
                        (misc::position(13), CALL_BEGIN)
                            (misc::position(13), IDENTIFIER, "write")
                        (misc::position(13), ARGUMENTS)
                            (misc::position(13), BOOLEAN, "false")
                        (misc::position(13), CALL_END)
                (BLOCK_END)

            (misc::position(19), NAME_DEF, "x")
                (misc::position(19), IDENTIFIER, "print_10")
            (misc::position(20), NAME_DEF, "y")
                (misc::position(20), IDENTIFIER, "fib")

            (misc::position(22), ARITHMETICS)
                (misc::position(22), LIST_BEGIN)
                (misc::position(22), LIST_END)

            (misc::position(23), ARITHMETICS)
                (misc::position(23), LIST_BEGIN)
                    (misc::position(23), INTEGER, "0")
                (misc::position(23), LIST_END)

            (misc::position(24), ARITHMETICS)
                (misc::position(24), LIST_BEGIN)
                    (misc::position(24), IDENTIFIER, "panty")
                    (misc::position(24), IDENTIFIER, "stocking")
                    (misc::position(24), CALL_BEGIN)
                        (misc::position(24), IDENTIFIER, "garterbelt")
                    (misc::position(24), ARGUMENTS)
                        (misc::position(24), IDENTIFIER, "chuck")
                    (misc::position(24), CALL_END)
                (misc::position(24), LIST_END)

            (misc::position(25), ARITHMETICS)
                (misc::position(25), LIST_BEGIN)
                    (misc::position(25), LIST_BEGIN)
                        (misc::position(25), IDENTIFIER, "zergling")
                        (misc::position(25), IDENTIFIER, "hydralisk")
                    (misc::position(25), LIST_END)
                    (misc::position(25), LIST_BEGIN)
                        (misc::position(25), IDENTIFIER, "marine")
                        (misc::position(25), IDENTIFIER, "firebat")
                    (misc::position(25), LIST_END)
                    (misc::position(25), LIST_BEGIN)
                        (misc::position(25), IDENTIFIER, "zealot")
                        (misc::position(25), IDENTIFIER, "dragoon")
                    (misc::position(25), LIST_END)
                (misc::position(25), LIST_END)

            (misc::position(26), ARITHMETICS)
                (misc::position(26), BINARY_OP, "[ ++ ]")
                (misc::position(26), OPERAND)
                    (misc::position(26), LIST_BEGIN)
                        (misc::position(26), INTEGER, "0")
                    (misc::position(26), LIST_END)
                (misc::position(26), OPERAND)
                    (misc::position(26), LIST_BEGIN)
                    (misc::position(26), LIST_END)

            (misc::position(27), ARITHMETICS)
                (misc::position(27), BINARY_OP, "[ ++ ]")
                (misc::position(27), OPERAND)
                    (misc::position(27), BINARY_OP, "[ ++ ]")
                    (misc::position(27), OPERAND)
                        (misc::position(27), IDENTIFIER, "z")
                    (misc::position(27), OPERAND)
                        (misc::position(27), IDENTIFIER, "z")
                (misc::position(27), OPERAND)
                    (misc::position(27), IDENTIFIER, "z")

            (misc::position(29), ARITHMETICS)
                (misc::position(29), CALL_BEGIN)
                    (misc::position(29), BINARY_OP, "[ . ]")
                    (misc::position(29), OPERAND)
                        (misc::position(29), LIST_BEGIN)
                        (misc::position(29), LIST_END)
                    (misc::position(29), OPERAND)
                            (misc::position(29), IDENTIFIER, "size")
                (misc::position(29), ARGUMENTS)
                (misc::position(29), CALL_END)

            (misc::position(30), ARITHMETICS)
                (misc::position(30), CALL_BEGIN)
                    (misc::position(30), BINARY_OP, "[ . ]")
                    (misc::position(30), OPERAND)
                        (misc::position(30), IDENTIFIER, "x")
                    (misc::position(30), OPERAND)
                            (misc::position(30), IDENTIFIER, "empty")
                (misc::position(30), ARGUMENTS)
                (misc::position(30), CALL_END)

            (misc::position(31), ARITHMETICS)
                (misc::position(31), CALL_BEGIN)
                    (misc::position(31), BINARY_OP, "[ . ]")
                    (misc::position(31), OPERAND)
                        (misc::position(31), CALL_BEGIN)
                            (misc::position(31), IDENTIFIER, "xx")
                        (misc::position(31), ARGUMENTS)
                        (misc::position(31), CALL_END)
                    (misc::position(31), OPERAND)
                            (misc::position(31), IDENTIFIER, "first")
                (misc::position(31), ARGUMENTS)
                (misc::position(31), CALL_END)

            (misc::position(32), ARITHMETICS)
                (misc::position(32), CALL_BEGIN)
                    (misc::position(32), BINARY_OP, "[ . ]")
                    (misc::position(32), OPERAND)
                        (misc::position(32), BINARY_OP, "[ ++ ]")
                        (misc::position(32), OPERAND)
                            (misc::position(32), LIST_BEGIN)
                            (misc::position(32), LIST_END)
                        (misc::position(32), OPERAND)
                            (misc::position(32), LIST_BEGIN)
                            (misc::position(32), LIST_END)
                    (misc::position(32), OPERAND)
                        (misc::position(32), IDENTIFIER, "push_back")
                (misc::position(32), ARGUMENTS)
                    (misc::position(32), INTEGER, "0")
                (misc::position(32), CALL_END)

            (misc::position(33), ARITHMETICS)
                (misc::position(33), CALL_BEGIN)
                    (misc::position(33), BINARY_OP, "[ . ]")
                    (misc::position(33), OPERAND)
                        (misc::position(33), CALL_BEGIN)
                            (misc::position(33), BINARY_OP, "[ . ]")
                            (misc::position(33), OPERAND)
                                (misc::position(33), IDENTIFIER, "x")
                            (misc::position(33), OPERAND)
                                (misc::position(33), IDENTIFIER, "xx")
                        (misc::position(33), ARGUMENTS)
                        (misc::position(33), CALL_END)
                    (misc::position(33), OPERAND)
                        (misc::position(33), IDENTIFIER, "first")
                (misc::position(33), ARGUMENTS)
                (misc::position(33), CALL_END)

            (misc::position(35), ARITHMETICS)
                (misc::position(35), BINARY_OP, "[ |: ]")
                (misc::position(35), OPERAND)
                    (misc::position(35), BINARY_OP, "[ |? ]")
                    (misc::position(35), OPERAND)
                        (misc::position(35), LIST_BEGIN)
                        (misc::position(35), LIST_END)
                    (misc::position(35), OPERAND)
                        (misc::position(35), BINARY_OP, "=")
                        (misc::position(35), OPERAND)
                            (misc::position(35), PIPE_INDEX)
                        (misc::position(35), OPERAND)
                            (misc::position(35), INTEGER, "0")
                (misc::position(35), OPERAND)
                    (misc::position(35), PRE_UNARY_OP, "!")
                    (misc::position(35), OPERAND)
                        (misc::position(35), BINARY_OP, "*")
                        (misc::position(35), OPERAND)
                            (misc::position(35), PIPE_ELEMENT)
                        (misc::position(35), OPERAND)
                            (misc::position(35), PIPE_KEY)

            (misc::position(37), NAME_DEF, "s")
                (misc::position(37), BINARY_OP, "+")
                (misc::position(37), OPERAND)
                    (misc::position(37), STRING, "sizukani tiriyuku")
                (misc::position(37), OPERAND)
                    (misc::position(37), STRING, "the quick brown fox jumps over a lazy dog.")

            (misc::position(38), NAME_DEF, "sk")
                (misc::position(38), STRING, "a quote\" and something \\else in\t the\n string")

            (misc::position(39), NAME_DEF, "st")
                (misc::position(39), BINARY_OP, "+")
                (misc::position(39), OPERAND)
                    (misc::position(39), STRING, "nomico\"misato")
                (misc::position(39), OPERAND)
                    (misc::position(39), STRING, "nomico'misato")

            (misc::position(41), EXTERN)
                (misc::position(41), IDENTIFIER, "emirin")

            (misc::position(42), EXTERN)
                (misc::position(42), IDENTIFIER, "kagami")
                (misc::position(42), IDENTIFIER, "hiiragi")

            (misc::position(44), ATTR_SET)
                (misc::position(44), BINARY_OP, "[ . ]")
                (misc::position(44), OPERAND)
                    (misc::position(44), IDENTIFIER, "emirin")
                (misc::position(44), OPERAND)
                    (misc::position(44), IDENTIFIER, "katou")
                (misc::position(44), BINARY_OP, "[ . ]")
                (misc::position(44), OPERAND)
                    (misc::position(44), IDENTIFIER, "kagami")
                (misc::position(44), OPERAND)
                    (misc::position(44), IDENTIFIER, "hiiragi")

            (misc::position(46), NAME_DEF, "x")
                (misc::position(50), DICT_BEGIN)
                (misc::position(50), DICT_ITEM)
                    (misc::position(47), STRING, "_1")
                    (misc::position(47), INTEGER, "2")
                (misc::position(50), DICT_ITEM)
                    (misc::position(48), STRING, "_3")
                    (misc::position(48), INTEGER, "4")
                (misc::position(50), DICT_ITEM)
                    (misc::position(49), STRING, "_5")
                    (misc::position(49), INTEGER, "7")
                (misc::position(50), DICT_END)
            (misc::position(51), NAME_DEF, "y")
                (misc::position(51), DICT_BEGIN)
                (misc::position(51), DICT_END)

            (misc::position(56), NAME_DEF, "triple_quoted")
                (misc::position(60), BINARY_OP, "+")
                (misc::position(60), OPERAND)
                    (misc::position(56), BINARY_OP, "+")
                    (misc::position(56), OPERAND)
                        (misc::position(56), BINARY_OP, "+")
                        (misc::position(56), OPERAND)
                            (misc::position(56), STRING, "<input type='text' value=''>")
                        (misc::position(56), OPERAND)
                            (misc::position(56), STRING, "\\'")
                    (misc::position(56), OPERAND)
                        (misc::position(56), STRING, "\nthis very long text will\n"
                                                     "take at least 3 lines\nas it said\n")
                (misc::position(60), OPERAND)
                    (misc::position(60), STRING, "\nsingle quoted\n")

            (misc::position(63), NAME_DEF, "line_test")
                (misc::position(63), INTEGER, "0")

            (misc::position(65), ARITHMETICS)
                (misc::position(65), CALL_BEGIN)
                    (misc::position(65), IDENTIFIER, "log")
                (misc::position(65), ARGUMENTS)
                    (misc::position(65), BINARY_OP, "+")
                    (misc::position(65), OPERAND)
                        (misc::position(65), BINARY_OP, "+")
                        (misc::position(65), OPERAND)
                            (misc::position(65), STRING, "I think ")
                        (misc::position(65), OPERAND)
                            (misc::position(65), PRE_UNARY_OP, "[ typeof ]")
                            (misc::position(65), OPERAND)
                                (misc::position(65), INTEGER, "0")
                    (misc::position(65), OPERAND)
                        (misc::position(65), STRING, " is the type of 0.")
                (misc::position(65), CALL_END)

            (misc::position(67), NAME_DEF, "bitwise")
                (misc::position(67), BINARY_OP, "+")
                (misc::position(67), OPERAND)
                    (misc::position(67), IDENTIFIER, "x")
                (misc::position(67), OPERAND)
                    (misc::position(67), BINARY_OP, "*")
                    (misc::position(67), OPERAND)
                        (misc::position(67), PRE_UNARY_OP, "~")
                        (misc::position(67), OPERAND)
                            (misc::position(67), PRE_UNARY_OP, "-")
                            (misc::position(67), OPERAND)
                                (misc::position(67), IDENTIFIER, "y")
                    (misc::position(67), OPERAND)
                        (misc::position(67), IDENTIFIER, "z")

            (misc::position(68), NAME_DEF, "bitwiseX")
                (misc::position(68), BINARY_OP, "<")
                (misc::position(68), OPERAND)
                    (misc::position(68), IDENTIFIER, "x")
                (misc::position(68), OPERAND)
                    (misc::position(68), BINARY_OP, "<<")
                    (misc::position(68), OPERAND)
                        (misc::position(68), PRE_UNARY_OP, "~")
                        (misc::position(68), OPERAND)
                            (misc::position(68), IDENTIFIER, "y")
                    (misc::position(68), OPERAND)
                        (misc::position(68), IDENTIFIER, "z")

            (misc::position(69), NAME_DEF, "bitwiseY")
                (misc::position(69), BINARY_OP, "&")
                (misc::position(69), OPERAND)
                    (misc::position(69), BINARY_OP, ">>>")
                    (misc::position(69), OPERAND)
                        (misc::position(69), IDENTIFIER, "x")
                    (misc::position(69), OPERAND)
                        (misc::position(69), PRE_UNARY_OP, "~")
                        (misc::position(69), OPERAND)
                            (misc::position(69), IDENTIFIER, "y")
                (misc::position(69), OPERAND)
                    (misc::position(69), IDENTIFIER, "z")

            (misc::position(71), TRY)
                (BLOCK_BEGIN)
                    (misc::position(72), ARITHMETICS)
                        (misc::position(72), CALL_BEGIN)
                            (misc::position(72), IDENTIFIER, "f")
                        (misc::position(72), ARGUMENTS)
                        (misc::position(72), CALL_END)
                (BLOCK_END)
            (misc::position(71), CATCH)
                (BLOCK_BEGIN)
                    (misc::position(74), ARITHMETICS)
                        (misc::position(74), CALL_BEGIN)
                            (misc::position(74), IDENTIFIER, "g")
                        (misc::position(74), ARGUMENTS)
                            (misc::position(74), EXCEPTION_OBJ)
                        (misc::position(74), CALL_END)
                (BLOCK_END)
        (BLOCK_END)
    ;
    DataTree::verify();
}