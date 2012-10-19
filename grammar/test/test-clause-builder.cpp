#include <gtest/gtest.h>

#include <flowcheck/expr-nodes.h>
#include <flowcheck/function.h>
#include <test/common.h>
#include <test/phony-errors.h>

#include "test-common.h"
#include "../clause-builder.h"
#include "../function.h"
#include "../stmt-nodes.h"

using namespace test;

typedef GrammarTest ClauseBuilderTest;

TEST_F(ClauseBuilderTest, AcceptorStackNext)
{
    misc::position item_pos(1);
    misc::position acc_pos(100);

    grammar::AcceptorStack stack0;
    stack0.nextStmt(0, util::mkptr(new grammar::NameDef(item_pos, "ruby", util::mkptr(
                                            new flchk::Reference(item_pos, "emerald")))));
    stack0.nextStmt(0, util::mkptr(new grammar::NameDef(item_pos, "topiz", util::mkptr(
                                            new flchk::Reference(item_pos, "ruby")))));
    stack0.nextFunc(0, util::mkptr(new grammar::Function(item_pos
                                                       , "skull"
                                                       , std::vector<std::string>({ "chipped" })
                                                       , grammar::Block())));
    ASSERT_FALSE(error::hasError());
    grammar::Block block0(stack0.packAll());
    block0.compile(mkfilter())->compile();

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (item_pos, FUNC_DEF, "skull")
            (item_pos, PARAMETER, "chipped")
            (BLOCK_BEGIN)
            (BLOCK_END)

        (item_pos, NAME_DEF, "ruby")
            (item_pos, IDENTIFIER, "emerald")
        (item_pos, NAME_DEF, "topiz")
            (item_pos, IDENTIFIER, "ruby")
        (BLOCK_END)
    ;

    misc::position err_pos0(101);
    misc::position err_pos1(102);
    ASSERT_FALSE(error::hasError());
    stack0.nextStmt(1, util::mkptr(new grammar::Arithmetics(err_pos0, util::mkptr(
                                            new flchk::IntLiteral(item_pos, "20110119")))));
    stack0.nextStmt(1, util::mkptr(new grammar::Arithmetics(item_pos, util::mkptr(
                                            new flchk::FloatLiteral(item_pos, "19.55")))));
    stack0.nextFunc(4, util::mkptr(new grammar::Function(err_pos1
                                                       , "ith"
                                                       , std::vector<std::string>({ "el", "eth" })
                                                       , grammar::Block())));
    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(2, getExcessInds().size());
    ASSERT_EQ(err_pos0, getExcessInds()[0].pos);
    ASSERT_EQ(err_pos1, getExcessInds()[1].pos);
}

TEST_F(ClauseBuilderTest, AcceptorStackAdd)
{
    misc::position item_pos(2);
    misc::position acc_pos(100);

    grammar::AcceptorStack stack0;
    stack0.nextStmt(0, util::mkptr(new grammar::Arithmetics(item_pos, util::mkptr(
                                            new flchk::Reference(item_pos, "eaglehorn")))));
    stack0.add(0, util::mkptr(new grammar::FunctionAcceptor(acc_pos
                                                          , "witherstring"
                                                          , std::vector<std::string>())));
    stack0.nextStmt(1, util::mkptr(new grammar::NameDef(item_pos, "cedar_bow", util::mkptr(
                                            new flchk::Reference(item_pos, "kuko_shakaku")))));
    stack0.nextStmt(1, util::mkptr(new grammar::Arithmetics(item_pos, util::mkptr(
                                          new flchk::Reference(item_pos, "buriza_do_kyanon")))));
    ASSERT_FALSE(error::hasError());
    grammar::Block block0(stack0.packAll());
    block0.compile(mkfilter())->compile();

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (acc_pos, FUNC_DEF, "witherstring")
            (BLOCK_BEGIN)
            (item_pos, NAME_DEF, "cedar_bow")
                (item_pos, IDENTIFIER, "kuko_shakaku")
            (item_pos, ARITHMETICS)
                (item_pos, IDENTIFIER, "buriza_do_kyanon")
            (BLOCK_END)

        (item_pos, ARITHMETICS)
            (item_pos, IDENTIFIER, "eaglehorn")
        (BLOCK_END)
    ;

    grammar::AcceptorStack stack1;
    stack1.add(0, util::mkptr(new grammar::FunctionAcceptor(acc_pos
                                                          , "witherstring"
                                                          , std::vector<std::string>())));
    stack1.nextStmt(1, util::mkptr(new grammar::NameDef(item_pos, "cedar_bow", util::mkptr(
                                            new flchk::Reference(item_pos, "kuko_shakaku")))));
    stack1.nextStmt(0, util::mkptr(new grammar::Arithmetics(item_pos, util::mkptr(
                                            new flchk::Reference(item_pos, "eaglehorn")))));
    stack1.nextStmt(1, util::mkptr(new grammar::Arithmetics(item_pos, util::mkptr(
                                            new flchk::Reference(item_pos, "buriza_do_kyanon")))));
    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getExcessInds().size());
    ASSERT_EQ(item_pos, getExcessInds()[0].pos);
}

