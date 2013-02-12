#include "test-clauses.h"

using namespace test;

typedef ClausesTest AnonymousFunctionTest;

TEST_F(AnonymousFunctionTest, AsNameDef)
{
    misc::position pos(1);
    misc::position pos_a(100);
    misc::position pos_b(101);
    grammar::ClauseBuilder builder;
    builder.addArith(0, pos, (new grammar::TokenSequence(id(pos, "nami")))
                                                   ->add(colon(pos))
                                                   ->add(open(pos, "("))
                                                   ->add(close(pos, ")"))
                                                   ->add(colon(pos))
                                                   ->deliver());
    builder.addArith(1, pos_a, (new grammar::TokenSequence(id(pos_a, "mayo")))
                                                     ->add(colon(pos_a))
                                                     ->add(open(pos_a, "("))
                                                     ->add(id(pos_a, "kanako"))
                                                     ->add(close(pos_a, ")"))
                                                     ->deliver());
    builder.addArith(0, pos_b, (new grammar::TokenSequence(id(pos_b, "tarou")))
                                                     ->add(colon(pos_b))
                                                     ->add(open(pos_b, "("))
                                                     ->add(id(pos_b, "kaere"))
                                                     ->add(close(pos_b, ")"))
                                                     ->deliver());

    builder.buildAndClear().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, NAME_DEF, "nami")
            (pos, FUNC_DEF)
                (BLOCK_BEGIN)
                    (pos_a, NAME_DEF, "mayo")
                        (pos_a, IDENTIFIER, "kanako")
                (BLOCK_END)
        (pos_b, NAME_DEF, "tarou")
            (pos_b, IDENTIFIER, "kaere")
        (BLOCK_END)
    ;
}

TEST_F(AnonymousFunctionTest, TerminateByEnd)
{
    misc::position pos(2);
    misc::position pos_a(200);
    misc::position pos_b(201);
    grammar::ClauseBuilder builder;
    builder.addArith(0, pos, (new grammar::TokenSequence(id(pos, "itosiki")))
                                                   ->add(colon(pos))
                                                   ->add(open(pos, "("))
                                                   ->add(id(pos_a, "fuura"))
                                                   ->add(close(pos, ")"))
                                                   ->add(colon(pos))
                                                   ->deliver());
    builder.addArith(1, pos_a, (new grammar::TokenSequence(id(pos_a, "sekiuti")))
                                                     ->add(open(pos_a, "("))
                                                     ->add(id(pos_a, "mitama"))
                                                     ->add(close(pos_a, ")"))
                                                     ->deliver());
    builder.addArith(1, pos_b, (new grammar::TokenSequence(id(pos_b, "fujiyosi")))
                                                     ->add(open(pos_b, "("))
                                                     ->add(id(pos_b, "hitou"))
                                                     ->add(close(pos_b, ")"))
                                                     ->deliver());

    builder.buildAndClear().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, NAME_DEF, "itosiki")
            (pos, FUNC_DEF)
                (pos, PARAMETER, "fuura")
            (BLOCK_BEGIN)
                (pos_a, ARITHMETICS)
                    (pos_a, CALL_BEGIN)
                        (pos_a, IDENTIFIER, "sekiuti")
                    (pos_a, ARGUMENTS)
                        (pos_a, IDENTIFIER, "mitama")
                    (pos_a, CALL_END)
                (pos_b, ARITHMETICS)
                    (pos_b, CALL_BEGIN)
                        (pos_b, IDENTIFIER, "fujiyosi")
                    (pos_b, ARGUMENTS)
                        (pos_b, IDENTIFIER, "hitou")
                    (pos_b, CALL_END)
            (BLOCK_END)
        (BLOCK_END)
    ;
}

