#include <gtest/gtest.h>

#include <semantic/expr-nodes.h>
#include <semantic/stmt-nodes.h>
#include <semantic/list-pipe.h>
#include <semantic/symbol-table.h>
#include <output/node-base.h>
#include <test/phony-errors.h>
#include <test/common.h>

#include "test-common.h"

using namespace test;

typedef SemanticTest PipelinesTest;

TEST_F(PipelinesTest, AsyncPipeTopExpression)
{
    misc::position pos(1);
    util::sptr<semantic::Scope> scope(semantic::Scope::global());
    semantic::Block block(pos);
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    scope->sym()->defName(pos, "f");
    scope->sym()->defName(pos, "g");
    scope->sym()->defName(pos, "list");

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

    compile(block, *scope)->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (ASYNC_RESULT_DEF)
                (ASYNC_PIPELINE)
                    (REFERENCE, "list")
                    (SCOPE_BEGIN)
                        (ASYNC_RESULT_DEF)
                        (CALL, 1)
                            (REFERENCE, "f")
                            (FUNCTION, 0)
                                (MANGLE_AS_PARAM)
                                (SCOPE_BEGIN)
                                    (ARITHMETICS)
                                        (CALL, 1)
                                            (BINARY_OP, "[.]")
                                                (PIPE_RESULT)
                                                (REFERENCE, "push")
                                            (ASYNC_REFERENCE)
                                    (CALL_NEXT)
                                        (CONTINUE)
                                (SCOPE_END)
                    (SCOPE_END)
                    (SCOPE_BEGIN)
                        (ARITHMETICS)
                            (CALL, 0)
                                (REFERENCE, "g")
                    (SCOPE_END)
                    (EXC_THROW)
        (SCOPE_END)
    ;
}

TEST_F(PipelinesTest, AsyncPipeNestedExpression)
{
    misc::position pos(2);
    util::sptr<semantic::Scope> scope(semantic::Scope::global());
    semantic::Block block(pos);
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    scope->sym()->defName(pos, "f");
    scope->sym()->addExternNames(pos, { "g" });
    scope->sym()->defName(pos, "list");

    fargs.append(util::mkptr(new semantic::AsyncCall(pos
                                                   , util::mkptr(new semantic::Reference(pos, "g"))
                                                   , util::ptrarr<semantic::Expression const>()
                                                   , std::vector<std::string>({ "k" })
                                                   , util::ptrarr<semantic::Expression const>())));
    block.addStmt(util::mkptr(new semantic::Extern(pos, std::vector<std::string>{ "h" })));
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

    block.compile(*scope);
    scope->deliver()->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (FWD_DECL, "f")
            (FWD_DECL, "list")
            (ASYNC_RESULT_DEF)
                (ASYNC_PIPELINE)
                    (REFERENCE, "list")
                    (SCOPE_BEGIN)
                        (ASYNC_RESULT_DEF)
                        (CALL, 1)
                            (IMPORTED_NAME, "g")
                            (FUNCTION, 1)
                                (PARAMETER, "k")
                                (MANGLE_AS_PARAM)
                                (SCOPE_BEGIN)
                                    (ARITHMETICS)
                                        (CALL, 1)
                                            (BINARY_OP, "[.]")
                                                (PIPE_RESULT)
                                                (REFERENCE, "push")
                                            (CALL, 1)
                                                (REFERENCE, "f")
                                                (ASYNC_REFERENCE)
                                    (CALL_NEXT)
                                        (CONTINUE)
                                (SCOPE_END)
                    (SCOPE_END)
                    (SCOPE_BEGIN)
                        (ARITHMETICS)
                            (CALL, 0)
                                (IMPORTED_NAME, "h")
                    (SCOPE_END)
                    (EXC_THROW)
        (SCOPE_END)
    ;
}

TEST_F(PipelinesTest, RefNameDefInAsyncWithinPipeSection)
{
    misc::position pos(3);
    misc::position pos_err(300);
    util::sptr<semantic::Scope> scope(semantic::Scope::global());
    semantic::Block block(pos);
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    scope->sym()->defName(pos, "f");
    scope->sym()->defName(pos, "g");
    scope->sym()->defName(pos, "list");

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

    compile(block, *scope);
    ASSERT_TRUE(error::hasError());

    std::vector<NameNotDefRec> nodefs(getNameNotDefRecs());
    ASSERT_EQ(1, nodefs.size());
    ASSERT_EQ("h", nodefs[0].name);
    ASSERT_EQ(pos_err, nodefs[0].ref_pos);
}

