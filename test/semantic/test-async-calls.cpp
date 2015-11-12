#include <gtest/gtest.h>

#include <semantic/expr-nodes.h>
#include <semantic/stmt-nodes.h>
#include <semantic/list-pipe.h>
#include <semantic/compiling-space.h>
#include <output/node-base.h>
#include <test/phony-errors.h>
#include <test/common.h>

#include "test-common.h"

using namespace test;

typedef SemanticTest AsyncCallsTest;

TEST_F(AsyncCallsTest, TopFlowCalls)
{
    misc::position pos(1);
    semantic::CompilingSpace space;
    semantic::Block block;
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    space.sym()->defName(pos, "setTimeout");
    space.sym()->defName(pos, "cb");
    space.sym()->defName(pos, "readLine");
    space.sym()->defName(pos, "writeLine");
    space.sym()->defName(pos, "log");

    largs.append(util::mkptr(new semantic::IntLiteral(pos, "1")));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::AsyncCall(pos
                                      , util::mkptr(new semantic::Reference(pos, "setTimeout"))
                                      , std::move(fargs)
                                      , std::vector<std::string>()
                                      , std::move(largs))))));

    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(new semantic::Call(
                    pos, util::mkptr(new semantic::Reference(pos, "cb")), std::move(largs))))));

    fargs.append(util::mkptr(new semantic::StringLiteral(pos, "config.ini")));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::AsyncCall(pos
                                      , util::mkptr(new semantic::Reference(pos, "readLine"))
                                      , std::move(fargs)
                                      , std::vector<std::string>({ "content" })
                                      , std::move(largs))))));

    fargs.append(util::mkptr(new semantic::IntLiteral(pos, "200")));
    largs.append(util::mkptr(new semantic::Reference(pos, "content")));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::AsyncCall(pos
                                      , util::mkptr(new semantic::Reference(pos, "writeLine"))
                                      , std::move(fargs)
                                      , std::vector<std::string>({ "err" })
                                      , std::move(largs))))));

    largs.append(util::mkptr(new semantic::Reference(pos, "err")));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(new semantic::Call(
                    pos, util::mkptr(new semantic::Reference(pos, "log")), std::move(largs))))));

    compile(block, space.sym())->write(dummyos());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (ASYNC_RESULT_DEF)
                (pos, CALL, 2)
                    (pos, REFERENCE, "setTimeout")
                    (pos, FUNCTION, 0)
                        (MANGLE_AS_PARAM)
                        (SCOPE_BEGIN)
                            (ARITHMETICS)
                                (pos, CALL, 0)
                                    (pos, REFERENCE, "cb")
                            (ASYNC_RESULT_DEF)
                                (pos, CALL, 2)
                                    (pos, REFERENCE, "readLine")
                                    (pos, STRING, "config.ini")
                                    (pos, FUNCTION, 1)
                                        (PARAMETER, "content")
                                        (MANGLE_AS_PARAM)
                                        (SCOPE_BEGIN)
                                            (ASYNC_RESULT_DEF)
                                                (pos, CALL, 3)
                                                    (pos, REFERENCE, "writeLine")
                                                    (pos, INTEGER, "200")
                                                    (pos, FUNCTION, 1)
                                                        (PARAMETER, "err")
                                                        (MANGLE_AS_PARAM)
                                                        (SCOPE_BEGIN)
                                                            (ARITHMETICS)
                                                                (pos, CALL, 1)
                                                                    (pos, REFERENCE, "log")
                                                                    (pos, TRANSIENT_PARAMETER, "err")
                                                        (SCOPE_END)
                                                    (pos, TRANSIENT_PARAMETER, "content")
                                        (SCOPE_END)
                        (SCOPE_END)
                    (pos, INTEGER, "1")
        (SCOPE_END)
    ;
}

