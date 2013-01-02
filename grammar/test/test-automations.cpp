#include <gtest/gtest.h>

#include <util/string.h>
#include <misc/const.h>
#include <test/phony-errors.h>

#include "test-common.h"
#include "../expr-automations.h"
#include "../stmt-automations.h"

using namespace test;

struct AutomationTest
    : GrammarTest
{
    AutomationTest()
        : stack(nullptr)
    {}

    void SetUp()
    {
        stkptr = new grammar::AutomationStack;
        stack.reset(stkptr);
        GrammarTest::SetUp();
    }

    void pushBoolean(misc::position const& pos, bool value)
    {
        stack->top()->pushFactor(
                    *stkptr, util::mkptr(new grammar::BoolLiteral(pos, value)), util::str(value));
    }

    void pushInteger(misc::position const& pos, std::string const& image)
    {
        stack->top()->pushFactor(*stkptr, util::mkptr(new grammar::IntLiteral(pos, image)), image);
    }

    void pushIdent(misc::position const& pos, std::string const& image)
    {
        stack->top()->pushFactor(*stkptr, util::mkptr(new grammar::Identifier(pos, image)), image);
    }

    void pushPipeElement(misc::position const& pos)
    {
        stack->top()->pushFactor(*stkptr, util::mkptr(new grammar::PipeElement(pos)), "$");
    }

    void finish(misc::position const& pos)
    {
        std::vector<util::sptr<grammar::ClauseBase>> clauses;
        grammar::ClauseStackWrapper wrapper(0, *stkptr, pos, clauses);
        stack->top()->finish(wrapper, stackref(), pos);
    }

    grammar::AutomationStack& stackref()
    {
        return *stkptr;
    }

    util::sptr<grammar::AutomationStack> stack;
    grammar::AutomationStack* stkptr;
};

struct TestToken
    : grammar::Token
{
    TestToken(misc::position const& pos, std::string const& image)
        : grammar::Token(pos, image)
    {}

    void act(grammar::AutomationStack&) {}
};

