#include <gtest/gtest.h>

#include <flowcheck/expr-nodes.h>
#include <flowcheck/function.h>
#include <test/common.h>
#include <test/phony-errors.h>

#include "test-common.h"
#include "../acceptor.h"
#include "../function.h"
#include "../stmt-nodes.h"

using namespace test;

struct TestAcceptor
    : public grammar::Acceptor
{
    TestAcceptor()
        : grammar::Acceptor(misc::position())
        , stmt_or_nul_if_not_set(nullptr)
        , func_or_nul_if_not_set(nullptr)
        , filter(mkfilter())
    {}

    void acceptStmt(util::sptr<grammar::Statement> s)
    {
        stmt_or_nul_if_not_set = std::move(s);
    }

    void acceptFunc(util::sptr<grammar::Function const> f)
    {
        func_or_nul_if_not_set = std::move(f);
    }

    void compile()
    {
        grammar::Block Block;
        if (stmt_or_nul_if_not_set.not_nul()) {
            Block.addStmt(std::move(stmt_or_nul_if_not_set));
        }
        if (func_or_nul_if_not_set.not_nul()) {
            Block.addFunc(std::move(func_or_nul_if_not_set));
        }
        filter = std::move(Block.compile(std::move(filter)));
    }

    util::sptr<grammar::Statement> stmt_or_nul_if_not_set;
    util::sptr<grammar::Function const> func_or_nul_if_not_set;
    util::sptr<flchk::Filter> filter;

    void deliverTo(util::sref<grammar::Acceptor>) {}
};

typedef GrammarTest AcceptorTest;

TEST_F(AcceptorTest, IfAcceptor)
{
    misc::position pos(1);
    misc::position pos_head(100);
    misc::position pos_else(101);
    TestAcceptor receiver;

    grammar::IfAcceptor acceptor_a(pos_head, util::mkptr(new flchk::IntLiteral(pos_head, "0")));
    acceptor_a.acceptStmt(util::mkptr(new grammar::ReturnNothing(pos)));

    acceptor_a.acceptElse(pos_else);
    ASSERT_FALSE(error::hasError());
    acceptor_a.acceptStmt(util::mkptr(new grammar::NameDef(
                    pos, "Hyperion", util::mkptr(new flchk::Reference(pos, "Raynor")))));

    acceptor_a.deliverTo(util::mkref(receiver));
    ASSERT_TRUE(receiver.stmt_or_nul_if_not_set.not_nul());
    ASSERT_TRUE(receiver.func_or_nul_if_not_set.nul());
    receiver.compile();

    grammar::IfAcceptor acceptor_b(pos_head
                                 , util::mkptr(new flchk::IntLiteral(pos_head, "1")));
    acceptor_b.acceptStmt(util::mkptr(new grammar::Return(
                    pos, util::mkptr(new flchk::Reference(pos, "Karrigan")))));

    acceptor_b.deliverTo(util::mkref(receiver));
    ASSERT_TRUE(receiver.stmt_or_nul_if_not_set.not_nul());
    ASSERT_TRUE(receiver.func_or_nul_if_not_set.nul());
    receiver.compile();

    grammar::IfAcceptor acceptor_c(pos_head, util::mkptr(new flchk::IntLiteral(pos_head, "2")));
    acceptor_c.acceptStmt(util::mkptr(new grammar::Arithmetics(
                    pos, util::mkptr(new flchk::BoolLiteral(pos, false)))));
    acceptor_c.acceptElse(pos_else);
    ASSERT_FALSE(error::hasError());

    acceptor_c.deliverTo(util::mkref(receiver));
    ASSERT_TRUE(receiver.stmt_or_nul_if_not_set.not_nul());
    ASSERT_TRUE(receiver.func_or_nul_if_not_set.nul());
    receiver.compile();

    grammar::IfAcceptor acceptor_d(pos_head, util::mkptr(new flchk::IntLiteral(pos_head, "3")));
    acceptor_d.acceptElse(pos_else);
    ASSERT_FALSE(error::hasError());
    acceptor_d.acceptStmt(util::mkptr(new grammar::Arithmetics(
                    pos, util::mkptr(new flchk::FloatLiteral(pos, "20.54")))));

    acceptor_d.deliverTo(util::mkref(receiver));
    ASSERT_TRUE(receiver.stmt_or_nul_if_not_set.not_nul());
    ASSERT_TRUE(receiver.func_or_nul_if_not_set.nul());
    receiver.compile();
    receiver.filter->compile();

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos_head, BRANCH)
        (pos_head, INTEGER, "0")
        (CONSEQUENCE)
            (BLOCK_BEGIN)
            (pos, RETURN_NOTHING)
            (BLOCK_END)
        (ALTERNATIVE)
            (BLOCK_BEGIN)
            (pos, NAME_DEF, "Hyperion" + NAME_DEF_FILTERED)
                (pos, IDENTIFIER, "Raynor")
            (BLOCK_END)

        (pos_head, BRANCH_CONSQ_ONLY)
        (pos_head, INTEGER, "1")
        (CONSEQUENCE)
            (BLOCK_BEGIN)
            (pos, RETURN)
                (pos, IDENTIFIER, "Karrigan")
            (BLOCK_END)

        (pos_head, BRANCH)
        (pos_head, INTEGER, "2")
        (CONSEQUENCE)
            (BLOCK_BEGIN)
            (pos, ARITHMETICS)
                (pos, BOOLEAN, "false")
            (BLOCK_END)
        (ALTERNATIVE)
            (BLOCK_BEGIN)
            (BLOCK_END)

        (pos_head, BRANCH)
        (pos_head, INTEGER, "3")
        (CONSEQUENCE)
            (BLOCK_BEGIN)
            (BLOCK_END)
        (ALTERNATIVE)
            (BLOCK_BEGIN)
            (pos, ARITHMETICS)
                (pos, FLOATING, "20.54")
            (BLOCK_END)
        (BLOCK_END)
    ;
    ASSERT_FALSE(error::hasError());
}

