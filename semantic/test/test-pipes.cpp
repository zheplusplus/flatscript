#include <gtest/gtest.h>

#include <output/node-base.h>
#include <test/phony-errors.h>
#include <test/common.h>

#include "test-common.h"
#include "../expr-nodes.h"
#include "../stmt-nodes.h"
#include "../list-pipe.h"
#include "../compiling-space.h"

using namespace test;

typedef SemanticTest PipelinesTest;

TEST_F(PipelinesTest, AsyncPipeTopExpression)
{
    misc::position pos(1);
    semantic::CompilingSpace space;
    semantic::Block block;
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    space.sym()->defName(pos, "f");
    space.sym()->defName(pos, "g");
    space.sym()->defName(pos, "list");

    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, semantic::Pipeline::createMapper(
                pos
              , util::mkptr(new semantic::Reference(pos, "list"))
              , util::mkptr(new semantic::AsyncCall(pos
                                                  , util::mkptr(new semantic::Reference(pos, "f"))
                                                  , std::move(fargs)
                                                  , std::vector<std::string>()
                                                  , std::move(largs)))))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                 new semantic::Call(pos
                                  , util::mkptr(new semantic::Reference(pos, "g"))
                                  , util::ptrarr<semantic::Expression const>())))));

    compile(block, space.sym())->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (ASYNC_RESULT_DEF)
                (pos, ASYNC_PIPELINE)
                    (pos, REFERENCE, "list")
                    (SCOPE_BEGIN)
                        (ASYNC_RESULT_DEF)
                        (pos, CALL, 1)
                            (pos, REFERENCE, "f")
                            (pos, FUNCTION, 0)
                                (MANGLE_AS_PARAM)
                                (SCOPE_BEGIN)
                                    (ARITHMETICS)
                                        (pos, CALL, 1)
                                            (pos, BINARY_OP, "[.]")
                                                (pos, PIPE_RESULT)
                                                (pos, REFERENCE, "push")
                                            (pos, ASYNC_REFERENCE)
                                    (PIPELINE_CONTINUE)
                                (SCOPE_END)
                    (SCOPE_END)
                    (SCOPE_BEGIN)
                        (ARITHMETICS)
                            (pos, PIPE_RESULT)
                        (ARITHMETICS)
                            (pos, CALL, 0)
                                (pos, REFERENCE, "g")
                    (SCOPE_END)
                    (EXC_THROW)
        (SCOPE_END)
    ;
}

TEST_F(PipelinesTest, AsyncPipeNestedExpression)
{
    misc::position pos(2);
    semantic::CompilingSpace space;
    semantic::Block block;
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    space.sym()->defName(pos, "f");
    space.sym()->importNames(pos, { "g" });
    space.sym()->defName(pos, "list");

    fargs.append(util::mkptr(new semantic::AsyncCall(pos
                                                   , util::mkptr(new semantic::Reference(pos, "g"))
                                                   , util::ptrarr<semantic::Expression const>()
                                                   , std::vector<std::string>({ "k" })
                                                   , util::ptrarr<semantic::Expression const>())));
    block.addStmt(util::mkptr(new semantic::Import(pos, std::vector<std::string>{ "h" })));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, semantic::Pipeline::createMapper(
                    pos
                  , util::mkptr(new semantic::Reference(pos, "list"))
                  , util::mkptr(new semantic::Call(pos
                                                 , util::mkptr(new semantic::Reference(pos, "f"))
                                                 , std::move(fargs)))))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                 new semantic::Call(pos
                                  , util::mkptr(new semantic::Reference(pos, "h"))
                                  , util::ptrarr<semantic::Expression const>())))));

    block.compile(space);
    space.deliver()->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (FWD_DECL, "f")
            (FWD_DECL, "list")
            (ASYNC_RESULT_DEF)
                (pos, ASYNC_PIPELINE)
                    (pos, REFERENCE, "list")
                    (SCOPE_BEGIN)
                        (ASYNC_RESULT_DEF)
                        (pos, CALL, 1)
                            (pos, IMPORTED_NAME, "g")
                            (pos, FUNCTION, 1)
                                (PARAMETER, "k")
                                (MANGLE_AS_PARAM)
                                (SCOPE_BEGIN)
                                    (ARITHMETICS)
                                        (pos, CALL, 1)
                                            (pos, BINARY_OP, "[.]")
                                                (pos, PIPE_RESULT)
                                                (pos, REFERENCE, "push")
                                            (pos, CALL, 1)
                                                (pos, REFERENCE, "f")
                                                (pos, ASYNC_REFERENCE)
                                    (PIPELINE_CONTINUE)
                                (SCOPE_END)
                    (SCOPE_END)
                    (SCOPE_BEGIN)
                        (ARITHMETICS)
                            (pos, PIPE_RESULT)
                        (ARITHMETICS)
                            (pos, CALL, 0)
                                (pos, IMPORTED_NAME, "h")
                    (SCOPE_END)
                    (EXC_THROW)
        (SCOPE_END)
    ;
}