TEST_F(AutomationTest, ReduceArithExpression)
{
    misc::position pos(1);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushInteger(pos, "20121104");
    stack->top()->pushOp(stackref(), TestToken(pos, "+"));
    stack->top()->pushOp(stackref(), TestToken(pos, "-"));
    pushInteger(pos, "1349");
    stack->top()->pushOp(stackref(), TestToken(pos, "*"));
    pushIdent(pos, "modokasii");
    stack->top()->pushOp(stackref(), TestToken(pos, ">"));
    pushIdent(pos, "seikaino");
    stack->top()->pushOp(stackref(), TestToken(pos, "-"));
    pushIdent(pos, "uede");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());
    ASSERT_TRUE(stack->empty());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, ">")
            (pos, OPERAND)
                (pos, BINARY_OP, "+")
                (pos, OPERAND)
                    (pos, INTEGER, "20121104")
                (pos, OPERAND)
                    (pos, BINARY_OP, "*")
                    (pos, OPERAND)
                        (pos, PRE_UNARY_OP, "-")
                            (pos, OPERAND)
                            (pos, INTEGER, "1349")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "modokasii")
            (pos, OPERAND)
                (pos, BINARY_OP, "-")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "seikaino")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "uede")
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceLogicExpression)
{
    misc::position pos(2);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "starlight");
    stack->top()->pushOp(stackref(), TestToken(pos, "<"));
    stack->top()->pushOp(stackref(), TestToken(pos, "!"));
    pushBoolean(pos, false);
    stack->top()->pushOp(stackref(), TestToken(pos, "&&"));
    pushIdent(pos, "vision");
    stack->top()->pushOp(stackref(), TestToken(pos, ">"));
    pushIdent(pos, "eternal");
    stack->top()->pushOp(stackref(), TestToken(pos, "||"));
    pushIdent(pos, "rite");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());
    ASSERT_TRUE(stack->empty());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, "||")
            (pos, OPERAND)
                (pos, BINARY_OP, "&&")
                (pos, OPERAND)
                    (pos, BINARY_OP, "<")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "starlight")
                    (pos, OPERAND)
                        (pos, PRE_UNARY_OP, "!")
                            (pos, OPERAND)
                            (pos, BOOLEAN, "false")
                (pos, OPERAND)
                    (pos, BINARY_OP, ">")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "vision")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "eternal")
            (pos, OPERAND)
                (pos, IDENTIFIER, "rite")
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReducePipeExpression)
{
    misc::position pos(3);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "kenji");
    stack->top()->pushOp(stackref(), TestToken(pos, "++"));
    pushIdent(pos, "kawai");
    stack->top()->pushOp(stackref(), TestToken(pos, "|?"));
    pushIdent(pos, "kugutu");
    stack->top()->pushOp(stackref(), TestToken(pos, "<"));
    pushIdent(pos, "uta");
    stack->top()->pushOp(stackref(), TestToken(pos, "|:"));
    pushPipeElement(pos);
    stack->top()->pushOp(stackref(), TestToken(pos, "*"));
    pushPipeElement(pos);
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());
    ASSERT_TRUE(stack->empty());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, int(cons::MAP))
            (pos, OPERAND)
                (pos, BINARY_OP, int(cons::FILTER))
                (pos, OPERAND)
                    (pos, BINARY_OP, "[ ++ ]")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "kenji")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "kawai")
                (pos, OPERAND)
                    (pos, BINARY_OP, "<")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "kugutu")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "uta")
            (pos, OPERAND)
                (pos, BINARY_OP, "*")
                (pos, OPERAND)
                    (pos, PIPE_ELEMENT)
                (pos, OPERAND)
                    (pos, PIPE_ELEMENT)
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceNameDef)
{
    misc::position pos(4);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "Zzz");
    stack->top()->pushColon(stackref(), pos);
    stack->top()->pushOp(stackref(), TestToken(pos, "!"));
    pushIdent(pos, "sasaki");
    stack->top()->pushOp(stackref(), TestToken(pos, "&&"));
    pushIdent(pos, "nitijou");
    stack->top()->pushOp(stackref(), TestToken(pos, "="));
    pushIdent(pos, "nano");
    stack->top()->pushOp(stackref(), TestToken(pos, "+"));
    pushIdent(pos, "mio");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());
    ASSERT_TRUE(stack->empty());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, NAME_DEF, "Zzz")
            (pos, BINARY_OP, "&&")
            (pos, OPERAND)
                (pos, PRE_UNARY_OP, "!")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "sasaki")
            (pos, OPERAND)
                (pos, BINARY_OP, "=")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "nitijou")
                (pos, OPERAND)
                    (pos, BINARY_OP, "+")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "nano")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "mio")
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceCallExpression)
{
    misc::position pos(5);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "masayosi");
    stack->top()->pushOp(stackref(), TestToken(pos, "+"));
    pushIdent(pos, "yamazaki");
    stack->top()->pushOp(stackref(), TestToken(pos, "."));
    pushIdent(pos, "one");
    stack->top()->pushOpenParen(stackref(), pos);
    pushIdent(pos, "more");
    stack->top()->pushComma(stackref(), pos);
    pushIdent(pos, "time");
    stack->top()->pushOp(stackref(), TestToken(pos, "&&"));
    pushIdent(pos, "chance");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());
    ASSERT_TRUE(stack->empty());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, "+")
            (pos, OPERAND)
                (pos, IDENTIFIER, "masayosi")
            (pos, OPERAND)
                (pos, CALL_BEGIN)
                    (pos, BINARY_OP, "[ . ]")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "yamazaki")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "one")
                (pos, ARGUMENTS)
                    (pos, IDENTIFIER, "more")
                    (pos, BINARY_OP, "&&")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "time")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "chance")
                (pos, CALL_END)
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceSubExpression)
{
    misc::position pos(6);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "sasahara");
    stack->top()->pushOp(stackref(), TestToken(pos, "*"));
    stack->top()->pushOpenParen(stackref(), pos);
    pushIdent(pos, "naganohara");
    stack->top()->pushOp(stackref(), TestToken(pos, "+"));
    pushIdent(pos, "sinonome");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->pushOp(stackref(), TestToken(pos, "."));
    pushIdent(pos, "sakamoto");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());
    ASSERT_TRUE(stack->empty());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, "*")
            (pos, OPERAND)
                (pos, IDENTIFIER, "sasahara")
            (pos, OPERAND)
                (pos, BINARY_OP, "[ . ]")
                (pos, OPERAND)
                    (pos, CALL_BEGIN)
                        (pos, BINARY_OP, "+")
                        (pos, OPERAND)
                            (pos, IDENTIFIER, "naganohara")
                        (pos, OPERAND)
                            (pos, IDENTIFIER, "sinonome")
                    (pos, ARGUMENTS)
                    (pos, CALL_END)
                (pos, OPERAND)
                    (pos, IDENTIFIER, "sakamoto")
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceAnonyFunc)
{
    misc::position pos(7);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->pushOpenParen(stackref(), pos);
    pushIdent(pos, "aioi");
    stack->top()->pushComma(stackref(), pos);
    pushIdent(pos, "minokami");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->pushColon(stackref(), pos);
    pushIdent(pos, "aioi");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->pushOpenParen(stackref(), pos);
    pushBoolean(pos, false);
    stack->top()->pushComma(stackref(), pos);
    pushInteger(pos, "20121106");
    stack->top()->pushComma(stackref(), pos);
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    clause.filter->compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, CALL_BEGIN)
                (pos, FUNC_DEF)
                    (pos, PARAMETER, "aioi")
                    (pos, PARAMETER, "minokami")
                    (BLOCK_BEGIN)
                        (pos, RETURN)
                            (pos, IDENTIFIER, "aioi")
                    (BLOCK_END)
                (pos, ARGUMENTS)
                    (pos, BOOLEAN, "false")
                    (pos, INTEGER, "20121106")
            (pos, CALL_END)
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceDefineAnonyFunc)
{
    misc::position pos(8);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "nakanojou");
    stack->top()->pushColon(stackref(), pos);
    stack->top()->pushOpenParen(stackref(), pos);
    pushIdent(pos, "nakamura");
    stack->top()->pushComma(stackref(), pos);
    pushIdent(pos, "takasaki");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->pushColon(stackref(), pos);
    pushIdent(pos, "sakurai");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    clause.filter->compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, NAME_DEF, "nakanojou")
            (pos, FUNC_DEF)
                (pos, PARAMETER, "nakamura")
                (pos, PARAMETER, "takasaki")
                (BLOCK_BEGIN)
                    (pos, RETURN)
                        (pos, IDENTIFIER, "sakurai")
                (BLOCK_END)
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceLookupSlice)
{
    misc::position pos(9);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "hakase");
    stack->top()->pushOpenBracket(stackref(), pos);
    pushIdent(pos, "minokami");
    stack->top()->pushComma(stackref(), pos);
    stack->top()->pushComma(stackref(), pos);
    pushIdent(pos, "aioi");
    stack->top()->matchClosing(stackref(), TestToken(pos, "]"));
    stack->top()->pushOpenBracket(stackref(), pos);
    pushIdent(pos, "naganohara");
    stack->top()->matchClosing(stackref(), TestToken(pos, "]"));
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    clause.filter->compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, "[]")
            (pos, OPERAND)
                (pos, LIST_SLICE_BEGIN)
                    (pos, IDENTIFIER, "hakase")
                    (pos, IDENTIFIER, "minokami")
                    (pos, LIST_SLICE_DEFAULT)
                    (pos, IDENTIFIER, "aioi")
                (pos, LIST_SLICE_END)
            (pos, OPERAND)
                (pos, IDENTIFIER, "naganohara")
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceAnonyFuncAsArg)
{
    misc::position pos(10);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "tanaka");
    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->pushOpenParen(stackref(), pos);
    pushIdent(pos, "annaka");
    stack->top()->pushComma(stackref(), pos);
    pushIdent(pos, "tatibana");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->pushColon(stackref(), pos);
    pushIdent(pos, "fechan");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    clause.filter->compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, CALL_BEGIN)
                (pos, IDENTIFIER, "tanaka")
            (pos, ARGUMENTS)
                (pos, FUNC_DEF)
                    (pos, PARAMETER, "annaka")
                    (pos, PARAMETER, "tatibana")
                    (BLOCK_BEGIN)
                        (pos, RETURN)
                            (pos, IDENTIFIER, "fechan")
                    (BLOCK_END)
            (pos, CALL_END)
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceListLiteral)
{
    misc::position pos(11);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    stack->top()->pushOpenBracket(stackref(), pos);
    pushIdent(pos, "daiku");
    stack->top()->pushComma(stackref(), pos);
    pushIdent(pos, "sekiguti");
    stack->top()->pushComma(stackref(), pos);
    stack->top()->matchClosing(stackref(), TestToken(pos, "]"));
    stack->top()->pushOp(stackref(), TestToken(pos, "++"));
    ASSERT_FALSE(stack->top()->finishOnBreak(true));
    stack->top()->pushOpenBracket(stackref(), pos);
    pushIdent(pos, "ogi");
    stack->top()->matchClosing(stackref(), TestToken(pos, "]"));
    stack->top()->pushOpenBracket(stackref(), pos);
    pushIdent(pos, "sakurai");
    stack->top()->matchClosing(stackref(), TestToken(pos, "]"));
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    clause.filter->compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, "[ ++ ]")
            (pos, OPERAND)
                (pos, LIST_BEGIN)
                    (pos, IDENTIFIER, "daiku")
                    (pos, IDENTIFIER, "sekiguti")
                (pos, LIST_END)
            (pos, OPERAND)
                (pos, BINARY_OP, "[]")
                (pos, OPERAND)
                    (pos, LIST_BEGIN)
                        (pos, IDENTIFIER, "ogi")
                    (pos, LIST_END)
                (pos, OPERAND)
                    (pos, IDENTIFIER, "sakurai")
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceDictionary)
{
    misc::position pos(12);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "hikari");
    stack->top()->pushOpenParen(stackref(), pos);
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    stack->top()->pushOpenBrace(stackref(), pos);
    stack->top()->matchClosing(stackref(), TestToken(pos, "}"));
    stack->top()->pushComma(stackref(), pos);
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    stack->top()->pushOpenBrace(stackref(), pos);
    pushIdent(pos, "no");
    stack->top()->pushPropertySeparator(stackref(), pos);
    stack->top()->pushOpenParen(stackref(), pos);
    pushIdent(pos, "miyako");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->pushColon(stackref(), pos);
    pushIdent(pos, "miyako");
    stack->top()->matchClosing(stackref(), TestToken(pos, "}"));
    stack->top()->pushComma(stackref(), pos);
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    stack->top()->pushOpenBrace(stackref(), pos);
    pushIdent(pos, "stella");
    stack->top()->pushColon(stackref(), pos);
    pushIdent(pos, "musica");
    stack->top()->pushComma(stackref(), pos);
    stack->top()->matchClosing(stackref(), TestToken(pos, "}"));
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    clause.filter->compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, CALL_BEGIN)
                (pos, IDENTIFIER, "hikari")
            (pos, ARGUMENTS)
                (pos, DICT_BEGIN)
                (pos, DICT_END)
                (pos, DICT_BEGIN)
                (pos, DICT_ITEM)
                    (pos, IDENTIFIER, "no")
                    (pos, FUNC_DEF)
                        (pos, PARAMETER, "miyako")
                        (BLOCK_BEGIN)
                            (pos, RETURN)
                                (pos, IDENTIFIER, "miyako")
                        (BLOCK_END)
                (pos, DICT_END)
                (pos, DICT_BEGIN)
                (pos, DICT_ITEM)
                    (pos, STRING, "stella")
                    (pos, IDENTIFIER, "musica")
                (pos, DICT_END)
            (pos, CALL_END)
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceInvalidNameDef)
{
    misc::position pos(13);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "am1141");
    stack->top()->pushOp(stackref(), TestToken(pos, "."));
    stack->top()->pushOpenParen(stackref(), pos);
    pushInteger(pos, "20121110");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->pushOp(stackref(), TestToken(pos, "+"));
    pushIdent(pos, "ohayou");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    ASSERT_TRUE(error::hasError());

    std::vector<InvalidNameRec> recs(getInvalidNameRecs());
    ASSERT_EQ(1, recs.size());
    ASSERT_EQ(pos, recs[0].pos);
}

