#include "test-clauses.h"
#include "../function.h"
#include "../stmt-nodes.h"

using namespace test;

typedef ClausesTest ClauseBuilderTest;

TEST_F(ClauseBuilderTest, Empty)
{
    misc::position pos(1);

    grammar::ClauseBuilder builder;
    builder.buildAndClear().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (BLOCK_END)
    ;
}

TEST_F(ClauseBuilderTest, IfBranch)
{
    misc::position pos(2);

    grammar::ClauseBuilder builder;
    builder.addArith(0, pos, (new grammar::TokenSequence(
              new grammar::TypedToken(pos, "if", grammar::IF)))
        ->add(new grammar::FactorToken(
                        pos, util::mkptr(new grammar::Identifier(pos, "kaeri")), "kaeri"))
        ->add(op(pos, "+"))
        ->deliver());
    builder.addArith(2, pos, (new grammar::TokenSequence(
              new grammar::FactorToken(
                        pos, util::mkptr(new grammar::Identifier(pos, "miti")), "miti")))
                                  ->deliver());
    builder.addArith(1, pos, (new grammar::TokenSequence(
              new grammar::FactorToken(
                        pos, util::mkptr(new grammar::Identifier(pos, "mayoi")), "mayoi")))
                                  ->deliver());

    builder.buildAndClear().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, BRANCH_CONSQ_ONLY)
            (pos, BINARY_OP, "+")
            (pos, OPERAND)
                (pos, IDENTIFIER, "kaeri")
            (pos, OPERAND)
                (pos, IDENTIFIER, "miti")
        (CONSEQUENCE)
            (BLOCK_BEGIN)
            (pos, ARITHMETICS)
                (pos, IDENTIFIER, "mayoi")
            (BLOCK_END)
        (BLOCK_END)
    ;
}

TEST_F(ClauseBuilderTest, IfBranchErrorElseAlreadyMatched)
{
    misc::position pos(3);
    misc::position else_pos_a(300);
    misc::position else_pos_b(301);

    grammar::ClauseBuilder builder;
    builder.addArith(0, pos, (new grammar::TokenSequence(
                    new grammar::TypedToken(pos, "if", grammar::IF)))
              ->add(new grammar::FactorToken(
                            pos, util::mkptr(new grammar::Identifier(pos, "yes")), "yes"))
              ->deliver());
    builder.addArith(1, pos, (new grammar::TokenSequence(
              new grammar::FactorToken(
                            pos, util::mkptr(new grammar::Identifier(pos, "i")), "i")))
                                  ->deliver());
    builder.addArith(0, else_pos_a, (new grammar::TokenSequence(
                    new grammar::TypedToken(else_pos_a, "else", grammar::ELSE)))
                                                ->deliver());
    builder.addArith(1, pos, (new grammar::TokenSequence(
              new grammar::FactorToken(
                            pos, util::mkptr(new grammar::Identifier(pos, "will")), "will")))
                                  ->deliver());
    builder.addArith(0, else_pos_b, (new grammar::TokenSequence(
                    new grammar::TypedToken(else_pos_b, "else", grammar::ELSE)))
                  ->deliver());
    builder.addArith(1, pos, (new grammar::TokenSequence(
              new grammar::FactorToken(
                            pos, util::mkptr(new grammar::Identifier(pos, "nadia")), "nadia")))
                                  ->deliver());
    builder.buildAndClear();

    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getIfAlreadyMatchElseRecs().size());
    ASSERT_EQ(else_pos_a, getIfAlreadyMatchElseRecs()[0].prev_else_pos);
    ASSERT_EQ(else_pos_b, getIfAlreadyMatchElseRecs()[0].this_else_pos);
}

