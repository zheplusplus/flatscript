#include <gtest/gtest.h>

#include <test/phony-errors.h>

#include "test-common.h"
#include "../stmt-nodes.h"
#include "../expr-nodes.h"
#include "../function.h"

using namespace test;

typedef GrammarTest StmtNodesTest;

TEST_F(StmtNodesTest, Arithmetics)
{
    misc::position pos(1);
    util::sptr<semantic::Filter> filter(std::move(mkfilter()));
    grammar::Arithmetics arith0(pos, util::mkptr(new grammar::IntLiteral(pos, "1840")));
    grammar::Arithmetics arith1(pos, util::mkptr(new grammar::BoolLiteral(pos, false)));
    arith0.compile(*filter);
    arith1.compile(*filter);
    filter->deliver().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, INTEGER, "1840")
        (pos, ARITHMETICS)
            (pos, BOOLEAN, "false")
        (BLOCK_END)
    ;
}

TEST_F(StmtNodesTest, NameDef)
{
    misc::position pos(2);
    util::sptr<semantic::Filter> filter(std::move(mkfilter()));
    grammar::NameDef def0(pos, "Shinji", util::mkptr(new grammar::FloatLiteral(pos, "18.47")));
    grammar::NameDef def1(pos, "Asuka", util::mkptr(new grammar::Identifier(pos, "tsundere")));
    def0.compile(*filter);
    def1.compile(*filter);
    filter->deliver().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, NAME_DEF, "Shinji")
            (pos, FLOATING, "18.47")
        (pos, NAME_DEF, "Asuka")
            (pos, IDENTIFIER, "tsundere")
        (BLOCK_END)
    ;
}

TEST_F(StmtNodesTest, Returns)
{
    misc::position pos(3);
    util::sptr<semantic::Filter> filter(std::move(mkfilter()));
    grammar::Return ret0(pos, util::mkptr(new grammar::Identifier(pos, "KaworuNagisa")));
    grammar::ReturnNothing ret1(pos);
    ret0.compile(*filter);
    ret1.compile(*filter);
    filter->deliver().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, RETURN)
            (pos, IDENTIFIER, "KaworuNagisa")
        (pos, RETURN_NOTHING)
        (BLOCK_END)
    ;
}

TEST_F(StmtNodesTest, Block)
{
    misc::position pos(4);
    grammar::Block block;
    block.addStmt(util::mkptr(new grammar::NameDef(
                    pos, "Misato", util::mkptr(new grammar::Identifier(pos, "Katsuragi")))));
    block.addStmt(util::mkptr(new grammar::ReturnNothing(pos)));
    block.compile()->deliver().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, NAME_DEF, "Misato")
            (pos, IDENTIFIER, "Katsuragi")
        (pos, RETURN_NOTHING)
        (BLOCK_END)
    ;
}

TEST_F(StmtNodesTest, Branch)
{
    misc::position pos(6);
    util::sptr<semantic::Filter> filter(std::move(mkfilter()));
    grammar::Branch branch0(pos
                          , util::mkptr(new grammar::BoolLiteral(pos, true))
                          , std::move(grammar::Block()));
    branch0.acceptElse(pos, grammar::Block());
    branch0.compile(*filter);

    grammar::Block block0;
    block0.addStmt(util::mkptr(new grammar::Arithmetics(
                    pos, util::mkptr(new grammar::Identifier(pos, "Kaji")))));
    block0.addStmt(util::mkptr(new grammar::ReturnNothing(pos)));
    grammar::Branch(pos, util::mkptr(new grammar::BoolLiteral(pos, false)), std::move(block0))
        .compile(*filter);

    grammar::Block block1;
    block1.addStmt(util::mkptr(new grammar::Arithmetics(
                            pos, util::mkptr(new grammar::Identifier(pos, "Ryoji")))));
    block1.addStmt(util::mkptr(new grammar::ReturnNothing(pos)));
    grammar::BranchAlterOnly(
                pos, util::mkptr(new grammar::BoolLiteral(pos, true)), std::move(block1))
        .compile(*filter);

    grammar::Block block2;
    block2.addStmt(util::mkptr(new grammar::Arithmetics(
                            pos, util::mkptr(new grammar::IntLiteral(pos, "7")))));
    block2.addStmt(util::mkptr(new grammar::ReturnNothing(pos)));
    grammar::Block block3;
    block3.addStmt(util::mkptr(new grammar::Return(
                            pos, util::mkptr(new grammar::Identifier(pos, "betsuni")))));
    grammar::Branch branch1(pos
                          , util::mkptr(new grammar::BoolLiteral(pos, false))
                          , std::move(block2));
    branch1.acceptElse(pos, std::move(block3));
    branch1.compile(*filter);
    filter->deliver().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, BRANCH)
        (pos, BOOLEAN, "true")
        (CONSEQUENCE)
            (BLOCK_BEGIN)
            (BLOCK_END)
        (ALTERNATIVE)
            (BLOCK_BEGIN)
            (BLOCK_END)

        (pos, BRANCH_CONSQ_ONLY)
        (pos, BOOLEAN, "false")
        (CONSEQUENCE)
            (BLOCK_BEGIN)
            (pos, ARITHMETICS)
                (pos, IDENTIFIER, "Kaji")
            (pos, RETURN_NOTHING)
            (BLOCK_END)

        (pos, BRANCH_ALTER_ONLY)
        (pos, BOOLEAN, "true")
        (ALTERNATIVE)
            (BLOCK_BEGIN)
            (pos, ARITHMETICS)
                (pos, IDENTIFIER, "Ryoji")
            (pos, RETURN_NOTHING)
            (BLOCK_END)

        (pos, BRANCH)
        (pos, BOOLEAN, "false")
        (CONSEQUENCE)
            (BLOCK_BEGIN)
            (pos, ARITHMETICS)
                (pos, INTEGER, "7")
            (pos, RETURN_NOTHING)
            (BLOCK_END)
        (ALTERNATIVE)
            (BLOCK_BEGIN)
            (pos, RETURN)
                (pos, IDENTIFIER, "betsuni")
            (BLOCK_END)
        (BLOCK_END)
    ;
}