TEST_F(AutomationTest, ReduceInvalidLeftValue)
{
    misc::position pos(14);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushInteger(pos, "20121110");
    stack->top()->pushColon(stackref(), pos);
    pushIdent(pos, "no_uta");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    ASSERT_TRUE(error::hasError());

    std::vector<InvalidLeftValueRec> recs(getInvalidLeftValueRecs());
    ASSERT_EQ(1, recs.size());
    ASSERT_EQ(pos, recs[0].pos);
}

TEST_F(AutomationTest, ReduceInvalidLambdaParams)
{
    misc::position pos(15);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    stack->top()->pushOpenParen(stackref(), pos);
    pushInteger(pos, "20121110");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->pushColon(stackref(), pos);
    pushIdent(pos, "no_uta");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    ASSERT_TRUE(error::hasError());

    std::vector<InvalidNameRec> recs(getInvalidNameRecs());
    ASSERT_EQ(1, recs.size());
    ASSERT_EQ(pos, recs[0].pos);
}

TEST_F(AutomationTest, UnexpectedBinaryOp)
{
    misc::position pos(16);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    stack->top()->pushOp(stackref(), TestToken(pos, "."));
    pushInteger(pos, "1300");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    ASSERT_TRUE(error::hasError());

    std::vector<UnexpectedTokenRec> recs(getUnexpectedTokenRecs());
    ASSERT_EQ(1, recs.size());
    ASSERT_EQ(pos, recs[0].pos);
    ASSERT_EQ(".", recs[0].image);
}