TEST_F(AnonymousFunctionTest, FuncAsArg)
{
    misc::position pos(3);
    misc::position pos_a(300);
    misc::position pos_b(301);
    grammar::ClauseBuilder builder;
    builder.addArith(0, pos, (new grammar::TokenSequence(id(pos, "ookusa")))
                                                   ->add(open(pos, "("))
                                                   ->add(open(pos, "("))
                                                   ->add(id(pos_a, "otonasi"))
                                                   ->add(close(pos, ")"))
                                                   ->add(colon(pos))
                                                   ->deliver());
    builder.addArith(1, pos_a, (new grammar::TokenSequence(id(pos_a, "kaga")))
                                                     ->add(open(pos_a, "("))
                                                     ->add(id(pos_a, "kitu"))
                                                     ->add(close(pos_a, ")"))
                                                     ->deliver());
    builder.addArith(0, pos_b, (new grammar::TokenSequence(comma(pos_b)))
                                                     ->add(open(pos_b, "("))
                                                     ->add(id(pos_b, "kimura"))
                                                     ->add(close(pos_b, ")"))
                                                     ->add(close(pos_b, ")"))
                                                     ->deliver());

    builder.buildAndClear().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, CALL_BEGIN)
                (pos, IDENTIFIER, "ookusa")
            (pos, ARGUMENTS)
                (pos, FUNC_DEF)
                    (pos, PARAMETER, "otonasi")
                (BLOCK_BEGIN)
                    (pos_a, ARITHMETICS)
                        (pos_a, CALL_BEGIN)
                            (pos_a, IDENTIFIER, "kaga")
                        (pos_a, ARGUMENTS)
                            (pos_a, IDENTIFIER, "kitu")
                        (pos_a, CALL_END)
                (BLOCK_END)
                (pos_b, IDENTIFIER, "kimura")
            (pos, CALL_END)
        (BLOCK_END)
    ;
}

TEST_F(AnonymousFunctionTest, TerminateByReturn)
{
    misc::position pos(4);
    misc::position pos_a(400);
    misc::position pos_b(401);
    grammar::ClauseBuilder builder;
    builder.addArith(0, pos, (new grammar::TokenSequence(id(pos, "kanji")))
                                                   ->add(colon(pos))
                                                   ->add(open(pos, "("))
                                                   ->add(id(pos_a, "makoto"))
                                                   ->add(close(pos, ")"))
                                                   ->add(colon(pos))
                                                   ->deliver());
    builder.addArith(1, pos_a, (new grammar::TokenSequence(id(pos_a, "harunobu")))
                                                     ->add(open(pos_a, "("))
                                                     ->add(close(pos_a, ")"))
                                                     ->deliver());
    builder.addReturn(0, pos_b, (new grammar::TokenSequence(id(pos_b, "souitirou")))->deliver());

    builder.buildAndClear().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, NAME_DEF, "kanji")
            (pos, FUNC_DEF)
                (pos, PARAMETER, "makoto")
            (BLOCK_BEGIN)
                (pos_a, ARITHMETICS)
                    (pos_a, CALL_BEGIN)
                        (pos_a, IDENTIFIER, "harunobu")
                    (pos_a, ARGUMENTS)
                    (pos_a, CALL_END)
            (BLOCK_END)
        (pos_b, RETURN)
            (pos_b, IDENTIFIER, "souitirou")
        (BLOCK_END)
    ;
}

TEST_F(AnonymousFunctionTest, TerminateByReturnIncompleted)
{
    misc::position pos(5);
    misc::position pos_a(500);
    misc::position pos_b(501);
    grammar::ClauseBuilder builder;
    builder.addArith(0, pos, (new grammar::TokenSequence(id(pos, "saki")))
                                                   ->add(open(pos, "("))
                                                   ->add(open(pos, "("))
                                                   ->add(id(pos_a, "tika"))
                                                   ->add(close(pos, ")"))
                                                   ->add(colon(pos))
                                                   ->deliver());
    builder.addArith(1, pos_a, (new grammar::TokenSequence(id(pos_a, "kanako")))
                                                     ->add(open(pos_a, "("))
                                                     ->add(close(pos_a, ")"))
                                                     ->deliver());
    builder.addReturn(0, pos_b, (new grammar::TokenSequence(id(pos_b, "keiko")))->deliver());

    builder.buildAndClear();
    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getUnexpectedTokenRecs().size());
    ASSERT_EQ(pos_b, getUnexpectedTokenRecs()[0].pos);
    ASSERT_EQ("return", getUnexpectedTokenRecs()[0].image);
    ASSERT_EQ(1, getUnexpectedEofRecs().size());
}
