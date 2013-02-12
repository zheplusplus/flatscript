#include <gtest/gtest.h>

#include <util/string.h>
#include <output/node-base.h>
#include <test/phony-errors.h>
#include <test/common.h>

#include "test-common.h"
#include "../expr-nodes.h"
#include "../list-pipe.h"
#include "../compiling-space.h"
#include "../filter.h"

using namespace test;

typedef SemanticTest ExprNodesTest;

TEST_F(ExprNodesTest, SimpleLiterals)
{
    misc::position pos(1);
    semantic::CompilingSpace space;
    semantic::IntLiteral int0(pos, "20110116");
    int0.compile(space)->str();
    EXPECT_TRUE(int0.isLiteral(space.sym()));

    semantic::FloatLiteral float0(pos, "19.50");
    float0.compile(space)->str();
    EXPECT_TRUE(float0.isLiteral(space.sym()));

    semantic::BoolLiteral bool0(pos, true);
    bool0.compile(space)->str();
    EXPECT_TRUE(bool0.isLiteral(space.sym()));
    EXPECT_TRUE(bool0.boolValue(space.sym()));

    semantic::IntLiteral int1(pos, "441499");
    int1.compile(space)->str();
    EXPECT_TRUE(int1.isLiteral(space.sym()));

    semantic::FloatLiteral float1(pos, "0.1950");
    float1.compile(space)->str();
    EXPECT_TRUE(float1.isLiteral(space.sym()));

    semantic::BoolLiteral bool1(pos, false);
    bool1.compile(space)->str();
    EXPECT_TRUE(bool1.isLiteral(space.sym()));
    EXPECT_FALSE(bool1.boolValue(space.sym()));

    semantic::StringLiteral str0(pos, "");
    str0.compile(space)->str();
    EXPECT_TRUE(str0.isLiteral(space.sym()));
    EXPECT_FALSE(str0.boolValue(space.sym()));
    EXPECT_FALSE(error::hasError());

    semantic::StringLiteral str1(pos, "x");
    str1.compile(space)->str();
    EXPECT_TRUE(str1.isLiteral(space.sym()));
    EXPECT_TRUE(str1.boolValue(space.sym()));
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (pos, INTEGER, "20110116")
        (pos, FLOATING, "19.5")
        (pos, BOOLEAN, "true")
        (pos, INTEGER, "441499")
        (pos, FLOATING, "0.195")
        (pos, BOOLEAN, "false")
        (pos, STRING, "")
        (pos, STRING, "x")
    ;
}

TEST_F(ExprNodesTest, ListLiterals)
{
    misc::position pos(2);
    semantic::CompilingSpace space;

    std::vector<util::sptr<semantic::Expression const>> members;
    members.push_back(util::mkptr(new semantic::IntLiteral(pos, "20110814")));
    members.push_back(util::mkptr(new semantic::ListLiteral(
                                    pos, std::vector<util::sptr<semantic::Expression const>>())));
    members.push_back(util::mkptr(new semantic::FloatLiteral(pos, "20.54")));

    semantic::ListLiteral ls(pos, std::move(members));
    ls.compile(space)->str();
    EXPECT_FALSE(ls.isLiteral(space.sym()));

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
    semantic::CompilingSpace space;
    space.sym()->defName(pos, "a20110116");
    space.sym()->defName(pos, "b1950");

    semantic::Reference ref0(pos, "a20110116");
    EXPECT_FALSE(ref0.isLiteral(space.sym()));
    ref0.compile(space)->str();

    semantic::Reference ref1(pos, "b1950");
    EXPECT_FALSE(ref0.isLiteral(space.sym()));
    ref1.compile(space)->str();

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
    semantic::CompilingSpace space;
    space.sym()->defName(pos, "darekatasukete");
    space.sym()->defName(pos, "leap");
    space.sym()->defName(pos_d, "fib");

    std::vector<util::sptr<semantic::Expression const>> args;
    semantic::Call call0(pos, util::mkptr(new semantic::Reference(pos, "fib")), std::move(args));

    args.push_back(util::mkptr(new semantic::BoolLiteral(pos, false)));
    args.push_back(util::mkptr(new semantic::PreUnaryOp(pos, "-", util::mkptr(
                                            new semantic::FloatLiteral(pos, "11.11")))));
    args.push_back(util::mkptr(new semantic::PreUnaryOp(pos, "!", util::mkptr(
                                            new semantic::BoolLiteral(pos, true)))));
    args.push_back(util::mkptr(new semantic::Reference(pos, "darekatasukete")));
    semantic::Call call1(pos, util::mkptr(new semantic::Reference(pos, "leap")), std::move(args));

    call0.compile(space)->str();
    EXPECT_FALSE(call0.isLiteral(space.sym()));

    call1.compile(space)->str();
    EXPECT_FALSE(call1.isLiteral(space.sym()));

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
    semantic::CompilingSpace space;
    semantic::BinaryOp bin_a(pos
                           , util::mkptr(new semantic::IntLiteral(pos, "20111"))
                           , "-"
                           , util::mkptr(new semantic::FloatLiteral(pos, "20110.4")));
    semantic::BinaryOp bin_b(pos
                           , util::mkptr(new semantic::StringLiteral(pos, "nov 8th"))
                           , "+"
                           , util::mkptr(new semantic::StringLiteral(pos, ", 2012")));
    EXPECT_TRUE(bin_a.isLiteral(space.sym()));
    bin_a.compile(space)->str();

    EXPECT_TRUE(bin_b.isLiteral(space.sym()));
    bin_b.compile(space)->str();

    ASSERT_FALSE(error::hasError());
    DataTree::expectOne()
        (pos, FLOATING, "0.6")
        (pos, STRING, "nov 8th, 2012")
    ;
}

