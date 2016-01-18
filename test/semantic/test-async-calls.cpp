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

typedef SemanticTest AsyncCallsTest;

TEST_F(AsyncCallsTest, TopFlowCalls)
{
    misc::position pos(1);
    util::sptr<semantic::Scope> scope(semantic::Scope::global());
    semantic::Block block(pos);
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    scope->sym()->defName(pos, "setTimeout");
    scope->sym()->defName(pos, "cb");
    scope->sym()->defName(pos, "readLine");
    scope->sym()->defName(pos, "writeLine");
    scope->sym()->defName(pos, "log");

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

    compile(block, *scope)->write(dummyos());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (ASYNC_RESULT_DEF)
                (CALL, 2)
                    (REFERENCE, "setTimeout")
                    (FUNCTION, 0)
                        (MANGLE_AS_PARAM)
                        (SCOPE_BEGIN)
                            (ARITHMETICS)
                                (CALL, 0)
                                    (REFERENCE, "cb")
                            (ASYNC_RESULT_DEF)
                                (CALL, 2)
                                    (REFERENCE, "readLine")
                                    (STRING, "config.ini")
                                    (FUNCTION, 1)
                                        (PARAMETER, "content")
                                        (MANGLE_AS_PARAM)
                                        (SCOPE_BEGIN)
                                            (ASYNC_RESULT_DEF)
                                                (CALL, 3)
                                                    (REFERENCE, "writeLine")
                                                    (INTEGER, "200")
                                                    (FUNCTION, 1)
                                                        (PARAMETER, "err")
                                                        (MANGLE_AS_PARAM)
                                                        (SCOPE_BEGIN)
                                                            (ARITHMETICS)
                                                                (CALL, 1)
                                                                    (REFERENCE, "log")
                                                                    (TRANSIENT_PARAMETER, "err")
                                                        (SCOPE_END)
                                                    (TRANSIENT_PARAMETER, "content")
                                        (SCOPE_END)
                        (SCOPE_END)
                    (INTEGER, "1")
        (SCOPE_END)
    ;
}

TEST_F(AsyncCallsTest, InBranch)
{
    misc::position pos(2);
    util::sptr<semantic::Scope> scope(semantic::Scope::global());
    semantic::Block block(pos);
    util::sptr<semantic::Block> consq_block(new semantic::Block(pos));
    util::sptr<semantic::Block> alter_block(new semantic::Block(pos));
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    scope->sym()->defName(pos, "x");
    scope->sym()->defName(pos, "y");
    scope->sym()->defName(pos, "read");

    largs.append(util::mkptr(new semantic::StringLiteral(pos, "f20130106")));
    consq_block->addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                new semantic::AsyncCall(pos
                                      , util::mkptr(new semantic::Reference(pos, "read"))
                                      , std::move(fargs)
                                      , std::vector<std::string>({ "content" })
                                      , std::move(largs))))));
    consq_block->addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                              new semantic::Reference(pos, "content")))));

    alter_block->addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                                new semantic::Reference(pos, "x")))));

    block.addStmt(util::mkptr(new semantic::Branch(
                     pos
                   , util::mkptr(new semantic::Reference(pos, "y"))
                   , std::move(consq_block)
                   , std::move(alter_block))));

    block.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                                new semantic::FloatLiteral(pos, "17.53")))));

    compile(block, *scope)->write(dummyos());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (FUNCTION, "# AnonymousCallback", 0)
                (SCOPE_BEGIN)
                (SCOPE_END)
            (BRANCH)
                (REFERENCE, "y")
            (SCOPE_BEGIN)
                (ASYNC_RESULT_DEF)
                    (CALL, 2)
                        (REFERENCE, "read")
                        (FUNCTION, 1)
                            (PARAMETER, "content")
                            (MANGLE_AS_PARAM)
                            (SCOPE_BEGIN)
                                (ARITHMETICS)
                                    (CALL, 0)
                                        (REFERENCE, "# AnonymousCallback")
                            (SCOPE_END)
                        (STRING, "f20130106")
            (SCOPE_END)
            (SCOPE_BEGIN)
                (ARITHMETICS)
                    (CALL, 0)
                        (REFERENCE, "# AnonymousCallback")
            (SCOPE_END)
        (SCOPE_END)
    ;
}

TEST_F(AsyncCallsTest, AsBranchPredicate)
{
    misc::position pos(3);
    util::sptr<semantic::Scope> scope(semantic::Scope::global());
    semantic::Block block(pos);
    util::sptr<semantic::Block> consq_block(new semantic::Block(pos));
    util::sptr<semantic::Block> alter_block(new semantic::Block(pos));
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    scope->sym()->defName(pos, "m");
    scope->sym()->defName(pos, "n");
    scope->sym()->defName(pos, "write");

    consq_block->addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                                new semantic::Reference(pos, "m")))));
    alter_block->addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
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

    compile(block, *scope)->write(dummyos());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (ASYNC_RESULT_DEF)
                (CALL, 2)
                    (REFERENCE, "write")
                    (FUNCTION, 1)
                        (PARAMETER, "err")
                        (MANGLE_AS_PARAM)
                        (SCOPE_BEGIN)
                            (BRANCH)
                                (ASYNC_REFERENCE)
                            (SCOPE_BEGIN)
                            (SCOPE_END)
                            (SCOPE_BEGIN)
                            (SCOPE_END)
                        (SCOPE_END)
                    (REFERENCE, "n")
        (SCOPE_END)
    ;
}