TEST_F(PipelinesTest, PipeBlock)
{
    misc::position pos(4);
    util::sptr<semantic::Scope> scope(semantic::Scope::global());
    semantic::Block block(pos);
    util::ptrarr<semantic::Expression const> args;

    scope->sym()->defName(pos, "merin");
    scope->sym()->defName(pos, "sakuya");

    util::sptr<semantic::Block> pipe_sec(new semantic::Block(pos));
    pipe_sec->addStmt(util::mkptr(new semantic::NameDef(
                    pos, "scarlet", util::mkptr(new semantic::Reference(pos, "sakuya")))));

    args.append(util::mkptr(new semantic::IntLiteral(pos, 20130204)));
    pipe_sec->addStmt(util::mkptr(new semantic::Arithmetics(
                    pos, util::mkptr(new semantic::Call(
                                    pos
                                  , util::mkptr(new semantic::Reference(pos, "scarlet"))
                                  , std::move(args))))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(new semantic::Pipeline(
                  pos, util::mkptr(new semantic::Reference(pos, "merin")), std::move(pipe_sec))))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                                    new semantic::Reference(pos, "merin")))));

    compile(block, *scope)->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (ARITHMETICS)
                (ROOT_SYNC_PIPELINE)
                    (REFERENCE, "merin")
                    (SCOPE_BEGIN)
                        (FWD_DECL, "scarlet")
                        (ARITHMETICS)
                            (BINARY_OP, "[=]")
                                (REFERENCE, "scarlet")
                                (REFERENCE, "sakuya")
                        (ARITHMETICS)
                            (CALL, 1)
                                (REFERENCE, "scarlet")
                                (INTEGER, "20130204")
                    (SCOPE_END)
        (SCOPE_END)
    ;
}

TEST_F(PipelinesTest, PipeAsyncBlock)
{
    misc::position pos(4);
    util::sptr<semantic::Scope> scope(semantic::Scope::global());
    semantic::Block block(pos);
    util::ptrarr<semantic::Expression const> args;

    scope->sym()->defName(pos, "merin");
    scope->sym()->defName(pos, "sakuya");

    util::sptr<semantic::Block> pipe_sec(new semantic::Block(pos));

    args.append(util::mkptr(new semantic::IntLiteral(pos, 1042)));
    pipe_sec->addStmt(util::mkptr(new semantic::Arithmetics(
                    pos, util::mkptr(new semantic::AsyncCall(
                                    pos
                                  , util::mkptr(new semantic::Reference(pos, "sakuya"))
                                  , util::ptrarr<semantic::Expression const>()
                                  , std::vector<std::string>({ "scarlet" })
                                  , util::ptrarr<semantic::Expression const>())))));
    pipe_sec->addStmt(util::mkptr(new semantic::Arithmetics(
                    pos, util::mkptr(new semantic::Reference(pos, "scarlet")))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(new semantic::Pipeline(
                  pos, util::mkptr(new semantic::Reference(pos, "merin")), std::move(pipe_sec))))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                                new semantic::Reference(pos, "merin")))));

    compile(block, *scope)->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (ASYNC_RESULT_DEF)
                (ASYNC_PIPELINE)
                    (REFERENCE, "merin")
                    (SCOPE_BEGIN)
                        (ASYNC_RESULT_DEF)
                            (CALL, 1)
                                (REFERENCE, "sakuya")
                                (FUNCTION, 1)
                                    (PARAMETER, "scarlet")
                                    (MANGLE_AS_PARAM)
                                    (SCOPE_BEGIN)
                                        (CALL_NEXT)
                                            (CONTINUE)
                                    (SCOPE_END)
                    (SCOPE_END)
                    (SCOPE_BEGIN)
                    (SCOPE_END)
                    (EXC_THROW)
        (SCOPE_END)
    ;
}

TEST_F(PipelinesTest, ReturnInRootPipelineContext)
{
    misc::position pos(5);
    misc::position pos_a(500);
    util::sptr<semantic::Scope> scope(semantic::Scope::global());
    semantic::Block block(pos);
    util::ptrarr<semantic::Expression const> args;

    scope->sym()->defName(pos, "marisa");
    scope->sym()->defName(pos, "reimu");

    util::sptr<semantic::Block> pipe_sec(new semantic::Block(pos));

    args.append(util::mkptr(new semantic::IntLiteral(pos, 1654)));
    pipe_sec->addStmt(util::mkptr(new semantic::Return(
                    pos_a, util::mkptr(new semantic::Call(
                                    pos
                                  , util::mkptr(new semantic::Reference(pos, "marisa"))
                                  , util::ptrarr<semantic::Expression const>())))));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(new semantic::Pipeline(
                  pos, util::mkptr(new semantic::Reference(pos, "reimu")), std::move(pipe_sec))))));

    compile(block, *scope)->write(dummyos());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (ARITHMETICS)
                (ROOT_SYNC_PIPELINE)
                    (REFERENCE, "reimu")
                    (SCOPE_BEGIN)
                        (SYNC_PIPELINE_RETURN)
                            (CALL, 0)
                                (REFERENCE, "marisa")
                    (SCOPE_END)
        (SCOPE_END)
    ;
}