TEST_F(AutomationTest, UnexpectedPreUnaryOp)
{
    misc::position pos(17);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushInteger(pos, "1300");
    stack->top()->pushOp(stackref(), TestToken(pos, "!"));
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    ASSERT_TRUE(error::hasError());

    std::vector<UnexpectedTokenRec> recs(getUnexpectedTokenRecs());
    ASSERT_EQ(1, recs.size());
    ASSERT_EQ(pos, recs[0].pos);
    ASSERT_EQ("!", recs[0].image);
}

TEST_F(AutomationTest, UnexpectedDictionaryFinished)
{
    misc::position pos(18);
    misc::position pos_a(1800);
    misc::position pos_b(1801);
    misc::position pos_c(1802);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    stack->top()->pushOpenBrace(stackref(), pos);
    pushInteger(pos, "1300");
    stack->top()->matchClosing(stackref(), TestToken(pos_a, "}"));

    stack->top()->pushOpenBracket(stackref(), pos);
    stack->top()->pushOpenBrace(stackref(), pos);
    pushInteger(pos, "1300");
    stack->top()->pushComma(stackref(), pos_b);
    stack->top()->matchClosing(stackref(), TestToken(pos_c, "}"));
    stack->top()->matchClosing(stackref(), TestToken(pos, "]"));

    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    ASSERT_TRUE(error::hasError());

    std::vector<UnexpectedTokenRec> recs(getUnexpectedTokenRecs());
    ASSERT_EQ(3, recs.size());
    ASSERT_EQ(pos_a, recs[0].pos);
    ASSERT_EQ("}", recs[0].image);
    ASSERT_EQ(pos_b, recs[1].pos);
    ASSERT_EQ(",", recs[1].image);
    ASSERT_EQ(pos_c, recs[2].pos);
    ASSERT_EQ("}", recs[2].image);
}

