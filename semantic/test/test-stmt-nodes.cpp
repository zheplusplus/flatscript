#include <gtest/gtest.h>

#include <util/string.h>
#include <output/node-base.h>
#include <test/phony-errors.h>
#include <test/common.h>

#include "test-common.h"
#include "../stmt-nodes.h"
#include "../expr-nodes.h"
#include "../compiling-space.h"

using namespace test;

typedef SemanticTest StmtNodesTest;

TEST_F(StmtNodesTest, AsyncSpaceInBranchWithConstantPredicate)
{
    misc::position pos(1);
    semantic::CompilingSpace space;
    semantic::Filter filter;
    util::sptr<semantic::Filter> consq_filter(new semantic::Filter);
    util::sptr<semantic::Filter> alter_filter(new semantic::Filter);
    util::ptrarr<semantic::Expression const> fargs;
    util::ptrarr<semantic::Expression const> largs;

    space.sym()->defName(pos, "x");
    space.sym()->defName(pos, "read");

    largs.append(util::mkptr(new semantic::StringLiteral(pos, "f20130123")));
    consq_filter->addArith(pos, util::mkptr(
                new semantic::AsyncCall(pos
                                      , util::mkptr(new semantic::Reference(pos, "read"))
                                      , std::move(fargs)
                                      , std::vector<std::string>({ "content" })
                                      , std::move(largs))));
    consq_filter->addArith(pos, util::mkptr(new semantic::Reference(pos, "content")));

    alter_filter->addArith(pos, util::mkptr(new semantic::Reference(pos, "x")));

    filter.addBranch(pos
                   , util::mkptr(new semantic::BoolLiteral(pos, true))
                   , std::move(consq_filter)
                   , std::move(alter_filter));

    filter.addArith(pos, util::mkptr(new semantic::FloatLiteral(pos, "11.04")));

    compile(filter, space.sym())->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (SCOPE_BEGIN)
                (ASYNC_RESULT_DEF)
                    (pos, CALL, 2)
                        (pos, REFERENCE, "read")
                        (pos, FUNCTION, 1)
                            (PARAMETER, "content")
                            (COPY_PARAM_DECL)
                            (SCOPE_BEGIN)
                                (ARITHMETICS)
                                    (pos, ASYNC_REFERENCE)
                                (ARITHMETICS)
                                    (pos, REFERENCE, "content")
                            (SCOPE_END)
                        (pos, STRING, "f20130123")
            (SCOPE_END)
            (ARITHMETICS)
                (pos, FLOATING, "11.04")
        (SCOPE_END)
    ;
}

TEST_F(StmtNodesTest, ReferenceThisInBranch)
{
    misc::position pos(2);
    semantic::CompilingSpace space;
    semantic::Filter filter;

    util::sptr<semantic::Filter> consq_filter(new semantic::Filter);
    util::sptr<semantic::Filter> alter_filter(new semantic::Filter);

    space.sym()->defName(pos, "houjou");
    space.sym()->defName(pos, "ryuuguu");

    consq_filter->addArith(pos, util::mkptr(
                new semantic::Lookup(pos
                                   , util::mkptr(new semantic::This(pos))
                                   , util::mkptr(new semantic::StringLiteral(pos, "maebara")))));
    alter_filter->addArith(pos, util::mkptr(
                new semantic::Call(pos
                                 , util::mkptr(new semantic::Reference(pos, "ryuuguu"))
                                 , util::ptrarr<semantic::Expression const>())));

    filter.addBranch(pos
                   , util::mkptr(new semantic::Reference(pos, "houjou"))
                   , std::move(consq_filter)
                   , std::move(alter_filter));

    compile(filter, space.sym())->write(dummyos());
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
    semantic::Filter filter;

    util::sptr<semantic::Filter> consq_filter(new semantic::Filter);
    util::sptr<semantic::Filter> alter_filter(new semantic::Filter);

    space.sym()->defName(pos, "rena");

    consq_filter->addArith(pos, util::mkptr(
                new semantic::Lookup(pos
                                   , util::mkptr(new semantic::This(pos))
                                   , util::mkptr(new semantic::StringLiteral(pos, "keiiti")))));
    alter_filter->addArith(pos, util::mkptr(
                new semantic::Call(pos
                                 , util::mkptr(new semantic::Reference(pos, "rena"))
                                 , util::ptrarr<semantic::Expression const>())));

    filter.addBranch(pos
                   , util::mkptr(new semantic::BoolLiteral(pos, false))
                   , std::move(consq_filter)
                   , std::move(alter_filter));

    compile(filter, space.sym())->write(dummyos());
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
    semantic::Filter filter;

    util::sptr<semantic::Filter> lambda_filter(new semantic::Filter);

    space.sym()->defName(pos, "satoko");
    space.sym()->defName(pos, "satosi");

    lambda_filter->addArith(pos, util::mkptr(
                new semantic::Lookup(pos
                                   , util::mkptr(new semantic::Reference(pos, "satoko"))
                                   , util::mkptr(new semantic::This(pos)))));

    filter.addArith(pos, util::mkptr(
                new semantic::Lambda(pos, std::vector<std::string>(), lambda_filter->deliver())));

    compile(filter, space.sym())->write(dummyos());
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
    semantic::Filter filter;

    util::sptr<semantic::Filter> lambda_filter(new semantic::Filter);

    space.sym()->defName(pos, "furukawa");
    space.sym()->defName(pos, "okasaki");

    lambda_filter->addArith(pos, util::mkptr(
                new semantic::Lookup(pos
                                   , util::mkptr(new semantic::Reference(pos, "furukawa"))
                                   , util::mkptr(new semantic::StringLiteral(pos, "okasaki")))));

    filter.addArith(pos, util::mkptr(
                new semantic::Lambda(pos, std::vector<std::string>(), lambda_filter->deliver())));
    filter.addArith(pos, util::mkptr(
                new semantic::Lookup(pos
                                   , util::mkptr(new semantic::This(pos))
                                   , util::mkptr(new semantic::Reference(pos, "okasaki")))));

    compile(filter, space.sym())->write(dummyos());
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
    semantic::Filter filter;

    space.sym()->defName(pos, "tomoya");
    space.sym()->defName(pos, "nagisa");

    filter.addArith(pos, util::mkptr(
                new semantic::AsyncCall(pos
                                      , util::mkptr(new semantic::Reference(pos, "nagisa"))
                                      , util::ptrarr<semantic::Expression const>()
                                      , std::vector<std::string>()
                                      , util::ptrarr<semantic::Expression const>())));
    filter.addArith(pos, util::mkptr(
                new semantic::Lookup(pos
                                   , util::mkptr(new semantic::This(pos))
                                   , util::mkptr(new semantic::Reference(pos, "tomoya")))));

    compile(filter, space.sym())->write(dummyos());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (DEC_THIS)
            (ASYNC_RESULT_DEF)
                (pos, CALL, 1)
                    (pos, REFERENCE, "nagisa")
                    (pos, FUNCTION, 0)
                        (COPY_PARAM_DECL)
                        (SCOPE_BEGIN)
                            (ARITHMETICS)
                                (pos, ASYNC_REFERENCE)
                            (ARITHMETICS)
                                (pos, BINARY_OP, "[]")
                                    (pos, THIS)
                                    (pos, REFERENCE, "tomoya")
                        (SCOPE_END)
        (SCOPE_END)
    ;
}