TEST_F(ExprNodesTest, LiteralBoolValueError)
{
    misc::position pos(7);
    semantic::CompilingSpace space;
    semantic::IntLiteral int0(pos, "20110409");
    int0.boolValue(space.sym());

    semantic::FloatLiteral float0(pos, "10.58");
    float0.boolValue(space.sym());

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
    semantic::CompilingSpace space;
    semantic::BinaryOp conj(pos
                          , util::mkptr(new semantic::BoolLiteral(pos, true))
                          , "&&"
                          , util::mkptr(new semantic::FloatLiteral(pos, "20110.4")));
    conj.boolValue(space.sym());
    semantic::BinaryOp disj(pos
                          , util::mkptr(new semantic::BoolLiteral(pos, false))
                          , "||"
                          , util::mkptr(new semantic::IntLiteral(pos, "2")));
    disj.boolValue(space.sym());
    semantic::PreUnaryOp nega(pos, "!", util::mkptr(new semantic::FloatLiteral(pos, "1.12")));
    nega.boolValue(space.sym());

    semantic::BinaryOp binary(pos
                            , util::mkptr(new semantic::IntLiteral(pos, "1"))
                            , "*"
                            , util::mkptr(new semantic::FloatLiteral(pos, "11235.8")));
    binary.boolValue(space.sym());

    semantic::PreUnaryOp pre_unary(pos, "+", util::mkptr(new semantic::FloatLiteral(pos, ".13")));
    pre_unary.boolValue(space.sym());

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
    semantic::CompilingSpace space;
    space.sym()->defName(pos, "chiaki");
    space.sym()->defName(pos, "douma");

    semantic::ListAppend lsa(pos
                           , util::mkptr(new semantic::Reference(pos, "chiaki"))
                           , util::mkptr(new semantic::Reference(pos, "douma")));

    lsa.compile(space)->str();
    ASSERT_FALSE(error::hasError());

    lsa.compile(space)->str();

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
    semantic::CompilingSpace space;

    semantic::BinaryOp bin_a(pos_a
                           , util::mkptr(new semantic::IntLiteral(pos, "20121112"))
                           , "%"
                           , util::mkptr(new semantic::IntLiteral(pos, "0")));
    EXPECT_TRUE(bin_a.isLiteral(space.sym()));
    bin_a.compile(space);

    semantic::BinaryOp bin_b(pos_b
                           , util::mkptr(new semantic::IntLiteral(pos, "20121112"))
                           , "/"
                           , util::mkptr(new semantic::FloatLiteral(pos, ".0")));
    EXPECT_TRUE(bin_b.isLiteral(space.sym()));
    bin_b.compile(space);

    EXPECT_TRUE(error::hasError());

    std::vector<DivZeroRec> recs(getDivZeroRecs());
    ASSERT_EQ(2, recs.size());
    ASSERT_EQ(pos_a, recs[0].pos);
    ASSERT_EQ(pos_b, recs[1].pos);
}