TEST_F(AutomationTest, UnexpectedSliceSeperator)
{
    misc::position pos(19);
    misc::position pos_a(1900);
    misc::position pos_b(1901);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "kuurei");

    stack->top()->pushOpenBracket(stackref(), pos);
    stack->top()->pushColon(stackref(), pos_a);
    pushInteger(pos_b, "1300");
    stack->top()->matchClosing(stackref(), TestToken(pos, "]"));

    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    ASSERT_TRUE(error::hasError());

    std::vector<UnexpectedTokenRec> urecs(getUnexpectedTokenRecs());
    ASSERT_EQ(urecs.size(), 2);
    ASSERT_EQ(urecs[0].pos, pos_a);
    ASSERT_EQ(urecs[0].image, ":");
    ASSERT_EQ(urecs[1].pos, pos_b);
    ASSERT_EQ(urecs[1].image, "1300");

    std::vector<EmptyLookupKeyRec> erecs(getEmptyLookupKeyRecs());
    ASSERT_EQ(erecs.size(), 1);
    ASSERT_EQ(erecs[0].pos, pos_a);
}

TEST_F(AutomationTest, BracketNestedExpressions)
{
    misc::position pos(20);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "am0942");
    stack->top()->pushOp(stackref(), TestToken(pos, "++"));
    stack->top()->pushOpenBracket(stackref(), pos);
    pushInteger(pos, "20121113");
    stack->top()->matchClosing(stackref(), TestToken(pos, "]"));

    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    ASSERT_FALSE(error::hasError());
    clause.filter->compile(semantic::CompilingSpace());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, "[ ++ ]")
            (pos, OPERAND)
                (pos, IDENTIFIER, "am0942")
            (pos, OPERAND)
                (pos, LIST_BEGIN)
                    (pos, INTEGER, "20121113")
                (pos, LIST_END)
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ParenNestedExpressions)
{
    misc::position pos(21);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "am0945");
    stack->top()->pushOp(stackref(), TestToken(pos, "+"));
    stack->top()->pushOpenParen(stackref(), pos);
    pushInteger(pos, "20121113");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));

    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    ASSERT_FALSE(error::hasError());
    clause.filter->compile(semantic::CompilingSpace());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, "+")
            (pos, OPERAND)
                (pos, IDENTIFIER, "am0945")
            (pos, OPERAND)
                (pos, INTEGER, "20121113")
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceAnonyFuncAsOneOfArgs)
{
    misc::position pos(22);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    stack->top()->pushOpenParen(stackref(), pos);
    pushIdent(pos, "nozomu");
    stack->top()->pushOp(stackref(), TestToken(pos, "+"));
    pushIdent(pos, "inoti");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->pushOpenParen(stackref(), pos);
    pushIdent(pos, "kafuka");
    stack->top()->pushComma(stackref(), pos);
    pushIdent(pos, "maria");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->pushColon(stackref(), pos);
    pushIdent(pos, "meru");
    stack->top()->pushComma(stackref(), pos);
    stack->top()->pushOpenParen(stackref(), pos);
    pushIdent(pos, "tiri");
    stack->top()->pushOp(stackref(), TestToken(pos, "%"));
    pushIdent(pos, "abiru");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->pushComma(stackref(), pos);
    stack->top()->pushOpenParen(stackref(), pos);
    pushIdent(pos, "kiri");
    stack->top()->pushOp(stackref(), TestToken(pos, "/"));
    pushIdent(pos, "matoi");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    clause.filter->compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, CALL_BEGIN)
                (pos, BINARY_OP, "+")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "nozomu")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "inoti")
            (pos, ARGUMENTS)
                (pos, FUNC_DEF)
                    (pos, PARAMETER, "kafuka")
                    (pos, PARAMETER, "maria")
                    (BLOCK_BEGIN)
                        (pos, RETURN)
                            (pos, IDENTIFIER, "meru")
                    (BLOCK_END)
                (pos, BINARY_OP, "%")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "tiri")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "abiru")
                (pos, BINARY_OP, "/")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "kiri")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "matoi")
            (pos, CALL_END)
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, EmptyNested)
{
    misc::position pos(23);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    ASSERT_TRUE(error::hasError());
    ASSERT_EQ(1, getInvalidEmptyExprRecs().size());
    ASSERT_EQ(pos, getInvalidEmptyExprRecs()[0].pos);
}