TEST_F(ClauseBuilderTest, ClauseBuilder)
{
    misc::position item_pos0(4);
    misc::position item_pos1(5);
    misc::position item_pos2(6);

    grammar::ClauseBuilder builder0;
    builder0.addArith(0, item_pos0, (new grammar::TokenSequence(
                  new grammar::TypedToken(item_pos0, "if", grammar::IF)))
            ->add(new grammar::FactorToken(
                      item_pos0, util::mkptr(new grammar::BoolLiteral(item_pos0, true)), "true"))
            ->deliver());
        builder0.addArith(1, item_pos1, (new grammar::TokenSequence(
                new grammar::FactorToken(item_pos1, util::mkptr(
                                new grammar::Identifier(item_pos1, "wind_force")), "wind_force")))
          ->add(new grammar::TypedToken(item_pos1, ":", grammar::COLON))
          ->add(new grammar::FactorToken(
                      item_pos1, util::mkptr(new grammar::IntLiteral(item_pos1, "13571")), "13571"))
          ->deliver());
        builder0.addIfnot(1, item_pos2, (new grammar::TokenSequence(
                new grammar::FactorToken(item_pos2, util::mkptr(
                                new grammar::Identifier(item_pos2, "raven_claw")), "raven_claw")))
                                          ->deliver());
            builder0.addReturn(2, item_pos0, (new grammar::TokenSequence(
                new grammar::FactorToken(item_pos0, util::mkptr(
                                new grammar::FloatLiteral(item_pos0, "0.000123")), "0.000123")))
                                                    ->deliver());
        builder0.addReturn(1, item_pos1, std::vector<util::sptr<grammar::Token>>());
    builder0.addArith(0, item_pos2, (new grammar::TokenSequence(
                    new grammar::TypedToken(item_pos2, "else", grammar::ELSE)))
                  ->deliver());
        builder0.addIfnot(1, item_pos2, (new grammar::TokenSequence(
                new grammar::FactorToken(item_pos2, util::mkptr(
                                new grammar::Identifier(item_pos2, "cliffkiller")), "cliffkiller")))
                                          ->deliver());
            builder0.addIfnot(2, item_pos0, (new grammar::TokenSequence(
                new grammar::FactorToken(item_pos0, util::mkptr(
                                new grammar::Identifier(item_pos0, "skystrike")), "skystrike")))
                                              ->deliver());
    builder0.addArith(0, item_pos1, (new grammar::TokenSequence(
                    new grammar::TypedToken(item_pos1, "func", grammar::FUNC)))
                  ->add(new grammar::FactorToken(item_pos1, util::mkptr(
                                new grammar::Identifier(item_pos1, "goldenstrike_arch"))
                          , "goldenstrike_arch"))
                  ->add(open(item_pos1, "("))
                  ->add(new grammar::FactorToken(item_pos1, util::mkptr(
                                new grammar::Identifier(item_pos1, "amn")), "amn"))
                  ->add(comma(item_pos1))
                  ->add(new grammar::FactorToken(item_pos1, util::mkptr(
                                new grammar::Identifier(item_pos1, "tir")), "tir"))
                  ->add(close(item_pos1, ")"))
                  ->deliver());
        builder0.addArith(1, item_pos1, (new grammar::TokenSequence(
                new grammar::FactorToken(item_pos1, util::mkptr(
                                new grammar::Identifier(item_pos1, "widowmaker")), "widowmaker")))
                              ->deliver());

    builder0.buildAndClear().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (item_pos1, FUNC_DEF, "goldenstrike_arch")
            (item_pos1, PARAMETER, "amn")
            (item_pos1, PARAMETER, "tir")
            (BLOCK_BEGIN)
            (item_pos1, ARITHMETICS)
                (item_pos1, IDENTIFIER, "widowmaker")
            (BLOCK_END)

        (item_pos0, BRANCH)
        (item_pos0, BOOLEAN, "true")
        (CONSEQUENCE)
            (BLOCK_BEGIN)
            (item_pos1, NAME_DEF, "wind_force")
                (item_pos1, INTEGER, "13571")

            (item_pos2, BRANCH_ALTER_ONLY)
            (item_pos2, IDENTIFIER, "raven_claw")
            (ALTERNATIVE)
                (BLOCK_BEGIN)
                (item_pos0, RETURN)
                    (item_pos0, FLOATING, "0.000123")
                (BLOCK_END)

            (item_pos1, RETURN_NOTHING)
            (BLOCK_END)
        (ALTERNATIVE)
            (BLOCK_BEGIN)
            (item_pos2, BRANCH_ALTER_ONLY)
            (item_pos2, IDENTIFIER, "cliffkiller")
            (ALTERNATIVE)
                (BLOCK_BEGIN)
                (item_pos0, BRANCH_ALTER_ONLY)
                (item_pos0, IDENTIFIER, "skystrike")
                (ALTERNATIVE)
                    (BLOCK_BEGIN)
                    (BLOCK_END)
                (BLOCK_END)
            (BLOCK_END)
        (BLOCK_END)
    ;
}