TEST_F(AsyncCallsTest, InBranch)
{
    misc::position pos(2);
    semantic::CompilingSpace space;
    semantic::Block block;
    semantic::Block consq_block;
    semantic::Block alter_block;
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    space.sym()->defName(pos, "x");
    space.sym()->defName(pos, "y");
    space.sym()->defName(pos, "read");

    largs.append(util::mkptr(new semantic::StringLiteral(pos, "f20130106")));
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
                   , util::mkptr(new semantic::Reference(pos, "y"))
                   , std::move(consq_block)
                   , std::move(alter_block))));

    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                                new semantic::FloatLiteral(pos, "17.53")))));

    compile(block, space.sym())->write(dummyos());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (FUNCTION, "# AnonymousCallback", 0)
                (SCOPE_BEGIN)
                (SCOPE_END)
            (BRANCH)
                (pos, REFERENCE, "y")
            (SCOPE_BEGIN)
                (ASYNC_RESULT_DEF)
                    (pos, CALL, 2)
                        (pos, REFERENCE, "read")
                        (pos, FUNCTION, 1)
                            (PARAMETER, "content")
                            (MANGLE_AS_PARAM)
                            (SCOPE_BEGIN)
                                (ARITHMETICS)
                                    (pos, CALL, 0)
                                        (pos, REFERENCE, "# AnonymousCallback")
                            (SCOPE_END)
                        (pos, STRING, "f20130106")
            (SCOPE_END)
            (SCOPE_BEGIN)
                (ARITHMETICS)
                    (pos, CALL, 0)
                        (pos, REFERENCE, "# AnonymousCallback")
            (SCOPE_END)
        (SCOPE_END)
    ;
}

TEST_F(AsyncCallsTest, AsBranchPredicate)
{
    misc::position pos(3);
    semantic::CompilingSpace space;
    semantic::Block block;
    semantic::Block consq_block;
    semantic::Block alter_block;
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    space.sym()->defName(pos, "m");
    space.sym()->defName(pos, "n");
    space.sym()->defName(pos, "write");

    consq_block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                                new semantic::Reference(pos, "m")))));
    alter_block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                                new semantic::Reference(pos, "err")))));

    largs.append(util::mkptr(new semantic::Reference(pos, "n")));
    block.addStmt(util::mkptr(new semantic::Branch(
                     pos
                   , util::mkptr(new semantic::AsyncCall(
                                        pos
                                      , util::mkptr(new semantic::Reference(pos, "write"))
                                      , std::move(fargs)
                                      , std::vector<std::string>({ "err" })
                                      , std::move(largs)))
                   , std::move(consq_block)
                   , std::move(alter_block))));

    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                                new semantic::FloatLiteral(pos, "17.53")))));

    compile(block, space.sym())->write(dummyos());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (ASYNC_RESULT_DEF)
                (pos, CALL, 2)
                    (pos, REFERENCE, "write")
                    (pos, FUNCTION, 1)
                        (PARAMETER, "err")
                        (MANGLE_AS_PARAM)
                        (SCOPE_BEGIN)
                            (BRANCH)
                                (pos, ASYNC_REFERENCE)
                            (SCOPE_BEGIN)
                            (SCOPE_END)
                            (SCOPE_BEGIN)
                            (SCOPE_END)
                        (SCOPE_END)
                    (pos, REFERENCE, "n")
        (SCOPE_END)
    ;
}

