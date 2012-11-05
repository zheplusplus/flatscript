#include <vector>
#include <gtest/gtest.h>

#include <output/node-base.h>
#include <output/function.h>
#include <test/phony-errors.h>
#include <test/common.h>

#include "test-common.h"
#include "../expr-nodes.h"
#include "../global-filter.h"
#include "../func-body-filter.h"
#include "../function.h"
#include "../symbol-table.h"

using namespace test;

typedef SemanticTest ExprNodesTest;

TEST_F(ExprNodesTest, SimpleLiterals)
{
    misc::position pos(1);
    semantic::SymbolTable st;
    semantic::IntLiteral int0(pos, "20110116");
    int0.compile(util::mkref(st))->str(false);
    EXPECT_TRUE(int0.isLiteral(util::mkref(st)));

    semantic::FloatLiteral float0(pos, "19.50");
    float0.compile(util::mkref(st))->str(false);
    EXPECT_TRUE(float0.isLiteral(util::mkref(st)));

    semantic::BoolLiteral bool0(pos, true);
    bool0.compile(util::mkref(st))->str(false);
    EXPECT_TRUE(bool0.isLiteral(util::mkref(st)));
    EXPECT_TRUE(bool0.boolValue(util::mkref(st)));

    semantic::IntLiteral int1(pos, "441499");
    int1.compile(util::mkref(st))->str(false);
    EXPECT_TRUE(int1.isLiteral(util::mkref(st)));

    semantic::FloatLiteral float1(pos, "0.1950");
    float1.compile(util::mkref(st))->str(false);
    EXPECT_TRUE(float1.isLiteral(util::mkref(st)));

    semantic::BoolLiteral bool1(pos, false);
    bool1.compile(util::mkref(st))->str(false);
    EXPECT_TRUE(bool1.isLiteral(util::mkref(st)));
    EXPECT_FALSE(bool1.boolValue(util::mkref(st)));

    semantic::StringLiteral str(pos, "");
    str.compile(util::mkref(st))->str(false);
    EXPECT_TRUE(str.isLiteral(util::mkref(st)));
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
    semantic::SymbolTable st;

    std::vector<util::sptr<semantic::Expression const>> members;
    members.push_back(util::mkptr(new semantic::IntLiteral(pos, "20110814")));
    members.push_back(util::mkptr(new semantic::ListLiteral(
                                    pos, std::vector<util::sptr<semantic::Expression const>>())));
    members.push_back(util::mkptr(new semantic::FloatLiteral(pos, "20.54")));

    semantic::ListLiteral ls(pos, std::move(members));
    ls.compile(util::mkref(st))->str(false);
    EXPECT_FALSE(ls.isLiteral(util::mkref(st)));

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
    semantic::SymbolTable st;
    st.defName(pos, "a20110116");
    st.defName(pos, "b1950");

    semantic::Reference ref0(pos, "a20110116");
    EXPECT_FALSE(ref0.isLiteral(util::mkref(st)));
    ref0.compile(util::mkref(st))->str(false);

    semantic::Reference ref1(pos, "b1950");
    EXPECT_FALSE(ref0.isLiteral(util::mkref(st)));
    ref1.compile(util::mkref(st))->str(false);

    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (pos, REFERENCE, "a20110116")
        (pos, REFERENCE, "b1950")
    ;
}

TEST_F(ExprNodesTest, Calls)
{
    misc::position pos(5);
    misc::position pos_d(300);
    semantic::SymbolTable st;
    st.defName(pos, "darekatasukete");
    st.defName(pos, "leap");
    st.defName(pos_d, "fib");

    std::vector<util::sptr<semantic::Expression const>> args;
    semantic::Call call0(pos, util::mkptr(new semantic::Reference(pos, "fib")), std::move(args));

    args.push_back(util::mkptr(new semantic::BoolLiteral(pos, false)));
    args.push_back(util::mkptr(new semantic::PreUnaryOp(pos, "-", util::mkptr(
                                            new semantic::FloatLiteral(pos, "11.11")))));
    args.push_back(util::mkptr(new semantic::PreUnaryOp(pos, "!", util::mkptr(
                                            new semantic::BoolLiteral(pos, true)))));
    args.push_back(util::mkptr(new semantic::Reference(pos, "darekatasukete")));
    semantic::Call call1(pos, util::mkptr(new semantic::Reference(pos, "leap")), std::move(args));

    call0.compile(util::mkref(st))->str(false);
    EXPECT_FALSE(call0.isLiteral(util::mkref(st)));

    call1.compile(util::mkref(st))->str(false);
    EXPECT_FALSE(call1.isLiteral(util::mkref(st)));

    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (pos, CALL, 0)
            (pos, REFERENCE, "fib")
        (pos, CALL, 4)
            (pos, REFERENCE, "leap")
            (pos, BOOLEAN, "false")
            (pos, FLOATING, "-11.11")
            (pos, BOOLEAN, "false")
            (pos, REFERENCE, "darekatasukete")
    ;
}