TEST_F(PipelinesTest, RefNameDefInAsyncWithinPipeSection)
{
    misc::position pos(3);
    misc::position pos_err(300);
    semantic::CompilingSpace space;
    semantic::Block block;
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    space.sym()->defName(pos, "f");
    space.sym()->defName(pos, "g");
    space.sym()->defName(pos, "list");

    fargs.append(util::mkptr(new semantic::AsyncCall(pos
                                                   , util::mkptr(new semantic::Reference(pos, "g"))
                                                   , util::ptrarr<semantic::Expression const>()
                                                   , std::vector<std::string>({ "h" })
                                                   , util::ptrarr<semantic::Expression const>())));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, semantic::Pipeline::createMapper(
                    pos
                  , util::mkptr(new semantic::Reference(pos, "list"))
                  , util::mkptr(new semantic::Call(pos
                                                 , util::mkptr(new semantic::Reference(pos, "f"))
                                                 , std::move(fargs)))))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                 new semantic::Call(pos
                                  , util::mkptr(new semantic::Reference(pos_err, "h"))
                                  , util::ptrarr<semantic::Expression const>())))));

    compile(block, space.sym());
    ASSERT_TRUE(error::hasError());

    std::vector<NameNotDefRec> nodefs(getNameNotDefRecs());
    ASSERT_EQ(1, nodefs.size());
    ASSERT_EQ("h", nodefs[0].name);
    ASSERT_EQ(pos_err, nodefs[0].ref_pos);
}

TEST_F(PipelinesTest, PipeBlock)
{
    misc::position pos(4);
    semantic::CompilingSpace space;
    semantic::Block block;
    util::ptrarr<semantic::Expression const> args;

    space.sym()->defName(pos, "merin");
    space.sym()->defName(pos, "sakuya");

    semantic::Block pipe_sec;
    pipe_sec.addStmt(util::mkptr(new semantic::NameDef(
                    pos, "scarlet", util::mkptr(new semantic::Reference(pos, "sakuya")))));

    args.append(util::mkptr(new semantic::IntLiteral(pos, 20130204)));
    pipe_sec.addStmt(util::mkptr(new semantic::Arithmetics(
                    pos, util::mkptr(new semantic::Call(
                                    pos
                                  , util::mkptr(new semantic::Reference(pos, "scarlet"))
                                  , std::move(args))))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(new semantic::Pipeline(
                  pos, util::mkptr(new semantic::Reference(pos, "merin")), std::move(pipe_sec))))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                                    new semantic::Reference(pos, "merin")))));

    compile(block, space.sym())->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (ARITHMETICS)
                (pos, SYNC_PIPELINE)
                    (pos, REFERENCE, "merin")
                    (SCOPE_BEGIN)
                        (FWD_DECL, "scarlet")
                        (ARITHMETICS)
                            (pos, BINARY_OP, "[=]")
                                (pos, REFERENCE, "scarlet")
                                (pos, REFERENCE, "sakuya")
                        (ARITHMETICS)
                            (pos, CALL, 1)
                                (pos, REFERENCE, "scarlet")
                                (pos, INTEGER, "20130204")
                    (SCOPE_END)
            (ARITHMETICS)
                (pos, REFERENCE, "merin")
        (SCOPE_END)
    ;
}

TEST_F(PipelinesTest, PipeAsyncBlock)
{
    misc::position pos(4);
    semantic::CompilingSpace space;
    semantic::Block block;
    util::ptrarr<semantic::Expression const> args;

    space.sym()->defName(pos, "merin");
    space.sym()->defName(pos, "sakuya");

    semantic::Block pipe_sec;

    args.append(util::mkptr(new semantic::IntLiteral(pos, 1042)));
    pipe_sec.addStmt(util::mkptr(new semantic::Arithmetics(
                    pos, util::mkptr(new semantic::AsyncCall(
                                    pos
                                  , util::mkptr(new semantic::Reference(pos, "sakuya"))
                                  , util::ptrarr<semantic::Expression const>()
                                  , std::vector<std::string>({ "scarlet" })
                                  , util::ptrarr<semantic::Expression const>())))));
    pipe_sec.addStmt(util::mkptr(new semantic::Arithmetics(
                    pos, util::mkptr(new semantic::Reference(pos, "scarlet")))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(new semantic::Pipeline(
                  pos, util::mkptr(new semantic::Reference(pos, "merin")), std::move(pipe_sec))))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                                new semantic::Reference(pos, "merin")))));

    compile(block, space.sym())->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (ASYNC_RESULT_DEF)
                (pos, ASYNC_PIPELINE)
                    (pos, REFERENCE, "merin")
                    (SCOPE_BEGIN)
                        (ASYNC_RESULT_DEF)
                            (pos, CALL, 1)
                                (pos, REFERENCE, "sakuya")
                                (pos, FUNCTION, 1)
                                    (PARAMETER, "scarlet")
                                    (MANGLE_AS_PARAM)
                                    (SCOPE_BEGIN)
                                        (ARITHMETICS)
                                            (pos, ASYNC_REFERENCE)
                                        (ARITHMETICS)
                                            (pos, TRANSIENT_PARAMETER, "scarlet")
                                        (PIPELINE_CONTINUE)
                                    (SCOPE_END)
                    (SCOPE_END)
                    (SCOPE_BEGIN)
                        (ARITHMETICS)
                            (pos, PIPE_RESULT)
                        (ARITHMETICS)
                            (pos, REFERENCE, "merin")
                    (SCOPE_END)
                    (EXC_THROW)
        (SCOPE_END)
    ;
}