TEST_F(AsyncCallsTest, ConflictDefinition)
{
    misc::position pos(4);
    misc::position pos_a(400);
    misc::position pos_b(401);
    misc::position pos_c(402);
    semantic::CompilingSpace space;
    semantic::Block block;
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    block.addStmt(util::mkptr(new semantic::NameDef(
                            pos, "suzuha", util::mkptr(new semantic::IntLiteral(pos, "1")))));
    largs.append(util::mkptr(new semantic::IntLiteral(pos, "1")));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::AsyncCall(pos_a
                                      , util::mkptr(new semantic::Reference(pos, "suzuha"))
                                      , std::move(fargs)
                                      , std::vector<std::string>({ "suzuha" })
                                      , std::move(largs))))));

    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::AsyncCall(pos_b
                                      , util::mkptr(new semantic::Reference(pos, "suzuha"))
                                      , std::move(fargs)
                                      , std::vector<std::string>({ "kyouma" })
                                      , std::move(largs))))));
    block.addStmt(util::mkptr(new semantic::NameDef(
                    pos_c, "kyouma", util::mkptr(new semantic::FloatLiteral(pos, "0.456914")))));

    compile(block, space.sym());
    ASSERT_TRUE(error::hasError());

    std::vector<NameAlreadyInLocalRec> redefs(getNameAlreadyInLocalRecs());
    ASSERT_EQ(2, redefs.size());
    ASSERT_EQ(pos, redefs[0].prev_def_pos);
    ASSERT_EQ(pos_a, redefs[0].this_def_pos);
    ASSERT_EQ("suzuha", redefs[0].name);

    ASSERT_EQ(pos_b, redefs[1].prev_def_pos);
    ASSERT_EQ(pos_c, redefs[1].this_def_pos);
    ASSERT_EQ("kyouma", redefs[1].name);
}

TEST_F(AsyncCallsTest, NestedAsyncArgs)
{
    misc::position pos(5);
    semantic::CompilingSpace space;
    semantic::Block block;
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    space.sym()->defName(pos, "f");
    space.sym()->defName(pos, "g");
    space.sym()->defName(pos, "h");

    fargs.append(util::mkptr(new semantic::AsyncCall(pos
                                                   , util::mkptr(new semantic::Reference(pos, "g"))
                                                   , util::ptrarr<semantic::Expression const>()
                                                   , std::vector<std::string>()
                                                   , util::ptrarr<semantic::Expression const>())));
    largs.append(util::mkptr(new semantic::AsyncCall(pos
                                                   , util::mkptr(new semantic::Reference(pos, "h"))
                                                   , util::ptrarr<semantic::Expression const>()
                                                   , std::vector<std::string>()
                                                   , util::ptrarr<semantic::Expression const>())));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::AsyncCall(pos
                                      , util::mkptr(new semantic::Reference(pos, "f"))
                                      , std::move(fargs)
                                      , std::vector<std::string>({ "x" })
                                      , std::move(largs))))));

    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                 new semantic::Call(pos
                                  , util::mkptr(new semantic::Reference(pos, "x"))
                                  , util::ptrarr<semantic::Expression const>())))));

    compile(block, space.sym())->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (ASYNC_RESULT_DEF)
                (pos, CALL, 1)
                    (pos, REFERENCE, "g")
                    (pos, FUNCTION, 0)
                        (MANGLE_AS_PARAM)
                        (SCOPE_BEGIN)
                            (ASYNC_RESULT_DEF)
                                (pos, CALL, 1)
                                    (pos, REFERENCE, "h")
                                    (pos, FUNCTION, 0)
                                        (MANGLE_AS_PARAM)
                                        (SCOPE_BEGIN)
                                            (ASYNC_RESULT_DEF)
                                                (pos, CALL, 3)
                                                    (pos, REFERENCE, "f")
                                                    (pos, ASYNC_REFERENCE)
                                                    (pos, FUNCTION, 1)
                                                        (PARAMETER, "x")
                                                        (MANGLE_AS_PARAM)
                                                        (SCOPE_BEGIN)
                                                            (ARITHMETICS)
                                                                (pos, CALL, 0)
                                                                    (pos, TRANSIENT_PARAMETER, "x")
                                                        (SCOPE_END)
                                                    (pos, ASYNC_REFERENCE)
                                        (SCOPE_END)
                        (SCOPE_END)
        (SCOPE_END)
    ;
}