TEST_F(ExprNodesTest, FoldBinaryOp)
{
    misc::position pos(6);
    semantic::SymbolTable st;
    semantic::BinaryOp bin_a(pos
                           , util::mkptr(new semantic::IntLiteral(pos, "20111"))
                           , "-"
                           , util::mkptr(new semantic::FloatLiteral(pos, "20110.4")));
    semantic::BinaryOp bin_b(pos
                           , util::mkptr(new semantic::StringLiteral(pos, "nov 8th"))
                           , "+"
                           , util::mkptr(new semantic::StringLiteral(pos, ", 2012")));
    EXPECT_TRUE(bin_a.isLiteral(util::mkref(st)));
    bin_a.compile(util::mkref(st))->str(false);

    EXPECT_TRUE(bin_b.isLiteral(util::mkref(st)));
    bin_b.compile(util::mkref(st))->str(false);

    ASSERT_FALSE(error::hasError());
    DataTree::expectOne()
        (pos, FLOATING, "0.6")
        (pos, STRING, "nov 8th, 2012")
    ;
}

TEST_F(ExprNodesTest, LiteralBoolValueError)
{
    misc::position pos(7);
    semantic::SymbolTable st;
    semantic::IntLiteral int0(pos, "20110409");
    int0.boolValue(util::mkref(st));

    semantic::FloatLiteral float0(pos, "10.58");
    float0.boolValue(util::mkref(st));

    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(2, getCondNotBoolRecs().size());
    EXPECT_EQ(pos, getCondNotBoolRecs()[0].pos);
    EXPECT_EQ("int", getCondNotBoolRecs()[0].actual_type);
    EXPECT_EQ(pos, getCondNotBoolRecs()[1].pos);
    EXPECT_EQ("float", getCondNotBoolRecs()[1].actual_type);
}

TEST_F(ExprNodesTest, OperationLiteralBoolValueError)
{
    misc::position pos(8);
    semantic::SymbolTable st;
    semantic::BinaryOp conj(pos
                          , util::mkptr(new semantic::BoolLiteral(pos, true))
                          , "&&"
                          , util::mkptr(new semantic::FloatLiteral(pos, "20110.4")));
    conj.boolValue(util::mkref(st));
    semantic::BinaryOp disj(pos
                          , util::mkptr(new semantic::BoolLiteral(pos, false))
                          , "||"
                          , util::mkptr(new semantic::IntLiteral(pos, "2")));
    disj.boolValue(util::mkref(st));
    semantic::PreUnaryOp nega(pos, "!", util::mkptr(new semantic::FloatLiteral(pos, "1.12")));
    nega.boolValue(util::mkref(st));

    semantic::BinaryOp binary(pos
                            , util::mkptr(new semantic::IntLiteral(pos, "1"))
                            , "*"
                            , util::mkptr(new semantic::FloatLiteral(pos, "11235.8")));
    binary.boolValue(util::mkref(st));

    semantic::PreUnaryOp pre_unary(pos, "+", util::mkptr(new semantic::FloatLiteral(pos, ".13")));
    pre_unary.boolValue(util::mkref(st));

    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(5, getCondNotBoolRecs().size());
    EXPECT_EQ(pos, getCondNotBoolRecs()[0].pos);
    EXPECT_EQ("float", getCondNotBoolRecs()[0].actual_type);
    EXPECT_EQ(pos, getCondNotBoolRecs()[1].pos);
    EXPECT_EQ("int", getCondNotBoolRecs()[1].actual_type);
    EXPECT_EQ(pos, getCondNotBoolRecs()[2].pos);
    EXPECT_EQ("float", getCondNotBoolRecs()[2].actual_type);
    EXPECT_EQ(pos, getCondNotBoolRecs()[3].pos);
    EXPECT_EQ("float", getCondNotBoolRecs()[3].actual_type);
    EXPECT_EQ(pos, getCondNotBoolRecs()[4].pos);
    EXPECT_EQ("float", getCondNotBoolRecs()[4].actual_type);
}

TEST_F(ExprNodesTest, ListAppending)
{
    misc::position pos(8);
    semantic::SymbolTable st;
    st.defName(pos, "chiaki");
    st.defName(pos, "douma");

    semantic::ListAppend lsa(pos
                           , util::mkptr(new semantic::Reference(pos, "chiaki"))
                           , util::mkptr(new semantic::Reference(pos, "douma")));

    lsa.compile(util::mkref(st))->str(false);
    ASSERT_FALSE(error::hasError());

    lsa.compile(util::mkref(st))->str(false);

    DataTree::expectOne()
        (pos, BINARY_OP, "++")
            (pos, REFERENCE, "chiaki")
            (pos, REFERENCE, "douma")
        (pos, BINARY_OP, "++")
            (pos, REFERENCE, "chiaki")
            (pos, REFERENCE, "douma")
    ;
}

TEST_F(ExprNodesTest, FoldDiv0)
{
    misc::position pos(9);
    misc::position pos_a(900);
    misc::position pos_b(901);
    semantic::SymbolTable st;

    semantic::BinaryOp bin_a(pos_a
                           , util::mkptr(new semantic::IntLiteral(pos, "20121112"))
                           , "%"
                           , util::mkptr(new semantic::IntLiteral(pos, "0")));
    EXPECT_TRUE(bin_a.isLiteral(util::mkref(st)));
    bin_a.compile(util::mkref(st));

    semantic::BinaryOp bin_b(pos_b
                           , util::mkptr(new semantic::IntLiteral(pos, "20121112"))
                           , "/"
                           , util::mkptr(new semantic::FloatLiteral(pos, ".0")));
    EXPECT_TRUE(bin_b.isLiteral(util::mkref(st)));
    bin_b.compile(util::mkref(st));

    EXPECT_TRUE(error::hasError());

    std::vector<DivZeroRec> recs(getDivZeroRecs());
    ASSERT_EQ(2, recs.size());
    ASSERT_EQ(pos_a, recs[0].pos);
    ASSERT_EQ(pos_b, recs[1].pos);
}