TEST_F(ClauseBuilderTest, PushExprSequence)
{
    misc::position pos(5);

    grammar::ClauseBuilder builder;
    builder.addArith(0, pos, (new grammar::TokenSequence(
                new grammar::FactorToken(
                            pos, util::mkptr(new grammar::Identifier(pos, "sunako")), "sunako")))
          ->add(op(pos, "+"))
          ->add(new grammar::FactorToken(
                            pos, util::mkptr(new grammar::Identifier(pos, "kirisiki")), "kirisiki"))
          ->deliver());

    builder.buildAndClear().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
            (pos, ARITHMETICS)
                (pos, BINARY_OP, "+")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "sunako")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "kirisiki")
        (BLOCK_END)
    ;
}

TEST_F(ClauseBuilderTest, IfnotBranchErrorMatchElse)
{
    misc::position pos(6);
    misc::position else_pos(600);

    grammar::ClauseBuilder builder;
    builder.addIfnot(0, pos, (new grammar::TokenSequence(
              new grammar::FactorToken(
                        pos, util::mkptr(new grammar::Identifier(pos, "morikawa")), "morikawa")))
                                  ->deliver());
    builder.addArith(1, pos, (new grammar::TokenSequence(
              new grammar::FactorToken(
                        pos, util::mkptr(new grammar::Identifier(pos, "miho")), "miho")))
                                  ->deliver());
    builder.addArith(0, else_pos, (new grammar::TokenSequence(
                    new grammar::TypedToken(else_pos, "else", grammar::ELSE)))
                  ->deliver());
    builder.buildAndClear();

    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getElseNotMatchIfRecs().size());
    ASSERT_EQ(else_pos, getElseNotMatchIfRecs()[0].else_pos);
}

TEST_F(ClauseBuilderTest, ErrorElseNotMatched)
{
    misc::position pos(7);
    misc::position else_pos(700);

    grammar::ClauseBuilder builder;
    builder.addArith(0, pos, (new grammar::TokenSequence(
                      new grammar::TypedToken(pos, "if", grammar::IF)))
                ->add(new grammar::FactorToken(
                            pos, util::mkptr(new grammar::Identifier(pos, "asabina")), "mikuru"))
                ->deliver());
    builder.addArith(1, pos, (new grammar::TokenSequence(
                new grammar::FactorToken(
                            pos, util::mkptr(new grammar::Identifier(pos, "mikuru")), "mikuru")))
                                  ->deliver());
    builder.addArith(1, else_pos, (new grammar::TokenSequence(
                    new grammar::TypedToken(else_pos, "else", grammar::ELSE)))
                  ->deliver());
    builder.buildAndClear();

    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getElseNotMatchIfRecs().size());
    ASSERT_EQ(else_pos, getElseNotMatchIfRecs()[0].else_pos);
}

TEST_F(ClauseBuilderTest, UnterminatedExprInGlobalScope)
{
    misc::position pos(8);

    grammar::ClauseBuilder builder;
    builder.addArith(0, pos, (new grammar::TokenSequence(
              new grammar::FactorToken(
                          pos, util::mkptr(new grammar::IntLiteral(pos, "20121111")), "20121111")))
        ->add(op(pos, "+"))
        ->deliver());

    builder.buildAndClear();
    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getUnexpectedEofRecs().size());
}