TEST_F(ExprNodesTest, TypeOf)
{
    misc::position pos(10);
    semantic::CompilingSpace space;
    space.sym()->defName(pos, "v1701");

    semantic::TypeOf t0(pos, util::mkptr(new semantic::BoolLiteral(pos, false)));
    semantic::TypeOf t1(pos, util::mkptr(new semantic::StringLiteral(pos, "s20121225")));
    semantic::TypeOf t2(pos, util::mkptr(new semantic::Reference(pos, "v1701")));
    semantic::BinaryOp bin(pos
                         , util::mkptr(new semantic::TypeOf(
                                        pos, util::mkptr(new semantic::IntLiteral(pos, "1700"))))
                         , "="
                         , util::mkptr(new semantic::StringLiteral(pos, "number")));

    t0.compile(space)->str();
    t1.compile(space)->str();
    t2.compile(space)->str();
    bin.compile(space)->str();
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (pos, STRING, "boolean")
        (pos, STRING, "string")
        (pos, PRE_UNARY_OP, "typeof ")
            (pos, REFERENCE, "v1701")
        (pos, BOOLEAN, "true")
    ;
}

TEST_F(ExprNodesTest, FoldBitwiseShift)
{
    misc::position pos(10);
    semantic::CompilingSpace space;

    semantic::BinaryOp bin_a(pos
                           , util::mkptr(new semantic::IntLiteral(pos, "-1"))
                           , ">>"
                           , util::mkptr(new semantic::IntLiteral(pos, "4")));
    EXPECT_TRUE(bin_a.isLiteral(space.sym()));
    bin_a.compile(space)->str();

    semantic::BinaryOp bin_b(pos
                           , util::mkptr(new semantic::IntLiteral(pos, "-1"))
                           , "<<"
                           , util::mkptr(new semantic::IntLiteral(pos, "4")));
    EXPECT_TRUE(bin_b.isLiteral(space.sym()));
    bin_b.compile(space)->str();

    semantic::BinaryOp bin_c(pos
                           , util::mkptr(new semantic::IntLiteral(pos, "4"))
                           , "<<"
                           , util::mkptr(new semantic::IntLiteral(pos, "2")));
    EXPECT_TRUE(bin_c.isLiteral(space.sym()));
    bin_c.compile(space)->str();

    semantic::BinaryOp bin_d(pos
                           , util::mkptr(new semantic::IntLiteral(pos, "-1"))
                           , ">>>"
                           , util::mkptr(new semantic::IntLiteral(pos, "4")));
    EXPECT_TRUE(bin_d.isLiteral(space.sym()));
    bin_d.compile(space)->str();

    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (pos, INTEGER, "-1")
        (pos, INTEGER, "-16")
        (pos, INTEGER, "16")
        (pos, INTEGER, util::str(long(0xFFFFFFFFUL >> 4)))
    ;
}

TEST_F(ExprNodesTest, FoldUnsupported)
{
    misc::position pos(11);
    misc::position pos_a(1100);
    misc::position pos_b(1101);
    semantic::CompilingSpace space;

    semantic::BinaryOp bin_a(pos_a
                           , util::mkptr(new semantic::FloatLiteral(pos, "2013.0119"))
                           , "<<"
                           , util::mkptr(new semantic::IntLiteral(pos, "0")));
    EXPECT_TRUE(bin_a.isLiteral(space.sym()));
    bin_a.compile(space);

    semantic::BinaryOp bin_b(pos_b
                           , util::mkptr(new semantic::IntLiteral(pos, "18"))
                           , ">>"
                           , util::mkptr(new semantic::FloatLiteral(pos, ".14")));
    EXPECT_TRUE(bin_b.isLiteral(space.sym()));
    bin_b.compile(space);

    EXPECT_TRUE(error::hasError());

    std::vector<BinaryOpNotAvaiRec> recs(getBinaryOpNotAvaiRecs());
    ASSERT_EQ(2, recs.size());
    ASSERT_EQ(pos_a, recs[0].pos);
    ASSERT_EQ("<<", recs[0].op_img);
    ASSERT_EQ("float", recs[0].lhst_name);
    ASSERT_EQ("int", recs[0].rhst_name);
    ASSERT_EQ(pos_b, recs[1].pos);
    ASSERT_EQ(">>", recs[1].op_img);
    ASSERT_EQ("int", recs[1].lhst_name);
    ASSERT_EQ("float", recs[1].rhst_name);
}