TEST_F(AsyncCallsTest, ConflictDefinition)
{
    misc::position pos(4);
    misc::position pos_a(400);
    misc::position pos_b(401);
    misc::position pos_c(402);
    util::sptr<semantic::Scope> scope(semantic::Scope::global());
    semantic::Block block(pos);
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

    compile(block, *scope);
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
    util::sptr<semantic::Scope> scope(semantic::Scope::global());
    semantic::Block block(pos);
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    scope->sym()->defName(pos, "f");
    scope->sym()->defName(pos, "g");
    scope->sym()->defName(pos, "h");

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

    compile(block, *scope)->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (ASYNC_RESULT_DEF)
                (CALL, 1)
                    (REFERENCE, "g")
                    (FUNCTION, 0)
                        (MANGLE_AS_PARAM)
                        (SCOPE_BEGIN)
                            (ASYNC_RESULT_DEF)
                                (CALL, 1)
                                    (REFERENCE, "h")
                                    (FUNCTION, 0)
                                        (MANGLE_AS_PARAM)
                                        (SCOPE_BEGIN)
                                            (ASYNC_RESULT_DEF)
                                                (CALL, 3)
                                                    (REFERENCE, "f")
                                                    (ASYNC_REFERENCE)
                                                    (FUNCTION, 1)
                                                        (PARAMETER, "x")
                                                        (MANGLE_AS_PARAM)
                                                        (SCOPE_BEGIN)
                                                            (ARITHMETICS)
                                                                (CALL, 0)
                                                                    (TRANSIENT_PARAMETER, "x")
                                                        (SCOPE_END)
                                                    (ASYNC_REFERENCE)
                                        (SCOPE_END)
                        (SCOPE_END)
        (SCOPE_END)
    ;
}

TEST_F(AsyncCallsTest, AsyncCallInConditionalConsequence)
{
    misc::position pos(16);
    util::sptr<semantic::Scope> scope(semantic::Scope::global());
    semantic::Block block(pos);

    scope->sym()->defName(pos, "suzuha");
    scope->sym()->defName(pos, "yuki");
    scope->sym()->defName(pos, "rintarou");
    scope->sym()->defName(pos, "mayuri");

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

    compile(block, *scope)->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (FWD_DECL, "ruka")
            (FUNCTION, "# AnonymousCallback", 1)
                (PARAMETER, "ConditionalCallback # Parameter")
                (SCOPE_BEGIN)
                    (ARITHMETICS)
                        (BINARY_OP, "[=]")
                            (REFERENCE, "ruka")
                            (COND_CALLBACK_PARAM)
                    (ARITHMETICS)
                        (CALL, 1)
                            (REFERENCE, "mayuri")
                            (REFERENCE, "ruka")
                (SCOPE_END)
            (BRANCH)
                (REFERENCE, "rintarou")
                (SCOPE_BEGIN)
                    (ASYNC_RESULT_DEF)
                        (CALL, 2)
                            (REFERENCE, "suzuha")
                            (FUNCTION, 1)
                                (PARAMETER, "itaru")
                                (MANGLE_AS_PARAM)
                                (SCOPE_BEGIN)
                                    (ARITHMETICS)
                                        (CALL, 1)
                                            (REFERENCE, "# AnonymousCallback")
                                            (CALL, 1)
                                                (REFERENCE, "yuki")
                                                (ASYNC_REFERENCE)
                                (SCOPE_END)
                            (INTEGER, "1559")
                (SCOPE_END)
                (SCOPE_BEGIN)
                    (ARITHMETICS)
                        (CALL, 1)
                            (REFERENCE, "# AnonymousCallback")
                            (BOOLEAN, "false")
                (SCOPE_END)
        (SCOPE_END)
    ;
}

TEST_F(AsyncCallsTest, RegularAsyncCall)
{
    misc::position pos(17);
    util::sptr<semantic::Scope> scope(semantic::Scope::global());
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;
    semantic::Block body(pos);

    scope->sym()->defName(pos, "yurine");

    fargs.append(util::mkptr(new semantic::StringLiteral(pos, "karas")));
    body.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                        new semantic::RegularAsyncCall(
                                        pos
                                      , util::mkptr(new semantic::Reference(pos, "yurine"))
                                      , std::move(fargs)
                                      , std::move(largs))))));

    body.compile(*scope);
    scope->deliver()->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (FWD_DECL, "yurine")
            (ARITHMETICS)
                (CALL, 2)
                    (REFERENCE, "yurine")
                    (STRING, "karas")
                    (FUNCTION)
                        (PARAMETER, "# RegularAsyncCallbackParameters")
                        (EXC_THROW)
                        (SCOPE_BEGIN)
                        (SCOPE_END)
        (SCOPE_END)
    ;
}