TEST_F(StmtNodesTest, Functions)
{
    misc::position pos(8);
    util::sptr<semantic::Filter> filter(std::move(mkfilter()));
    grammar::Function func0(
            pos, "func0", std::vector<std::string>(), -1, std::move(grammar::Block()));
    func0.compile(*filter);

    grammar::Block body;
    body.addStmt(util::mkptr(new grammar::Arithmetics(
                        pos, util::mkptr(new grammar::Identifier(pos, "Kuroi")))));
    body.addStmt(std::move(util::mkptr(new grammar::ReturnNothing(pos))));
    grammar::Function func1(pos
                          , "func1"
                          , std::vector<std::string>({ "Konata", "Kagami", "Tsukasa", "Miyuki" })
                          , -1
                          , std::move(body));
    func1.compile(*filter);
    filter->deliver().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, FUNC_DEF, "func0")
            (BLOCK_BEGIN)
            (BLOCK_END)
        (pos, FUNC_DEF, "func1")
            (pos, PARAMETER, "Konata")
            (pos, PARAMETER, "Kagami")
            (pos, PARAMETER, "Tsukasa")
            (pos, PARAMETER, "Miyuki")
            (BLOCK_BEGIN)
            (pos, ARITHMETICS)
                (pos, IDENTIFIER, "Kuroi")
            (pos, RETURN_NOTHING)
            (BLOCK_END)
        (BLOCK_END)
    ;
}

TEST_F(StmtNodesTest, Mixed)
{
    misc::position pos(9);
    util::sptr<semantic::Filter> filter(std::move(mkfilter()));

    grammar::Block block_nested;
    block_nested.addStmt(util::mkptr(new grammar::Arithmetics(
                            pos, util::mkptr(new grammar::IntLiteral(pos, "9")))));
    util::sptr<grammar::Function> func_nested0(new grammar::Function(
                pos, "funcn", std::vector<std::string>({ "SOS" }), -1, std::move(block_nested)));
    util::sptr<grammar::Function> func_nested1(new grammar::Function(
                pos, "funcn", std::vector<std::string>(), -1, std::move(grammar::Block())));

    grammar::Block body;
    body.addStmt(util::mkptr(new grammar::Arithmetics(
                    pos, util::mkptr(new grammar::Identifier(pos, "Kyon")))));
    body.addFunc(std::move(func_nested0));
    body.addFunc(std::move(func_nested1));
    body.addStmt(std::move(util::mkptr(new grammar::ReturnNothing(pos))));

    grammar::Function func(pos
                         , "funco"
                         , std::vector<std::string>({ "Suzumiya", "Koizumi", "Nagato", "Asahina" })
                         , -1
                         , std::move(body));
    func.compile(*filter);
    filter->deliver().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, FUNC_DEF, "funco")
            (pos, PARAMETER, "Suzumiya")
            (pos, PARAMETER, "Koizumi")
            (pos, PARAMETER, "Nagato")
            (pos, PARAMETER, "Asahina")
            (BLOCK_BEGIN)
            (pos, FUNC_DEF, "funcn")
                (pos, PARAMETER, "SOS")
                (BLOCK_BEGIN)
                (pos, ARITHMETICS)
                    (pos, INTEGER, "9")
                (BLOCK_END)
            (pos, FUNC_DEF, "funcn")
                (BLOCK_BEGIN)
                (BLOCK_END)
            (pos, ARITHMETICS)
                (pos, IDENTIFIER, "Kyon")
            (pos, RETURN_NOTHING)
            (BLOCK_END)
        (BLOCK_END)
    ;
}