TEST_F(AcceptorTest, IfAcceptorError)
{
    misc::position pos(2);
    misc::position pos_head(200);
    misc::position pos_else(201);
    grammar::IfAcceptor acceptor_a(pos_head, util::mkptr(new flchk::IntLiteral(pos_head, "0")));
    acceptor_a.acceptElse(pos);
    ASSERT_FALSE(error::hasError());
    acceptor_a.acceptElse(pos_else);
    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getIfMatchedRecs().size());
    ASSERT_EQ(pos, getIfMatchedRecs()[0].prev_pos);
    ASSERT_EQ(pos_else, getIfMatchedRecs()[0].this_pos);
}

TEST_F(AcceptorTest, IfNotAcceptor)
{
    misc::position pos(3);
    TestAcceptor receiver;

    grammar::IfnotAcceptor ifnot_acc0(pos, util::mkptr(new flchk::BoolLiteral(pos, false)));
    ifnot_acc0.acceptStmt(util::mkptr(new grammar::NameDef(
                    pos, "SCV", util::mkptr(new flchk::IntLiteral(pos, "60")))));
    ifnot_acc0.acceptStmt(util::mkptr(new grammar::Arithmetics(
                    pos, util::mkptr(new flchk::Reference(pos, "Marine")))));

    ifnot_acc0.deliverTo(util::mkref(receiver));
    ASSERT_TRUE(receiver.stmt_or_nul_if_not_set.not_nul());
    ASSERT_TRUE(receiver.func_or_nul_if_not_set.nul());
    receiver.compile();
    receiver.filter->compile();

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, BRANCH_ALTER_ONLY)
        (pos, BOOLEAN, "false")
        (ALTERNATIVE)
            (BLOCK_BEGIN)
            (pos, NAME_DEF, "SCV" + NAME_DEF_FILTERED)
                (pos, INTEGER, "60")
            (pos, ARITHMETICS)
                (pos, IDENTIFIER, "Marine")
            (BLOCK_END)
        (BLOCK_END)
    ;
    ASSERT_FALSE(error::hasError());

    misc::position pos_else(20);
    grammar::IfnotAcceptor ifnot_acc1(pos, util::mkptr(new flchk::BoolLiteral(pos, true)));
    ifnot_acc1.acceptElse(pos_else);
    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getElseNotMatches().size());
    ASSERT_EQ(pos_else, getElseNotMatches()[0].pos);
}

