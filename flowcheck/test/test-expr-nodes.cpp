#include <vector>
#include <gtest/gtest.h>

#include <proto/node-base.h>
#include <proto/function.h>
#include <test/phony-errors.h>
#include <test/common.h>

#include "test-common.h"
#include "../expr-nodes.h"
#include "../global-filter.h"
#include "../func-body-filter.h"
#include "../function.h"
#include "../symbol-table.h"

using namespace test;

typedef FlowcheckTest ExprNodesTest;

TEST_F(ExprNodesTest, SimpleLiterals)
{
    misc::position pos(1);
    util::sptr<proto::Block> block(new proto::Block);
    flchk::SymbolTable st;
    flchk::IntLiteral int0(pos, "20110116");
    int0.compile(util::mkref(st))->write();
    EXPECT_TRUE(int0.isLiteral());

    flchk::FloatLiteral float0(pos, "19.50");
    float0.compile(util::mkref(st))->write();
    EXPECT_TRUE(float0.isLiteral());

    flchk::BoolLiteral bool0(pos, true);
    bool0.compile(util::mkref(st))->write();
    EXPECT_TRUE(bool0.isLiteral());
    EXPECT_TRUE(bool0.boolValue());

    flchk::IntLiteral int1(pos, "441499");
    int1.compile(util::mkref(st))->write();
    EXPECT_TRUE(int1.isLiteral());

    flchk::FloatLiteral float1(pos, "0.1950");
    float1.compile(util::mkref(st))->write();
    EXPECT_TRUE(float1.isLiteral());

    flchk::BoolLiteral bool1(pos, false);
    bool1.compile(util::mkref(st))->write();
    EXPECT_TRUE(bool1.isLiteral());
    EXPECT_FALSE(bool1.boolValue());

    flchk::StringLiteral str(pos, "");
    str.compile(util::mkref(st))->write();
    EXPECT_FALSE(str.isLiteral());
    EXPECT_FALSE(str.boolValue());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (pos, INTEGER, "20110116")
        (pos, FLOATING, "19.5")
        (pos, BOOLEAN, "true")
        (pos, INTEGER, "441499")
        (pos, FLOATING, "0.195")
        (pos, BOOLEAN, "false")
        (pos, STRING, "")
    ;
}

TEST_F(ExprNodesTest, ListLiterals)
{
    misc::position pos(2);
    util::sptr<proto::Block> block(new proto::Block);
    flchk::SymbolTable st;

    std::vector<util::sptr<flchk::Expression const>> members;
    members.push_back(util::mkptr(new flchk::IntLiteral(pos, "20110814")));
    members.push_back(util::mkptr(
                  new flchk::ListLiteral(pos, std::vector<util::sptr<flchk::Expression const>>())));
    members.push_back(util::mkptr(new flchk::FloatLiteral(pos, "20.54")));

    flchk::ListLiteral ls(pos, std::move(members));
    ls.compile(util::mkref(st))->write();
    EXPECT_FALSE(ls.isLiteral());

    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (pos, LIST, 3)
            (pos, INTEGER, "20110814")
            (pos, LIST, 0)
            (pos, FLOATING, "20.54")
    ;
}

TEST_F(ExprNodesTest, Reference)
{
    misc::position pos(3);
    util::sptr<proto::Block> block(new proto::Block);
    flchk::GlobalFilter filter;
    filter.getSymbols()->defVar(pos, "a20110116");
    filter.getSymbols()->defVar(pos, "b1950");

    flchk::Reference ref0(pos, "a20110116");
    EXPECT_FALSE(ref0.isLiteral());
    ref0.compile(filter.getSymbols())->write();

    flchk::Reference ref1(pos, "b1950");
    EXPECT_FALSE(ref0.isLiteral());
    ref1.compile(filter.getSymbols())->write();

    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (pos, REFERENCE, "a20110116")
        (pos, REFERENCE, "b1950")
    ;
}

