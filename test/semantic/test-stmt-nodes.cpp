#include <gtest/gtest.h>

#include <semantic/stmt-nodes.h>
#include <semantic/expr-nodes.h>
#include <semantic/compiling-space.h>
#include <util/string.h>
#include <output/node-base.h>
#include <test/phony-errors.h>
#include <test/common.h>

#include "test-common.h"

using namespace test;

typedef SemanticTest StmtNodesTest;

TEST_F(StmtNodesTest, AsyncSpaceInBranchWithConstantPredicate)
{
    misc::position pos(1);
    semantic::CompilingSpace space;
    semantic::Block block;
    semantic::Block consq_block;
    semantic::Block alter_block;
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    space.sym()->defName(pos, "x");
    space.sym()->defName(pos, "read");

    largs.append(util::mkptr(new semantic::StringLiteral(pos, "f20130123")));
    consq_block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::AsyncCall(pos
                                      , util::mkptr(new semantic::Reference(pos, "read"))
                                      , std::move(fargs)
                                      , std::vector<std::string>({ "content" })
                                      , std::move(largs))))));
    consq_block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                            new semantic::Reference(pos, "content")))));

    alter_block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                            new semantic::Reference(pos, "x")))));

    block.addStmt(util::mkptr(new semantic::Branch(
                     pos
                   , util::mkptr(new semantic::BoolLiteral(pos, true))
                   , std::move(consq_block)
                   , std::move(alter_block))));

    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                            new semantic::FloatLiteral(pos, "11.04")))));

    compile(block, space.sym())->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (SCOPE_BEGIN)
                (ASYNC_RESULT_DEF)
                    (pos, CALL, 2)
                        (pos, REFERENCE, "read")
                        (pos, FUNCTION, 1)
                            (PARAMETER, "content")
                            (MANGLE_AS_PARAM)
                            (SCOPE_BEGIN)
                            (SCOPE_END)
                        (pos, STRING, "f20130123")
            (SCOPE_END)
        (SCOPE_END)
    ;
}

TEST_F(StmtNodesTest, ReferenceThisInBranch)
{
    misc::position pos(2);
    semantic::CompilingSpace space;
    semantic::Block block;

    semantic::Block consq_block;
    semantic::Block alter_block;

    space.sym()->defName(pos, "houjou");
    space.sym()->defName(pos, "ryuuguu");

    consq_block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::Lookup(pos
                                   , util::mkptr(new semantic::This(pos))
                                   , util::mkptr(new semantic::StringLiteral(pos, "maebara")))))));
    alter_block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::Call(pos
                                 , util::mkptr(new semantic::Reference(pos, "ryuuguu"))
                                 , util::ptrarr<semantic::Expression const>())))));

    block.addStmt(util::mkptr(new semantic::Branch(
                     pos
                   , util::mkptr(new semantic::Reference(pos, "houjou"))
                   , std::move(consq_block)
                   , std::move(alter_block))));

    compile(block, space.sym())->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (DEC_THIS)
            (BRANCH)
            (pos, REFERENCE, "houjou")
            (SCOPE_BEGIN)
                (ARITHMETICS)
                    (pos, BINARY_OP, "[]")
                        (pos, THIS)
                        (pos, STRING, "maebara")
            (SCOPE_END)
            (SCOPE_BEGIN)
                (ARITHMETICS)
                    (pos, CALL, 0)
                        (pos, REFERENCE, "ryuuguu")
            (SCOPE_END)
        (SCOPE_END)
    ;
}

TEST_F(StmtNodesTest, ReferenceThisInBranchWithConstantPredicate)
{
    misc::position pos(3);
    semantic::CompilingSpace space;
    semantic::Block block;

    semantic::Block consq_block;
    semantic::Block alter_block;

    space.sym()->defName(pos, "rena");

    consq_block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::Lookup(pos
                                   , util::mkptr(new semantic::This(pos))
                                   , util::mkptr(new semantic::StringLiteral(pos, "keiiti")))))));
    alter_block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::Call(pos
                                 , util::mkptr(new semantic::Reference(pos, "rena"))
                                 , util::ptrarr<semantic::Expression const>())))));

    block.addStmt(util::mkptr(new semantic::Branch(
                     pos
                   , util::mkptr(new semantic::BoolLiteral(pos, false))
                   , std::move(consq_block)
                   , std::move(alter_block))));

    compile(block, space.sym())->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (SCOPE_BEGIN)
            (ARITHMETICS)
                (pos, CALL, 0)
                    (pos, REFERENCE, "rena")
            (SCOPE_END)
        (SCOPE_END)
    ;
}

TEST_F(StmtNodesTest, ReferenceThisInLambda)
{
    misc::position pos(4);
    semantic::CompilingSpace space;
    semantic::Block block;

    semantic::Block lambda_block;

    space.sym()->defName(pos, "satoko");
    space.sym()->defName(pos, "satosi");

    lambda_block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::Lookup(pos
                                   , util::mkptr(new semantic::Reference(pos, "satoko"))
                                   , util::mkptr(new semantic::This(pos)))))));

    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::Lambda(pos, std::vector<std::string>(), std::move(lambda_block))))));

    compile(block, space.sym())->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (ARITHMETICS)
                (pos, FUNCTION, 0)
                (SCOPE_BEGIN)
                    (DEC_THIS)
                    (ARITHMETICS)
                        (pos, BINARY_OP, "[]")
                            (pos, REFERENCE, "satoko")
                            (pos, THIS)
                (SCOPE_END)
        (SCOPE_END)
    ;
}