TEST_F(ClauseBuilderTest, AcceptorStackMatchElse)
{
    misc::position item_pos(3);
    misc::position acc_pos(210);
    misc::position else_pos(211);

    grammar::AcceptorStack stack0;
    stack0.nextStmt(0, util::mkptr(new grammar::Arithmetics(item_pos, util::mkptr(
                                        new flchk::Reference(item_pos, "roguesbow")))));
    stack0.add(0, util::mkptr(new grammar::IfAcceptor(acc_pos, util::mkptr(
                                        new flchk::Reference(item_pos, "stormstrike")))));
    stack0.nextStmt(1, util::mkptr(new grammar::Arithmetics(item_pos, util::mkptr(
                                        new flchk::Reference(item_pos, "witchwild_string")))));
    stack0.matchElse(0, else_pos);
    stack0.nextStmt(1, util::mkptr(new grammar::Arithmetics(item_pos, util::mkptr(
                                        new flchk::Reference(item_pos, "magewrath")))));
    ASSERT_FALSE(error::hasError());
    grammar::Block block0(stack0.packAll());
    block0.compile(mkfilter())->compile();

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (item_pos, ARITHMETICS)
            (item_pos, IDENTIFIER, "roguesbow")
        (acc_pos, BRANCH)
        (item_pos, IDENTIFIER, "stormstrike")
        (CONSEQUENCE)
            (BLOCK_BEGIN)
            (item_pos, ARITHMETICS)
                (item_pos, IDENTIFIER, "witchwild_string")
            (BLOCK_END)
        (ALTERNATIVE)
            (BLOCK_BEGIN)
            (item_pos, ARITHMETICS)
                (item_pos, IDENTIFIER, "magewrath")
            (BLOCK_END)
        (BLOCK_END)
    ;

    grammar::AcceptorStack stack1;
    stack1.nextStmt(0, util::mkptr(new grammar::Arithmetics(item_pos, util::mkptr(
                                            new flchk::Reference(item_pos, "roguesbow")))));
    stack1.add(0, util::mkptr(new grammar::IfAcceptor(acc_pos, util::mkptr(
                                            new flchk::Reference(item_pos, "stormstrike")))));
    stack1.nextStmt(1, util::mkptr(new grammar::Arithmetics(item_pos, util::mkptr(
                                            new flchk::Reference(item_pos, "witchwild_string")))));
    stack1.matchElse(2, else_pos);
    stack1.nextStmt(1, util::mkptr(new grammar::Arithmetics(item_pos, util::mkptr(
                                            new flchk::Reference(item_pos, "magewrath")))));
    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getElseNotMatches().size());
    ASSERT_EQ(else_pos, getElseNotMatches()[0].pos);
}

TEST_F(ClauseBuilderTest, ClauseBuilder)
{
    misc::position item_pos0(4);
    misc::position item_pos1(5);
    misc::position item_pos2(6);

    grammar::ClauseBuilder builder0;
    builder0.addIf(0, util::mkptr(new flchk::BoolLiteral(item_pos0, true)));
        builder0.addNameDef(1, "wind_force", util::mkptr(
                                    new flchk::IntLiteral(item_pos1, "13571")));
        builder0.addIfnot(1, util::mkptr(new flchk::Reference(item_pos2, "raven_claw")));
            builder0.addReturn(2, util::mkptr(new flchk::FloatLiteral(item_pos0, "0.000123")));
        builder0.addReturnNothing(1, item_pos1);
    builder0.addElse(0, item_pos2);
        builder0.addIfnot(1, util::mkptr(new flchk::Reference(item_pos2, "cliffkiller")));
            builder0.addIfnot(2, util::mkptr(new flchk::Reference(item_pos0, "skystrike")));
    builder0.addFunction(0
                       , item_pos1
                       , "goldenstrike_arch"
                       , std::vector<std::string>({ "amn", "tir" }));
        builder0.addArith(1, util::mkptr(new flchk::Reference(item_pos1, "widowmaker")));

    builder0.buildAndClear()->compile();
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
            (item_pos1, NAME_DEF, "wind_force" + NAME_DEF_FILTERED)
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

    grammar::ClauseBuilder builder1;
    builder1.addIf(0, util::mkptr(new flchk::BoolLiteral(item_pos0, true)));
    builder1.addNameDef(0, "wind_force", util::mkptr(new flchk::IntLiteral(item_pos1, "13571")));
    builder1.addElse(0, item_pos2);
    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getElseNotMatches().size());
    ASSERT_EQ(item_pos2, getElseNotMatches()[0].pos);
}