TEST_F(ExprNodesTest, Operations)
{
    misc::position pos(4);
    util::sptr<proto::Block> block(new proto::Block);
    flchk::GlobalFilter filter;
    filter.getSymbols()->defVar(pos, "wasureru");
    filter.getSymbols()->defVar(pos, "koto");

    flchk::BinaryOp binary0(pos
                          , std::move(util::mkptr(new flchk::IntLiteral(pos, "1")))
                          , "+"
                          , std::move(util::mkptr(new flchk::FloatLiteral(pos, "11235.8"))));
    flchk::BinaryOp binary1(pos
                          , std::move(util::mkptr(new flchk::FloatLiteral(pos, "1.12358")))
                          , "<="
                          , std::move(util::mkptr(new flchk::IntLiteral(pos, "2357111317"))));

    flchk::PreUnaryOp pre_unary0(pos, "+", util::mkptr(new flchk::FloatLiteral(pos, ".13")));
    flchk::PreUnaryOp pre_unary1(
            pos
          , "-"
          , util::mkptr(new flchk::BinaryOp(pos
                                          , util::mkptr(new flchk::Reference(pos, "wasureru"))
                                          , "%"
                                          , util::mkptr(new flchk::IntLiteral(pos, "1")))));

    flchk::Conjunction conj(pos
                          , util::mkptr(new flchk::BoolLiteral(pos, true))
                          , util::mkptr(new flchk::Reference(pos, "koto")));
    flchk::Disjunction disj(pos
                          , util::mkptr(new flchk::BoolLiteral(pos, false))
                          , util::mkptr(new flchk::IntLiteral(pos, "2")));
    flchk::Negation nega(pos, util::mkptr(new flchk::FloatLiteral(pos, "1954.01")));

    binary0.compile(filter.getSymbols())->write();
    EXPECT_TRUE(binary0.isLiteral());

    binary1.compile(filter.getSymbols())->write();
    EXPECT_TRUE(binary1.isLiteral());
    EXPECT_TRUE(binary1.boolValue());

    pre_unary0.compile(filter.getSymbols())->write();
    EXPECT_TRUE(pre_unary0.isLiteral());

    pre_unary1.compile(filter.getSymbols())->write();
    EXPECT_FALSE(pre_unary1.isLiteral());

    conj.compile(filter.getSymbols())->write();
    EXPECT_FALSE(conj.isLiteral());

    disj.compile(filter.getSymbols())->write();
    EXPECT_TRUE(disj.isLiteral());

    nega.compile(filter.getSymbols())->write();
    EXPECT_TRUE(nega.isLiteral());

    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (pos, BINARY_OP, "+")
            (pos, INTEGER, "1")
            (pos, FLOATING, "11235.8")
        (pos, BINARY_OP, "<=")
            (pos, FLOATING, "1.12358")
            (pos, INTEGER, "2357111317")
        (pos, PRE_UNARY_OP, "+")
            (pos, FLOATING, "0.13")
        (pos, PRE_UNARY_OP, "-")
            (pos, BINARY_OP, "%")
                (pos, REFERENCE, "wasureru")
                (pos, INTEGER, "1")
        (pos, BINARY_OP, "&&")
            (pos, BOOLEAN, "true")
            (pos, REFERENCE, "koto")
        (pos, BINARY_OP, "||")
            (pos, BOOLEAN, "false")
            (pos, INTEGER, "2")
        (pos, PRE_UNARY_OP, "!")
            (pos, FLOATING, "1954.01")
    ;
}

TEST_F(ExprNodesTest, Calls)
{
    misc::position pos(5);
    misc::position pos_d(300);
    util::sptr<proto::Block> block(new proto::Block);
    flchk::GlobalFilter filter;
    filter.getSymbols()->defVar(pos, "darekatasukete");
    filter.getSymbols()->defVar(pos, "leap");

    filter.defFunc(pos_d, "fib", std::vector<std::string>(), util::mkptr(new flchk::FuncBodyFilter(
                    pos, filter.getSymbols(), std::vector<std::string>())));

    std::vector<util::sptr<flchk::Expression const>> args;
    flchk::Call Call0(pos, "fib", std::move(args));

    args.push_back(util::mkptr(new flchk::BoolLiteral(pos, false)));
    args.push_back(util::mkptr(new flchk::PreUnaryOp(pos, "-", util::mkptr(
                                            new flchk::FloatLiteral(pos, "11.11")))));
    args.push_back(util::mkptr(new flchk::Negation(pos, util::mkptr(
                                            new flchk::IntLiteral(pos, "21")))));
    args.push_back(util::mkptr(new flchk::Reference(pos, "darekatasukete")));
    flchk::Call Call1(pos, "leap", std::move(args));

    Call0.compile(filter.getSymbols())->write();
    EXPECT_FALSE(Call0.isLiteral());

    Call1.compile(filter.getSymbols())->write();
    EXPECT_FALSE(Call1.isLiteral());

    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (pos, CALL, "fib", 0)
        (pos, CALL, "leap", 4)
            (pos, BOOLEAN, "false")
            (pos, PRE_UNARY_OP, "-")
                (pos, FLOATING, "11.11")
            (pos, PRE_UNARY_OP, "!")
                (pos, INTEGER, "21")
            (pos, REFERENCE, "darekatasukete")
    ;
}

