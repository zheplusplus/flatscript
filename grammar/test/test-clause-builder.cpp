#include <gtest/gtest.h>

#include <test/phony-errors.h>

#include "test-common.h"
#include "../clause-builder.h"
#include "../function.h"
#include "../stmt-nodes.h"
#include "../expr-nodes.h"
#include "../expr-tokens.h"
#include "../syntax-types.h"

using namespace test;

typedef GrammarTest ClauseBuilderTest;

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
    builder.addIf(0, pos, (new grammar::TokenSequence(
              new grammar::FactorToken(
                        pos, util::mkptr(new grammar::Identifier(pos, "kaeri")), "kaeri")))
        ->add(new grammar::OpToken(pos, "+"))
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
    builder.addIf(0, pos, (new grammar::TokenSequence(
              new grammar::FactorToken(
                            pos, util::mkptr(new grammar::Identifier(pos, "yes")), "yes")))
                                  ->deliver());
    builder.addArith(1, pos, (new grammar::TokenSequence(
              new grammar::FactorToken(
                            pos, util::mkptr(new grammar::Identifier(pos, "i")), "i")))
                                  ->deliver());
    builder.addElse(0, else_pos_a);
    builder.addArith(1, pos, (new grammar::TokenSequence(
              new grammar::FactorToken(
                            pos, util::mkptr(new grammar::Identifier(pos, "will")), "will")))
                                  ->deliver());
    builder.addElse(0, else_pos_b);
    builder.addArith(1, pos, (new grammar::TokenSequence(
              new grammar::FactorToken(
                            pos, util::mkptr(new grammar::Identifier(pos, "nadia")), "nadia")))
                                  ->deliver());

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
    builder0.addIf(0, item_pos0, (new grammar::TokenSequence(
            new grammar::FactorToken(
                      item_pos0, util::mkptr(new grammar::BoolLiteral(item_pos0, true)), "true")))
                                      ->deliver());
        builder0.addArith(1, item_pos1, (new grammar::TokenSequence(
                new grammar::FactorToken(item_pos1, util::mkptr(
                                new grammar::Identifier(item_pos1, "wind_force")), "wind_force")))
          ->add(new grammar::ColonToken(item_pos1))
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
    builder0.addElse(0, item_pos2);
        builder0.addIfnot(1, item_pos2, (new grammar::TokenSequence(
                new grammar::FactorToken(item_pos2, util::mkptr(
                                new grammar::Identifier(item_pos2, "cliffkiller")), "cliffkiller")))
                                          ->deliver());
            builder0.addIfnot(2, item_pos0, (new grammar::TokenSequence(
                new grammar::FactorToken(item_pos0, util::mkptr(
                                new grammar::Identifier(item_pos0, "skystrike")), "skystrike")))
                                              ->deliver());
    builder0.addFunction(0
                       , item_pos1
                       , "goldenstrike_arch"
                       , std::vector<std::string>({ "amn", "tir" }));
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
          ->add(new grammar::OpToken(pos, "+"))
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
    builder.addElse(0, else_pos);

    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getElseNotMatchIfRecs().size());
    ASSERT_EQ(else_pos, getElseNotMatchIfRecs()[0].else_pos);
}

TEST_F(ClauseBuilderTest, ErrorElseNotMatched)
{
    misc::position pos(7);
    misc::position else_pos(700);

    grammar::ClauseBuilder builder;
    builder.addIf(0, pos, (new grammar::TokenSequence(
                new grammar::FactorToken(
                            pos, util::mkptr(new grammar::Identifier(pos, "asabina")), "mikuru")))
                                  ->deliver());
    builder.addArith(1, pos, (new grammar::TokenSequence(
                new grammar::FactorToken(
                            pos, util::mkptr(new grammar::Identifier(pos, "mikuru")), "mikuru")))
                                  ->deliver());
    builder.addElse(1, else_pos);

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
        ->add(new grammar::OpToken(pos, "+"))
        ->deliver());

    builder.buildAndClear();
    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getUnexpectedEofRecs().size());
}

TEST_F(ClauseBuilderTest, UnterminatedExprInBranch)
{
    misc::position pos(9);

    grammar::ClauseBuilder builder;
    builder.addIf(0, pos, (new grammar::TokenSequence(
              new grammar::FactorToken(
                            pos, util::mkptr(new grammar::BoolLiteral(pos, false)), "false")))
                                  ->deliver());
    builder.addArith(1, pos, (new grammar::TokenSequence(
                                        new grammar::OpenParenToken(pos)))
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

    grammar::ClauseBuilder builder1;
    builder1.addIf(0, item_pos0, (new grammar::TokenSequence(
                new grammar::FactorToken(item_pos0, util::mkptr(
                                new grammar::BoolLiteral(item_pos0, true)), "true")))
                                      ->deliver());
    builder1.addArith(0, item_pos1, (new grammar::TokenSequence(
                new grammar::FactorToken(item_pos1, util::mkptr(
                                new grammar::Identifier(item_pos1, "wind_force")), "wind_force")))
          ->add(new grammar::ColonToken(item_pos1))
          ->add(new grammar::FactorToken(item_pos1, util::mkptr(
                                new grammar::Identifier(item_pos1, "13571")), "13571"))
          ->deliver());
    builder1.addElse(0, item_pos2);
    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getElseNotMatchIfRecs().size());
    ASSERT_EQ(item_pos2, getElseNotMatchIfRecs()[0].else_pos);
}