TEST_F(AutomationTest, ReduceSimpleAsyncPlaceholderInCall)
{
    misc::position pos(24);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "motoharu");
    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->pushOp(stackref(), TestToken(pos, "%"));
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));

    stack->top()->pushOp(stackref(), TestToken(pos, "+"));

    pushIdent(pos, "tadakuni");
    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->pushOp(stackref(), TestToken(pos, "%"));
    stack->top()->pushComma(stackref(), pos);
    pushInteger(pos, "20130102");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));

    stack->top()->pushOp(stackref(), TestToken(pos, "*"));

    pushIdent(pos, "hidenori");
    stack->top()->pushOpenParen(stackref(), pos);
    pushInteger(pos, "1056");
    stack->top()->pushComma(stackref(), pos);
    stack->top()->pushOp(stackref(), TestToken(pos, "%"));
    pushIdent(pos, "yositake");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());
    ASSERT_TRUE(stack->empty());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, "+")
            (pos, OPERAND)
                (pos, ASYNC_CALL)
                (pos, CALL_BEGIN)
                    (pos, IDENTIFIER, "motoharu")
                (pos, ARGUMENTS)
                    (pos, ASYNC_PLACEHOLDER_BEGIN)
                    (pos, ASYNC_PLACEHOLDER_END)
                (pos, CALL_END)
            (pos, OPERAND)
                (pos, BINARY_OP, "*")
                (pos, OPERAND)
                    (pos, ASYNC_CALL)
                    (pos, CALL_BEGIN)
                        (pos, IDENTIFIER, "tadakuni")
                    (pos, ARGUMENTS)
                        (pos, ASYNC_PLACEHOLDER_BEGIN)
                        (pos, ASYNC_PLACEHOLDER_END)
                        (pos, INTEGER, "20130102")
                    (pos, CALL_END)
                (pos, OPERAND)
                    (pos, ASYNC_CALL)
                    (pos, CALL_BEGIN)
                        (pos, IDENTIFIER, "hidenori")
                    (pos, ARGUMENTS)
                        (pos, INTEGER, "1056")
                        (pos, ASYNC_PLACEHOLDER_BEGIN)
                            (pos, PARAMETER, "yositake")
                        (pos, ASYNC_PLACEHOLDER_END)
                    (pos, CALL_END)
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceParenAsyncPlaceholderInCall)
{
    misc::position pos(25);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "tosiyuki");
    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->pushOp(stackref(), TestToken(pos, "%"));
    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));

    stack->top()->pushOp(stackref(), TestToken(pos, "<"));

    pushIdent(pos, "mituo");
    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->pushOp(stackref(), TestToken(pos, "%"));
    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->pushComma(stackref(), pos);
    pushIdent(pos, "JanSecond");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));

    stack->top()->pushOp(stackref(), TestToken(pos, "-"));

    pushIdent(pos, "yuusuke");
    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->pushOp(stackref(), TestToken(pos, "%"));
    stack->top()->pushOpenParen(stackref(), pos);
    pushIdent(pos, "ringochan");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->pushComma(stackref(), pos);
    pushInteger(pos, "1106");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));

    stack->top()->pushOp(stackref(), TestToken(pos, "%"));

    pushIdent(pos, "otoha");
    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->pushOp(stackref(), TestToken(pos, "%"));
    stack->top()->pushOpenParen(stackref(), pos);
    pushIdent(pos, "yurine");
    stack->top()->pushComma(stackref(), pos);
    pushIdent(pos, "homura");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));

    stack->top()->pushOp(stackref(), TestToken(pos, "/"));

    pushIdent(pos, "ekou");
    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->pushOp(stackref(), TestToken(pos, "%"));
    stack->top()->pushOpenParen(stackref(), pos);
    pushIdent(pos, "kabba");
    stack->top()->pushComma(stackref(), pos);
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));

    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    clause.compile();
    clause.filter->compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());
    ASSERT_TRUE(stack->empty());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, "<")
            (pos, OPERAND)
                (pos, ASYNC_CALL)
                (pos, CALL_BEGIN)
                    (pos, IDENTIFIER, "tosiyuki")
                (pos, ARGUMENTS)
                    (pos, ASYNC_PLACEHOLDER_BEGIN)
                    (pos, ASYNC_PLACEHOLDER_END)
                (pos, CALL_END)
            (pos, OPERAND)
                (pos, BINARY_OP, "-")
                (pos, OPERAND)
                    (pos, ASYNC_CALL)
                    (pos, CALL_BEGIN)
                        (pos, IDENTIFIER, "mituo")
                    (pos, ARGUMENTS)
                        (pos, ASYNC_PLACEHOLDER_BEGIN)
                        (pos, ASYNC_PLACEHOLDER_END)
                        (pos, IDENTIFIER, "JanSecond")
                    (pos, CALL_END)
                (pos, OPERAND)
                    (pos, BINARY_OP, "/")
                    (pos, OPERAND)
                        (pos, BINARY_OP, "%")
                        (pos, OPERAND)
                            (pos, ASYNC_CALL)
                            (pos, CALL_BEGIN)
                                (pos, IDENTIFIER, "yuusuke")
                            (pos, ARGUMENTS)
                                (pos, ASYNC_PLACEHOLDER_BEGIN)
                                    (pos, PARAMETER, "ringochan")
                                (pos, ASYNC_PLACEHOLDER_END)
                                (pos, INTEGER, "1106")
                            (pos, CALL_END)
                        (pos, OPERAND)
                            (pos, ASYNC_CALL)
                            (pos, CALL_BEGIN)
                                (pos, IDENTIFIER, "otoha")
                            (pos, ARGUMENTS)
                                (pos, ASYNC_PLACEHOLDER_BEGIN)
                                    (pos, PARAMETER, "yurine")
                                    (pos, PARAMETER, "homura")
                                (pos, ASYNC_PLACEHOLDER_END)
                            (pos, CALL_END)
                    (pos, OPERAND)
                        (pos, ASYNC_CALL)
                        (pos, CALL_BEGIN)
                            (pos, IDENTIFIER, "ekou")
                        (pos, ARGUMENTS)
                            (pos, ASYNC_PLACEHOLDER_BEGIN)
                                (pos, PARAMETER, "kabba")
                            (pos, ASYNC_PLACEHOLDER_END)
                        (pos, CALL_END)
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceAsyncPlaceholderNotInCall)
{
    misc::position pos(26);
    misc::position pos_a(2600);
    misc::position pos_b(2601);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    stack->top()->pushOpenBracket(stackref(), pos);
    stack->top()->pushOp(stackref(), TestToken(pos_a, "%"));
    pushIdent(pos, "nue");
    stack->top()->pushComma(stackref(), pos);
    pushIdent(pos, "narumi");
    stack->top()->matchClosing(stackref(), TestToken(pos, "]"));

    stack->top()->pushOp(stackref(), TestToken(pos, "++"));

    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->pushOp(stackref(), TestToken(pos_b, "%"));
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));

    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    clause.compile();
    ASSERT_TRUE(error::hasError());

    std::vector<AsyncPlaceholderNotArgumentRec> recs(getAsyncPlaceholderNotArgumentRecs());
    ASSERT_EQ(2, recs.size());
    ASSERT_EQ(pos_a, recs[0].pos);
    ASSERT_EQ(pos_b, recs[1].pos);
}