TEST_F(ExprNodesTest, LiteralBoolValueError)
{
    misc::position pos(7);
    flchk::IntLiteral int0(pos, "20110409");
    int0.boolValue();

    flchk::FloatLiteral float0(pos, "10.58");
    float0.boolValue();

    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(2, getCondNotBools().size());
    EXPECT_EQ(pos, getCondNotBools()[0].pos);
    EXPECT_EQ("(int(20110409))", getCondNotBools()[0].type_name);
    EXPECT_EQ(pos, getCondNotBools()[1].pos);
    EXPECT_EQ("(float(10.58))", getCondNotBools()[1].type_name);
}

TEST_F(ExprNodesTest, OperationLiteralBoolValueError)
{
    misc::position pos(8);
    flchk::Conjunction conj(pos
                          , util::mkptr(new flchk::BoolLiteral(pos, true))
                          , util::mkptr(new flchk::FloatLiteral(pos, "20110.4")));
    conj.boolValue();
    flchk::Disjunction disj(pos
                          , util::mkptr(new flchk::BoolLiteral(pos, false))
                          , util::mkptr(new flchk::IntLiteral(pos, "2")));
    disj.boolValue();
    flchk::Negation nega(pos, util::mkptr(new flchk::FloatLiteral(pos, "1.12")));
    nega.boolValue();

    flchk::BinaryOp binary(pos
                         , util::mkptr(new flchk::IntLiteral(pos, "1"))
                         , "*"
                         , util::mkptr(new flchk::FloatLiteral(pos, "11235.8")));
    binary.boolValue();

    flchk::PreUnaryOp pre_unary(pos, "+", util::mkptr(new flchk::FloatLiteral(pos, ".13")));
    pre_unary.boolValue();

    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(5, getCondNotBools().size());
    EXPECT_EQ(pos, getCondNotBools()[0].pos);
    EXPECT_EQ("(float(20110.4))", getCondNotBools()[0].type_name);
    EXPECT_EQ(pos, getCondNotBools()[1].pos);
    EXPECT_EQ("(int(2))", getCondNotBools()[1].type_name);
    EXPECT_EQ(pos, getCondNotBools()[2].pos);
    EXPECT_EQ("(float(1.12))", getCondNotBools()[2].type_name);
    EXPECT_EQ(pos, getCondNotBools()[3].pos);
    EXPECT_EQ("((int(1))*(float(11235.8)))", getCondNotBools()[3].type_name);
    EXPECT_EQ(pos, getCondNotBools()[4].pos);
    EXPECT_EQ("(+(float(0.13)))", getCondNotBools()[4].type_name);
}

TEST_F(ExprNodesTest, ListAppending)
{
    misc::position pos(8);
    util::sptr<proto::Block> block(new proto::Block);
    flchk::GlobalFilter filter;
    filter.getSymbols()->defVar(pos, "chiaki");
    filter.getSymbols()->defVar(pos, "douma");

    flchk::ListAppend lsa(pos
                        , util::mkptr(new flchk::Reference(pos, "chiaki"))
                        , util::mkptr(new flchk::Reference(pos, "douma")));

    lsa.compile(filter.getSymbols())->write();
    ASSERT_FALSE(error::hasError());

    lsa.fold()->compile(filter.getSymbols())->write();

    DataTree::expectOne()
        (pos, BINARY_OP, "++")
            (pos, REFERENCE, "chiaki")
            (pos, REFERENCE, "douma")
        (pos, BINARY_OP, "++")
            (pos, REFERENCE, "chiaki")
            (pos, REFERENCE, "douma")
    ;
}