TEST_F(ClauseBuilderTest, UnterminatedExprInBranch)
{
    misc::position pos(9);

    grammar::ClauseBuilder builder;
    builder.addArith(0, pos, (new grammar::TokenSequence(
                    new grammar::TypedToken(pos, "if", grammar::IF)))
              ->add(new grammar::FactorToken(
                            pos, util::mkptr(new grammar::BoolLiteral(pos, false)), "false"))
              ->deliver());
    builder.addArith(1, pos, (new grammar::TokenSequence(
                                    new grammar::TypedToken(pos, "(", grammar::OPEN_PAREN)))
                                  ->deliver());

    builder.buildAndClear();
    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getUnexpectedEofRecs().size());
}

TEST_F(ClauseBuilderTest, IfReducedElseNotMatched)
{
    misc::position item_pos0(10);
    misc::position item_pos1(1000);
    misc::position item_pos2(1001);

    grammar::ClauseBuilder builder;
    builder.addArith(0, item_pos0, (new grammar::TokenSequence(
                      new grammar::TypedToken(item_pos0, "if", grammar::IF)))
                ->add(new grammar::FactorToken(item_pos0, util::mkptr(
                                new grammar::BoolLiteral(item_pos0, true)), "true"))
                ->deliver());
    builder.addArith(0, item_pos1, (new grammar::TokenSequence(
                new grammar::FactorToken(item_pos1, util::mkptr(
                                new grammar::Identifier(item_pos1, "wind_force")), "wind_force")))
          ->add(new grammar::TypedToken(item_pos1, ":", grammar::COLON))
          ->add(new grammar::FactorToken(item_pos1, util::mkptr(
                                new grammar::Identifier(item_pos1, "13571")), "13571"))
          ->deliver());
    builder.addArith(0, item_pos2, (new grammar::TokenSequence(
                    new grammar::TypedToken(item_pos2, "else", grammar::ELSE)))
                  ->deliver());
    builder.buildAndClear();

    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getElseNotMatchIfRecs().size());
    ASSERT_EQ(item_pos2, getElseNotMatchIfRecs()[0].else_pos);
}

TEST_F(ClauseBuilderTest, RegularAsyncFunc)
{
    misc::position pos(11);

    grammar::ClauseBuilder builder;
    builder.addArith(0, pos, (new grammar::TokenSequence(
              new grammar::TypedToken(pos, "func", grammar::FUNC)))
        ->add(id(pos, "sakura"))
        ->add(open(pos, "("))
        ->add(id(pos, "xiaolang"))
        ->add(comma(pos))
        ->add(regularAsyncParam(pos))
        ->add(comma(pos))
        ->add(id(pos, "tukisiro"))
        ->add(close(pos, ")"))
        ->deliver());
    builder.addReturn(1, pos, (new grammar::TokenSequence(id(pos, "meiling")))->deliver());

    builder.buildAndClear().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
            (pos, REGULAR_ASYNC_PARAM_INDEX, 1)
            (pos, FUNC_DEF, "sakura")
                (pos, PARAMETER, "xiaolang")
                (pos, PARAMETER, "tukisiro")
                (BLOCK_BEGIN)
                    (pos, RETURN)
                        (pos, IDENTIFIER, "meiling")
                (BLOCK_END)
        (BLOCK_END)
    ;
}

TEST_F(ClauseBuilderTest, RegularAsyncParamOccurMoreThanOnce)
{
    misc::position pos(12);
    misc::position pos_a(1200);

    grammar::ClauseBuilder builder;
    builder.addArith(0, pos, (new grammar::TokenSequence(
              new grammar::TypedToken(pos, "func", grammar::FUNC)))
        ->add(id(pos, "sion"))
        ->add(open(pos, "("))
        ->add(id(pos, "mion"))
        ->add(comma(pos))
        ->add(regularAsyncParam(pos))
        ->add(comma(pos))
        ->add(regularAsyncParam(pos_a))
        ->add(close(pos, ")"))
        ->deliver());
    builder.addReturn(1, pos, (new grammar::TokenSequence(id(pos, "sonozaki")))->deliver());

    builder.buildAndClear();
    ASSERT_TRUE(error::hasError());

    ASSERT_EQ(1, getMoreThanOneAsyncPlaceholderRecs().size());
    ASSERT_EQ(pos_a, getMoreThanOneAsyncPlaceholderRecs()[0].pos);
}