TEST_F(StmtNodesTest, ReferenceThisInHostFunction)
{
    misc::position pos(5);
    semantic::CompilingSpace space;
    semantic::Block block;

    semantic::Block lambda_block;

    space.sym()->defName(pos, "furukawa");
    space.sym()->defName(pos, "okasaki");

    lambda_block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::Lookup(pos
                                   , util::mkptr(new semantic::Reference(pos, "furukawa"))
                                   , util::mkptr(new semantic::StringLiteral(pos, "okasaki")))))));

    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::Lambda(pos, std::vector<std::string>(), std::move(lambda_block))))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::Lookup(pos
                                   , util::mkptr(new semantic::This(pos))
                                   , util::mkptr(new semantic::Reference(pos, "okasaki")))))));

    compile(block, space.sym())->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (DEC_THIS)
            (ARITHMETICS)
                (pos, FUNCTION, 0)
                (SCOPE_BEGIN)
                    (ARITHMETICS)
                        (pos, BINARY_OP, "[]")
                            (pos, REFERENCE, "furukawa")
                            (pos, STRING, "okasaki")
                (SCOPE_END)
            (ARITHMETICS)
                (pos, BINARY_OP, "[]")
                    (pos, THIS)
                    (pos, REFERENCE, "okasaki")
        (SCOPE_END)
    ;
}

TEST_F(StmtNodesTest, ReferenceThisInAsyncSpace)
{
    misc::position pos(6);
    semantic::CompilingSpace space;
    semantic::Block block;

    space.sym()->defName(pos, "tomoya");
    space.sym()->defName(pos, "nagisa");

    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::AsyncCall(pos
                                      , util::mkptr(new semantic::Reference(pos, "nagisa"))
                                      , util::ptrarr<semantic::Expression const>()
                                      , std::vector<std::string>()
                                      , util::ptrarr<semantic::Expression const>())))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::Lookup(pos
                                   , util::mkptr(new semantic::This(pos))
                                   , util::mkptr(new semantic::Reference(pos, "tomoya")))))));

    compile(block, space.sym())->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (DEC_THIS)
            (ASYNC_RESULT_DEF)
                (pos, CALL, 1)
                    (pos, REFERENCE, "nagisa")
                    (pos, FUNCTION, 0)
                        (MANGLE_AS_PARAM)
                        (SCOPE_BEGIN)
                            (ARITHMETICS)
                                (pos, BINARY_OP, "[]")
                                    (pos, THIS)
                                    (pos, REFERENCE, "tomoya")
                        (SCOPE_END)
        (SCOPE_END)
    ;
}

TEST_F(StmtNodesTest, StatementsAfterReturn)
{
    misc::position pos(7);
    misc::position pos_a(700);
    misc::position pos_b(701);
    semantic::CompilingSpace space;
    semantic::Block block;

    block.addStmt(util::mkptr(new semantic::Return(pos, util::mkptr(
                                        new semantic::IntLiteral(pos, 328)))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos_a, util::mkptr(
                                        new semantic::IntLiteral(pos, 2013)))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos_b, util::mkptr(
                                        new semantic::IntLiteral(pos, 923)))));

    block.compile(space);
    ASSERT_TRUE(error::hasError());

    ASSERT_EQ(1, getFlowTerminatedRecs().size());
    ASSERT_EQ(pos_a, getFlowTerminatedRecs()[0].this_pos);
    ASSERT_EQ(pos, getFlowTerminatedRecs()[0].prev_pos);
}

TEST_F(StmtNodesTest, StatementsAfterBothBranchesReturned)
{
    misc::position pos(8);
    misc::position pos_a(800);
    misc::position pos_b(801);
    misc::position pos_c(802);
    semantic::CompilingSpace space;
    space.sym()->defName(pos, "mion");
    space.sym()->defName(pos, "sion");
    semantic::Block block;
    semantic::Block consq_block;
    semantic::Block alter_block;

    consq_block.addStmt(util::mkptr(new semantic::Return(pos_a, util::mkptr(
                                        new semantic::IntLiteral(pos, 328)))));
    alter_block.addStmt(util::mkptr(new semantic::Return(pos_b, util::mkptr(
                                        new semantic::Reference(pos, "mion")))));
    block.addStmt(util::mkptr(new semantic::Branch(
                    pos
                  , util::mkptr(new semantic::Reference(pos, "sion"))
                  , std::move(consq_block)
                  , std::move(alter_block))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos_c, util::mkptr(
                                        new semantic::BoolLiteral(pos, false)))));

    block.compile(space);
    ASSERT_TRUE(error::hasError());

    ASSERT_EQ(1, getFlowTerminatedRecs().size());
    ASSERT_EQ(pos_c, getFlowTerminatedRecs()[0].this_pos);
    ASSERT_EQ(pos, getFlowTerminatedRecs()[0].prev_pos);
}

TEST_F(StmtNodesTest, StatementsAfterFoldedBranchReturned)
{
    misc::position pos(9);
    semantic::CompilingSpace space;
    space.sym()->defName(pos, "sonozaki");
    semantic::Block block;
    semantic::Block consq_block;
    semantic::Block alter_block;

    consq_block.addStmt(util::mkptr(new semantic::Return(pos, util::mkptr(
                                        new semantic::FloatLiteral(pos, 9.49)))));
    alter_block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                        new semantic::Reference(pos, "sonozaki")))));

    block.addStmt(util::mkptr(new semantic::Branch(
                    pos
                  , util::mkptr(new semantic::BoolLiteral(pos, true))
                  , std::move(consq_block)
                  , std::move(alter_block))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                        new semantic::BoolLiteral(pos, false)))));

    block.compile(space);
    ASSERT_FALSE(error::hasError());
}