TEST_F(AcceptorTest, FuncAcceptor)
{
    misc::position pos(5);
    TestAcceptor receiver;

    grammar::FunctionAcceptor func_acc0(
                        pos, "func1", std::vector<std::string>({ "Duke", "Duran" }));
    func_acc0.acceptStmt(util::mkptr(new grammar::Arithmetics(
                        pos, util::mkptr(new flchk::FloatLiteral(pos, "21.37")))));
    func_acc0.acceptStmt(util::mkptr(new grammar::NameDef(
                        pos, "SonOfKorhal", util::mkptr(new flchk::IntLiteral(pos, "20110116")))));

    func_acc0.deliverTo(util::mkref(receiver));
    ASSERT_TRUE(receiver.stmt_or_nul_if_not_set.nul());
    ASSERT_TRUE(receiver.func_or_nul_if_not_set.not_nul());
    receiver.compile();
    receiver.filter->compile();

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, FUNC_DEF, "func1")
            (pos, PARAMETER, "Duke")
            (pos, PARAMETER, "Duran")
            (BLOCK_BEGIN)
            (pos, ARITHMETICS)
                (pos, FLOATING, "21.37")
            (pos, NAME_DEF, "SonOfKorhal")
                (pos, INTEGER, "20110116")
            (BLOCK_END)
        (BLOCK_END)
    ;
    ASSERT_FALSE(error::hasError());

    misc::position pos_else(10);
    grammar::FunctionAcceptor func_acc1(pos, "func2", std::vector<std::string>({ "Mengsk" }));
    func_acc1.acceptElse(pos_else);
    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getElseNotMatches().size());
    ASSERT_EQ(pos_else, getElseNotMatches()[0].pos);
}

TEST_F(AcceptorTest, FuncAccNested)
{
    misc::position pos(5);
    TestAcceptor receiver;

    grammar::FunctionAcceptor func_acc0(pos
                                      , "funca", std::vector<std::string>({ "firebat", "ghost" }));
    func_acc0.acceptStmt(util::mkptr(new grammar::Arithmetics(pos, util::mkptr(
                                                new flchk::FloatLiteral(pos, "22.15")))));
    func_acc0.acceptStmt(util::mkptr(new grammar::NameDef(pos, "medic", util::mkptr(
                                                new flchk::Reference(pos, "wraith")))));

    grammar::FunctionAcceptor func_acc1(pos, "funca", std::vector<std::string>({ "vulture" }));
    func_acc1.acceptStmt(util::mkptr(new grammar::Arithmetics(pos, util::mkptr(
                                                new flchk::Reference(pos, "goliath")))));

    func_acc1.deliverTo(util::mkref(func_acc0));
    func_acc0.deliverTo(util::mkref(receiver));
    ASSERT_TRUE(receiver.stmt_or_nul_if_not_set.nul());
    ASSERT_TRUE(receiver.func_or_nul_if_not_set.not_nul());
    receiver.compile();
    receiver.filter->compile();

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, FUNC_DEF, "funca")
            (pos, PARAMETER, "firebat")
            (pos, PARAMETER, "ghost")
            (BLOCK_BEGIN)
            (pos, FUNC_DEF, "funca")
                (pos, PARAMETER, "vulture")
                (BLOCK_BEGIN)
                (pos, ARITHMETICS)
                    (pos, IDENTIFIER, "goliath")
                (BLOCK_END)
            (pos, ARITHMETICS)
                (pos, FLOATING, "22.15")
            (pos, NAME_DEF, "medic")
                (pos, IDENTIFIER, "wraith")
            (BLOCK_END)
        (BLOCK_END)
    ;
    ASSERT_FALSE(error::hasError());
}
