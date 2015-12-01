#include <gtest/gtest.h>

#include <grammar/stmt-nodes.h>
#include <grammar/expr-nodes.h>
#include <grammar/function.h>
#include <test/phony-errors.h>

#include "test-common.h"

using namespace test;

typedef GrammarTest StmtNodesTest;

TEST_F(StmtNodesTest, Arithmetics)
{
    misc::position pos(1);
    semantic::Block block;
    grammar::Arithmetics arith0(pos, util::mkptr(new grammar::IntLiteral(pos, "1840")));
    grammar::Arithmetics arith1(pos, util::mkptr(new grammar::BoolLiteral(pos, false)));
    block.addStmt(arith0.compile());
    block.addStmt(arith1.compile());
    block.compile(nulScope());
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
    semantic::Block block;
    grammar::NameDef def0(pos, "Shinji", util::mkptr(new grammar::FloatLiteral(pos, "18.47")));
    grammar::NameDef def1(pos, "Asuka", util::mkptr(new grammar::Identifier(pos, "tsundere")));
    block.addStmt(def0.compile());
    block.addStmt(def1.compile());
    block.compile(nulScope());
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
    misc::position pos_a(300);
    misc::position pos_b(301);
    semantic::Block block;
    grammar::Return ret0(pos, util::mkptr(new grammar::Identifier(pos_a, "KaworuNagisa")));
    grammar::Return ret1(pos, util::mkptr(new grammar::EmptyExpr(pos_b)));
    block.addStmt(ret0.compile());
    block.addStmt(ret1.compile());
    block.compile(nulScope());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, RETURN)
            (pos_a, IDENTIFIER, "KaworuNagisa")
        (pos, RETURN)
            (pos_b, UNDEFINED)
        (BLOCK_END)
    ;
}

TEST_F(StmtNodesTest, Block)
{
    misc::position pos(4);
    grammar::Block block;
    block.addStmt(util::mkptr(new grammar::NameDef(
                    pos, "Misato", util::mkptr(new grammar::Identifier(pos, "Katsuragi")))));
    block.compile().compile(nulScope());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, NAME_DEF, "Misato")
            (pos, IDENTIFIER, "Katsuragi")
        (BLOCK_END)
    ;
}

TEST_F(StmtNodesTest, Branch)
{
    misc::position pos(6);
    semantic::Block block;
    grammar::Branch branch0(pos
                          , util::mkptr(new grammar::BoolLiteral(pos, true))
                          , std::move(grammar::Block()));
    branch0.acceptElse(pos, grammar::Block());
    block.addStmt(branch0.compile());

    grammar::Block block0;
    block0.addStmt(util::mkptr(new grammar::Arithmetics(
                    pos, util::mkptr(new grammar::Identifier(pos, "Kaji")))));
    block.addStmt(grammar::Branch(
                pos, util::mkptr(new grammar::BoolLiteral(pos, false)), std::move(block0))
            .compile());

    grammar::Block block1;
    block1.addStmt(util::mkptr(new grammar::Arithmetics(
                            pos, util::mkptr(new grammar::Identifier(pos, "Ryoji")))));
    block.addStmt(grammar::BranchAlterOnly(
                pos, util::mkptr(new grammar::BoolLiteral(pos, true)), std::move(block1))
            .compile());

    grammar::Block block2;
    block2.addStmt(util::mkptr(new grammar::Arithmetics(
                            pos, util::mkptr(new grammar::IntLiteral(pos, "7")))));
    grammar::Block block3;
    block3.addStmt(util::mkptr(new grammar::Return(
                            pos, util::mkptr(new grammar::Identifier(pos, "betsuni")))));
    grammar::Branch branch1(pos
                          , util::mkptr(new grammar::BoolLiteral(pos, false))
                          , std::move(block2));
    branch1.acceptElse(pos, std::move(block3));
    block.addStmt(branch1.compile());
    block.compile(nulScope());
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

        (pos, BRANCH)
        (pos, BOOLEAN, "false")
        (CONSEQUENCE)
            (BLOCK_BEGIN)
            (pos, ARITHMETICS)
                (pos, IDENTIFIER, "Kaji")
            (BLOCK_END)
        (ALTERNATIVE)
            (BLOCK_BEGIN)
            (BLOCK_END)

        (pos, BRANCH)
        (pos, BOOLEAN, "true")
        (CONSEQUENCE)
            (BLOCK_BEGIN)
            (BLOCK_END)
        (ALTERNATIVE)
            (BLOCK_BEGIN)
            (pos, ARITHMETICS)
                (pos, IDENTIFIER, "Ryoji")
            (BLOCK_END)

        (pos, BRANCH)
        (pos, BOOLEAN, "false")
        (CONSEQUENCE)
            (BLOCK_BEGIN)
            (pos, ARITHMETICS)
                (pos, INTEGER, "7")
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
    semantic::Block block;
    grammar::Function func0(
            pos, "func0", std::vector<std::string>(), -1, std::move(grammar::Block()));
    block.addFunc(func0.compile());

    grammar::Block body;
    body.addStmt(util::mkptr(new grammar::Arithmetics(
                        pos, util::mkptr(new grammar::Identifier(pos, "Kuroi")))));
    grammar::Function func1(pos
                          , "func1"
                          , std::vector<std::string>({ "Konata", "Kagami", "Tsukasa", "Miyuki" })
                          , -1
                          , std::move(body));
    block.addFunc(func1.compile());
    block.compile(nulScope());
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
            (BLOCK_END)
        (BLOCK_END)
    ;
}

TEST_F(StmtNodesTest, Mixed)
{
    misc::position pos(9);
    semantic::Block block;

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

    grammar::Function func(pos
                         , "funco"
                         , std::vector<std::string>({ "Suzumiya", "Koizumi", "Nagato", "Asahina" })
                         , -1
                         , std::move(body));
    block.addFunc(func.compile());
    block.compile(nulScope());
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
            (BLOCK_END)
        (BLOCK_END)
    ;
}