TEST_F(PipelinesTest, ReturnInPipelineContext)
{
    misc::position pos(8);
    misc::position pos_a(800);
    util::sptr<semantic::Scope> scope(semantic::Scope::global());
    semantic::Block block(pos);
    util::ptrarr<semantic::Expression const> args;

    scope->sym()->defName(pos, "marisa");
    scope->sym()->defName(pos, "reimu");

    util::sptr<semantic::Block> pipe_sec(new semantic::Block(pos));

    args.append(util::mkptr(new semantic::IntLiteral(pos, 1654)));
    pipe_sec->addStmt(util::mkptr(new semantic::Return(
                    pos_a, util::mkptr(new semantic::AsyncCall(
                                    pos
                                  , util::mkptr(new semantic::Reference(pos, "marisa"))
                                  , util::ptrarr<semantic::Expression const>()
                                  , std::vector<std::string>({ "x" })
                                  , util::ptrarr<semantic::Expression const>())))));
    block.addStmt(util::mkptr(new semantic::NameDef(pos, "n20151203", util::mkptr(
                new semantic::Pipeline(pos
                                     , util::mkptr(new semantic::Reference(pos, "reimu"))
                                     , std::move(pipe_sec))))));

    compile(block, *scope);
    ASSERT_TRUE(error::hasError());

    std::vector<ReturnNotAllowedInExprPipeRec> recs(getReturnNotAllowedInExprPipeRecs());
    ASSERT_EQ(1, recs.size());
    ASSERT_EQ(pos_a, recs[0].pos);
}

TEST_F(PipelinesTest, RedefineExternalNameSameAsAsyncParam)
{
    misc::position pos(6);
    misc::position pos_ap(600);
    misc::position pos_extern(601);
    util::sptr<semantic::Scope> scope(semantic::Scope::global());
    semantic::Block block(pos);

    block.addStmt(util::mkptr(new semantic::Extern(pos, std::vector<std::string>{ "g" })));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::AsyncCall(pos_ap
                                      , util::mkptr(new semantic::Reference(pos, "g"))
                                      , util::ptrarr<semantic::Expression const>()
                                      , std::vector<std::string>({ "h" })
                                      , util::ptrarr<semantic::Expression const>())))));
    block.addStmt(util::mkptr(new semantic::Extern(pos_extern, std::vector<std::string>{ "h" })));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::Call(pos
                                 , util::mkptr(new semantic::Reference(pos, "h"))
                                 , util::ptrarr<semantic::Expression const>())))));

    block.compile(*scope);
    ASSERT_TRUE(error::hasError());

    std::vector<NameAlreadyInLocalRec> redefs = getNameAlreadyInLocalRecs();
    ASSERT_EQ(1, redefs.size());
    ASSERT_EQ(pos_ap, redefs[0].prev_def_pos);
    ASSERT_EQ(pos_extern, redefs[0].this_def_pos);
    ASSERT_EQ("h", redefs[0].name);
}

TEST_F(PipelinesTest, RedefineAsyncParamSameAsExternalName)
{
    misc::position pos(7);
    misc::position pos_extern(700);
    misc::position pos_ap(701);
    util::sptr<semantic::Scope> scope(semantic::Scope::global());
    semantic::Block block(pos);

    block.addStmt(util::mkptr(new semantic::Extern(pos, std::vector<std::string>{ "g" })));
    block.addStmt(util::mkptr(new semantic::Extern(pos_extern, std::vector<std::string>{ "h" })));
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

    block.compile(*scope);
    ASSERT_TRUE(error::hasError());

    std::vector<NameAlreadyInLocalRec> redefs = getNameAlreadyInLocalRecs();
    ASSERT_EQ(1, redefs.size());
    ASSERT_EQ(pos_extern, redefs[0].prev_def_pos);
    ASSERT_EQ(pos_ap, redefs[0].this_def_pos);
    ASSERT_EQ("h", redefs[0].name);
}