TEST_F(ExprNodesTest, PipeElementOutOfPipeEnvironment)
{
    misc::position pos(12);
    misc::position pos_a(1200);
    misc::position pos_b(1201);
    semantic::CompilingSpace space;

    util::sptr<semantic::Expression const> p(semantic::Pipeline::createMapper(
                pos
              , util::mkptr(new semantic::PipeKey(pos_a))
              , util::mkptr(new semantic::PipeElement(pos_b))));
    p->compile(space);
    ASSERT_TRUE(error::hasError());

    ASSERT_EQ(1, getPipeReferenceNotInListContextRecs().size());
    ASSERT_EQ(pos_a, getPipeReferenceNotInListContextRecs()[0].pos);
}

TEST_F(ExprNodesTest, SyncConditional)
{
    misc::position pos(13);
    semantic::CompilingSpace space;
    space.sym()->defName(pos, "shanon");
    space.sym()->defName(pos, "kanon");
    space.sym()->defName(pos, "beatoriche");

    util::sptr<semantic::Expression const> c(new semantic::Conditional(
                pos
              , util::mkptr(new semantic::Reference(pos, "shanon"))
              , util::mkptr(new semantic::Reference(pos, "kanon"))
              , util::mkptr(new semantic::Reference(pos, "beatoriche"))));
    c->compile(space)->str();

    EXPECT_FALSE(c->isLiteral(space.sym()));
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (pos, CONDITIONAL)
            (pos, REFERENCE, "shanon")
            (pos, REFERENCE, "kanon")
            (pos, REFERENCE, "beatoriche")
    ;

    util::sptr<semantic::Expression const> ca(new semantic::Conditional(
                pos
              , util::mkptr(new semantic::Reference(pos, "shanon"))
              , util::mkptr(new semantic::IntLiteral(pos, 20130225))
              , util::mkptr(new semantic::Reference(pos, "beatoriche"))));
    EXPECT_FALSE(ca->isLiteral(space.sym()));

    util::sptr<semantic::Expression const> cb(new semantic::Conditional(
                pos
              , util::mkptr(new semantic::Reference(pos, "shanon"))
              , util::mkptr(new semantic::IntLiteral(pos, 1452))
              , util::mkptr(new semantic::BoolLiteral(pos, false))));
    EXPECT_FALSE(ca->isLiteral(space.sym()));
}

TEST_F(ExprNodesTest, SyncConditionalFoldOnLiteralPredicate)
{
    misc::position pos(14);
    semantic::CompilingSpace space;
    space.sym()->defName(pos, "shanon");
    space.sym()->defName(pos, "kanon");
    space.sym()->defName(pos, "beatoriche");

    util::sptr<semantic::Expression const> c(new semantic::Conditional(
                pos
              , util::mkptr(new semantic::BoolLiteral(pos, false))
              , util::mkptr(new semantic::Reference(pos, "kanon"))
              , util::mkptr(new semantic::Reference(pos, "beatoriche"))));
    c->compile(space)->str();

    EXPECT_FALSE(c->isLiteral(space.sym()));
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (pos, REFERENCE, "beatoriche")
    ;
}

TEST_F(ExprNodesTest, SyncConditionalLiteral)
{
    misc::position pos(15);
    semantic::CompilingSpace space;

    util::sptr<semantic::Expression const> c(new semantic::Conditional(
                pos
              , util::mkptr(new semantic::BoolLiteral(pos, true))
              , util::mkptr(new semantic::StringLiteral(pos, "kirie"))
              , util::mkptr(new semantic::Reference(pos, "eba"))));
    c->compile(space)->str();

    EXPECT_TRUE(c->isLiteral(space.sym()));
    EXPECT_EQ("string", c->literalType(space.sym()));
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (pos, STRING, "kirie")
    ;
}