TEST_F(AsyncCallsTest, AsyncCallInConditionalConsequence)
{
    misc::position pos(16);
    semantic::CompilingSpace space;
    semantic::Block block;

    space.sym()->defName(pos, "suzuha");
    space.sym()->defName(pos, "yuki");
    space.sym()->defName(pos, "rintarou");
    space.sym()->defName(pos, "mayuri");

    util::ptrarr<semantic::Expression const> args;
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    largs.append(util::mkptr(new semantic::IntLiteral(pos, 1559)));

    args.append(util::mkptr(new semantic::AsyncCall(
                pos
              , util::mkptr(new semantic::Reference(pos, "suzuha"))
              , std::move(fargs)
              , std::vector<std::string>({ "itaru" })
              , std::move(largs))));
    util::sptr<semantic::Expression const> call(new semantic::Call(
                pos, util::mkptr(new semantic::Reference(pos, "yuki")), std::move(args)));

    block.addStmt(util::mkptr(new semantic::NameDef(pos, "ruka", util::mkptr(
                new semantic::Conditional(pos
                                        , util::mkptr(new semantic::Reference(pos, "rintarou"))
                                        , std::move(call)
                                        , util::mkptr(new semantic::BoolLiteral(pos, false)))))));
    args.append(util::mkptr(new semantic::Reference(pos, "ruka")));
    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(new semantic::Call(
                    pos, util::mkptr(new semantic::Reference(pos, "mayuri")), std::move(args))))));

    compile(block, space.sym())->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (FWD_DECL, "ruka")
            (FUNCTION, "# AnonymousCallback", 1)
                (PARAMETER, "ConditionalCallback # Parameter")
                (SCOPE_BEGIN)
                    (ARITHMETICS)
                        (pos, BINARY_OP, "[=]")
                            (pos, REFERENCE, "ruka")
                            (pos, COND_CALLBACK_PARAM)
                    (ARITHMETICS)
                        (pos, CALL, 1)
                            (pos, REFERENCE, "mayuri")
                            (pos, REFERENCE, "ruka")
                (SCOPE_END)
            (BRANCH)
                (pos, REFERENCE, "rintarou")
                (SCOPE_BEGIN)
                    (ASYNC_RESULT_DEF)
                        (pos, CALL, 2)
                            (pos, REFERENCE, "suzuha")
                            (pos, FUNCTION, 1)
                                (PARAMETER, "itaru")
                                (MANGLE_AS_PARAM)
                                (SCOPE_BEGIN)
                                    (ARITHMETICS)
                                        (pos, CALL, 1)
                                            (pos, REFERENCE, "# AnonymousCallback")
                                            (pos, CALL, 1)
                                                (pos, REFERENCE, "yuki")
                                                (pos, ASYNC_REFERENCE)
                                (SCOPE_END)
                            (pos, INTEGER, "1559")
                (SCOPE_END)
                (SCOPE_BEGIN)
                    (ARITHMETICS)
                        (pos, CALL, 1)
                            (pos, REFERENCE, "# AnonymousCallback")
                            (pos, BOOLEAN, "false")
                (SCOPE_END)
        (SCOPE_END)
    ;
}

TEST_F(AsyncCallsTest, RegularAsyncCall)
{
    misc::position pos(17);
    semantic::CompilingSpace space;
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;
    semantic::Block body;

    space.sym()->defName(pos, "yurine");

    fargs.append(util::mkptr(new semantic::StringLiteral(pos, "karas")));
    body.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                        new semantic::RegularAsyncCall(
                                        pos
                                      , util::mkptr(new semantic::Reference(pos, "yurine"))
                                      , std::move(fargs)
                                      , std::move(largs))))));

    body.compile(space);
    space.deliver()->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (FWD_DECL, "yurine")
            (ARITHMETICS)
                (pos, CALL, 2)
                    (pos, REFERENCE, "yurine")
                    (pos, STRING, "karas")
                    (pos, FUNCTION)
                        (PARAMETER, "# RegularAsyncCallbackParameters")
                        (EXC_THROW)
                        (SCOPE_BEGIN)
                        (SCOPE_END)
        (SCOPE_END)
    ;
}