TEST_F(AutomationTest, ReduceAsyncPlaceholderNotNames)
{
    misc::position pos(27);
    misc::position pos_a(2700);
    misc::position pos_b(2701);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "hinaru");
    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->pushOp(stackref(), TestToken(pos, "%"));
    stack->top()->pushOpenParen(stackref(), pos);
    pushInteger(pos_a, "1129");
    stack->top()->pushComma(stackref(), pos);
    pushBoolean(pos_b, false);
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));

    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    clause.compile();
    ASSERT_TRUE(error::hasError());

    std::vector<InvalidNameRec> recs(getInvalidNameRecs());
    ASSERT_EQ(2, recs.size());
    ASSERT_EQ(pos_a, recs[0].pos);
    ASSERT_EQ(pos_b, recs[1].pos);
}

TEST_F(AutomationTest, ReduceMoreThanOneAsyncPlaceholders)
{
    misc::position pos(28);
    misc::position pos_a(2800);
    misc::position pos_b(2801);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "wanyuudou");
    stack->top()->pushOpenParen(stackref(), pos);
    stack->top()->pushOp(stackref(), TestToken(pos_a, "%"));
    stack->top()->pushComma(stackref(), pos);
    pushBoolean(pos_b, false);
    stack->top()->pushComma(stackref(), pos);
    stack->top()->pushOp(stackref(), TestToken(pos_b, "%"));
    pushIdent(pos, "tutigumo");
    stack->top()->matchClosing(stackref(), TestToken(pos, ")"));

    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    clause.compile();
    ASSERT_TRUE(error::hasError());

    std::vector<MoreThanOneAsyncPlaceholderRec> recs(getMoreThanOneAsyncPlaceholderRecs());
    ASSERT_EQ(1, recs.size());
    ASSERT_EQ(pos_b, recs[0].pos);
}