TEST_F(PipelinesTest, ReturnInPipelineContext)
{
    misc::position pos(5);
    misc::position pos_a(500);
    semantic::CompilingSpace space;
    semantic::Block block;
    util::ptrarr<semantic::Expression const> args;

    space.sym()->defName(pos, "marisa");
    space.sym()->defName(pos, "reimu");

    semantic::Block pipe_sec;

    args.append(util::mkptr(new semantic::IntLiteral(pos, 1654)));
    pipe_sec.addStmt(util::mkptr(new semantic::Return(
                    pos, util::mkptr(new semantic::AsyncCall(
                                    pos_a
                                  , util::mkptr(new semantic::Reference(pos, "marisa"))
                                  , util::ptrarr<semantic::Expression const>()
                                  , std::vector<std::string>({ "x" })
                                  , util::ptrarr<semantic::Expression const>())))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(new semantic::Pipeline(
                  pos, util::mkptr(new semantic::Reference(pos, "reimu")), std::move(pipe_sec))))));

    compile(block, space.sym());
    ASSERT_TRUE(error::hasError());

    std::vector<ReturnNotAllowedInPipeRec> recs(getReturnNotAllowedInPipeRecs());
    ASSERT_EQ(1, recs.size());
    ASSERT_EQ(pos_a, recs[0].pos);
}

TEST_F(PipelinesTest, RedefineImportNameSameAsAsyncParam)
{
    misc::position pos(6);
    misc::position pos_ap(600);
    misc::position pos_import(601);
    semantic::CompilingSpace space;
    semantic::Block block;

    block.addStmt(util::mkptr(new semantic::Import(pos, std::vector<std::string>{ "g" })));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::AsyncCall(pos_ap
                                      , util::mkptr(new semantic::Reference(pos, "g"))
                                      , util::ptrarr<semantic::Expression const>()
                                      , std::vector<std::string>({ "h" })
                                      , util::ptrarr<semantic::Expression const>())))));
    block.addStmt(util::mkptr(new semantic::Import(pos_import, std::vector<std::string>{ "h" })));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::Call(pos
                                 , util::mkptr(new semantic::Reference(pos, "h"))
                                 , util::ptrarr<semantic::Expression const>())))));

    block.compile(space);
    ASSERT_TRUE(error::hasError());

    std::vector<NameAlreadyInLocalRec> redefs = getNameAlreadyInLocalRecs();
    ASSERT_EQ(1, redefs.size());
    ASSERT_EQ(pos_ap, redefs[0].prev_def_pos);
    ASSERT_EQ(pos_import, redefs[0].this_def_pos);
    ASSERT_EQ("h", redefs[0].name);
}

TEST_F(PipelinesTest, RedefineAsyncParamSameAsImportName)
{
    misc::position pos(7);
    misc::position pos_import(700);
    misc::position pos_ap(701);
    semantic::CompilingSpace space;
    semantic::Block block;

    block.addStmt(util::mkptr(new semantic::Import(pos, std::vector<std::string>{ "g" })));
    block.addStmt(util::mkptr(new semantic::Import(pos_import, std::vector<std::string>{ "h" })));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::AsyncCall(pos_ap
                                      , util::mkptr(new semantic::Reference(pos, "g"))
                                      , util::ptrarr<semantic::Expression const>()
                                      , std::vector<std::string>({ "h" })
                                      , util::ptrarr<semantic::Expression const>())))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::Call(pos
                                 , util::mkptr(new semantic::Reference(pos, "h"))
                                 , util::ptrarr<semantic::Expression const>())))));

    block.compile(space);
    ASSERT_TRUE(error::hasError());

    std::vector<NameAlreadyInLocalRec> redefs = getNameAlreadyInLocalRecs();
    ASSERT_EQ(1, redefs.size());
    ASSERT_EQ(pos_import, redefs[0].prev_def_pos);
    ASSERT_EQ(pos_ap, redefs[0].this_def_pos);
    ASSERT_EQ("h", redefs[0].name);
}
