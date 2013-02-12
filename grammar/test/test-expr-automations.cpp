#include "test-automations.h"

using namespace test;

TEST_F(AutomationTest, ReduceArithExpression)
{
    misc::position pos(1);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushInteger(pos, "20121104");
    pushOp(pos, "+");
    pushOp(pos, "-");
    pushInteger(pos, "1349");
    pushOp(pos, "*");
    pushIdent(pos, "modokasii");
    pushOp(pos, ">");
    pushIdent(pos, "seikaino");
    pushOp(pos, "-");
    pushIdent(pos, "uede");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
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
    pushOp(pos, "<");
    pushOp(pos, "!");
    pushBoolean(pos, false);
    pushOp(pos, "&&");
    pushIdent(pos, "vision");
    pushOp(pos, ">");
    pushIdent(pos, "eternal");
    pushOp(pos, "||");
    pushIdent(pos, "rite");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());
    ASSERT_TRUE(stack->empty());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, CONDITIONAL)
            (pos, OPERAND)
                (pos, CONDITIONAL)
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
                    (pos, BOOLEAN, "false")
            (pos, OPERAND)
                (pos, BOOLEAN, "true")
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
    pushOp(pos, "++");
    pushIdent(pos, "kawai");
    pushPipeSep(pos, "|?");
    pushIdent(pos, "kugutu");
    pushOp(pos, "<");
    pushIdent(pos, "uta");
    pushPipeSep(pos, "|:");
    pushPipeElement(pos);
    pushOp(pos, "*");
    pushPipeElement(pos);
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());
    ASSERT_TRUE(stack->empty());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, "[ |: ]")
            (pos, OPERAND)
                (pos, BINARY_OP, "[ |? ]")
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

TEST_F(AutomationTest, ReduceBitwise)
{
    misc::position pos(4);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "touma");
    pushOp(pos, "+");
    pushIdent(pos, "fuyuki");
    pushOp(pos, "^");
    pushIdent(pos, "makoto");
    pushOp(pos, "+");
    pushIdent(pos, "shuuiti");
    pushOp(pos, "|");
    pushIdent(pos, "yosino");
    pushOp(pos, "*");
    pushIdent(pos, "fujioka");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());
    ASSERT_TRUE(stack->empty());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, "|")
            (pos, OPERAND)
                (pos, BINARY_OP, "^")
                (pos, OPERAND)
                    (pos, BINARY_OP, "+")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "touma")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "fuyuki")
                (pos, OPERAND)
                    (pos, BINARY_OP, "+")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "makoto")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "shuuiti")
            (pos, OPERAND)
                (pos, BINARY_OP, "*")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "yosino")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "fujioka")
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceCallExpression)
{
    misc::position pos(5);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "masayosi");
    pushOp(pos, "+");
    pushIdent(pos, "yamazaki");
    pushOp(pos, ".");
    pushIdent(pos, "one");
    open(pos, "(");
    pushIdent(pos, "more");
    pushComma(pos);
    pushIdent(pos, "time");
    pushOp(pos, "&&");
    pushIdent(pos, "chance");
    close(pos, ")");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
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
                    (pos, CONDITIONAL)
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "time")
                    (pos, OPERAND)
                        (pos, IDENTIFIER, "chance")
                    (pos, OPERAND)
                        (pos, BOOLEAN, "false")
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
    pushOp(pos, "*");
    open(pos, "(");
    pushIdent(pos, "naganohara");
    pushOp(pos, "+");
    pushIdent(pos, "sinonome");
    close(pos, ")");
    open(pos, "(");
    close(pos, ")");
    pushOp(pos, ".");
    pushIdent(pos, "sakamoto");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
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

    open(pos, "(");
    open(pos, "(");
    pushIdent(pos, "aioi");
    pushComma(pos);
    pushIdent(pos, "minokami");
    close(pos, ")");
    pushColon(pos);
    pushIdent(pos, "aioi");
    close(pos, ")");
    open(pos, "(");
    pushBoolean(pos, false);
    pushComma(pos);
    pushInteger(pos, "20121106");
    pushComma(pos);
    close(pos, ")");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
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
    pushColon(pos);
    open(pos, "(");
    pushIdent(pos, "nakamura");
    pushComma(pos);
    pushIdent(pos, "takasaki");
    close(pos, ")");
    pushColon(pos);
    pushIdent(pos, "sakurai");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
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
    open(pos, "[");
    pushIdent(pos, "minokami");
    pushComma(pos);
    pushComma(pos);
    pushIdent(pos, "aioi");
    close(pos, "]");
    open(pos, "[");
    pushIdent(pos, "naganohara");
    close(pos, "]");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, "[]")
            (pos, OPERAND)
                (pos, LIST_SLICE_BEGIN)
                    (pos, IDENTIFIER, "hakase")
                    (pos, IDENTIFIER, "minokami")
                    (pos, UNDEFINED)
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
    open(pos, "(");
    open(pos, "(");
    pushIdent(pos, "annaka");
    pushComma(pos);
    pushIdent(pos, "tatibana");
    close(pos, ")");
    pushColon(pos);
    pushIdent(pos, "fechan");
    close(pos, ")");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
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

    open(pos, "[");
    pushIdent(pos, "daiku");
    pushComma(pos);
    pushIdent(pos, "sekiguti");
    pushComma(pos);
    close(pos, "]");
    pushOp(pos, "++");
    ASSERT_FALSE(stack->top()->finishOnBreak(true));
    open(pos, "[");
    pushIdent(pos, "ogi");
    close(pos, "]");
    open(pos, "[");
    pushIdent(pos, "sakurai");
    close(pos, "]");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
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
    open(pos, "(");
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    open(pos, "{");
    close(pos, "}");
    pushComma(pos);
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    open(pos, "{");
    pushIdent(pos, "no");
    pushPropSep(pos);
    open(pos, "(");
    pushIdent(pos, "miyako");
    close(pos, ")");
    pushColon(pos);
    pushIdent(pos, "miyako");
    close(pos, "}");
    pushComma(pos);
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    open(pos, "{");
    pushIdent(pos, "stella");
    pushColon(pos);
    pushIdent(pos, "musica");
    pushComma(pos);
    close(pos, "}");
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    close(pos, ")");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
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

TEST_F(AutomationTest, ReduceSingleThis)
{
    misc::position pos(13);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushThis(pos);
    ASSERT_TRUE(stack->top()->finishOnBreak(true));

    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, THIS)
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceThisAndProperty)
{
    misc::position pos(14);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushThis(pos);
    pushIdent(pos, "higurasi");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));

    pushOp(pos, "+");
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    pushThis(pos);
    ASSERT_TRUE(stack->top()->finishOnBreak(true));

    open(pos, "(");
    pushThis(pos);
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    pushComma(pos);
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    pushThis(pos);
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    open(pos, "[");
    pushThis(pos);
    close(pos, "]");

    pushComma(pos);
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    pushThis(pos);
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    close(pos, ")");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));

    pushOp(pos, "+");
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    pushThis(pos);
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    pushInteger(pos, "20130121");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));

    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, BINARY_OP, "+")
            (pos, OPERAND)
                (pos, BINARY_OP, "+")
                (pos, OPERAND)
                    (pos, BINARY_OP, "[]")
                    (pos, OPERAND)
                        (pos, THIS)
                    (pos, OPERAND)
                        (pos, STRING, "higurasi")
                (pos, OPERAND)
                    (pos, CALL_BEGIN)
                        (pos, THIS)
                    (pos, ARGUMENTS)
                        (pos, THIS)
                        (pos, BINARY_OP, "[]")
                        (pos, OPERAND)
                            (pos, THIS)
                        (pos, OPERAND)
                            (pos, THIS)
                        (pos, THIS)
                    (pos, CALL_END)
            (pos, OPERAND)
                (pos, BINARY_OP, "[]")
                (pos, OPERAND)
                    (pos, THIS)
                (pos, OPERAND)
                    (pos, STRING, "20130121")
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ReduceInvalidLambdaParams)
{
    misc::position pos(15);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    open(pos, "(");
    pushInteger(pos, "20121110");
    close(pos, ")");
    pushColon(pos);
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

    pushOp(pos, ".");
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
    pushOp(pos, "!");
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

    open(pos, "{");
    pushInteger(pos, "1300");
    close(pos_a, "}");

    open(pos, "[");
    open(pos, "{");
    pushInteger(pos, "1300");
    pushComma(pos_b);
    close(pos_c, "}");
    close(pos, "]");

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

    open(pos, "[");
    pushColon(pos_a);
    pushInteger(pos_b, "1300");
    close(pos, "]");

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
    pushOp(pos, "++");
    open(pos, "[");
    pushInteger(pos, "20121113");
    close(pos, "]");

    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    ASSERT_FALSE(error::hasError());
    clause.filter->deliver().compile(semantic::CompilingSpace());

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
    pushOp(pos, "+");
    open(pos, "(");
    pushInteger(pos, "20121113");
    close(pos, ")");

    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    ASSERT_FALSE(error::hasError());
    clause.filter->deliver().compile(semantic::CompilingSpace());

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

    open(pos, "(");
    pushIdent(pos, "nozomu");
    pushOp(pos, "+");
    pushIdent(pos, "inoti");
    close(pos, ")");
    open(pos, "(");
    open(pos, "(");
    pushIdent(pos, "kafuka");
    pushComma(pos);
    pushIdent(pos, "maria");
    close(pos, ")");
    pushColon(pos);
    pushIdent(pos, "meru");
    pushComma(pos);
    open(pos, "(");
    pushIdent(pos, "tiri");
    pushOp(pos, "%");
    pushIdent(pos, "abiru");
    close(pos, ")");
    pushComma(pos);
    open(pos, "(");
    pushIdent(pos, "kiri");
    pushOp(pos, "/");
    pushIdent(pos, "matoi");
    close(pos, ")");
    close(pos, ")");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    ASSERT_TRUE(stack->empty());

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
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

    open(pos, "(");
    close(pos, ")");
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
    open(pos, "(");
    pushOp(pos, "%");
    close(pos, ")");

    pushOp(pos, "+");

    pushIdent(pos, "tadakuni");
    open(pos, "(");
    pushOp(pos, "%");
    pushComma(pos);
    pushInteger(pos, "20130102");
    close(pos, ")");

    pushOp(pos, "*");

    pushIdent(pos, "hidenori");
    open(pos, "(");
    pushInteger(pos, "1056");
    pushComma(pos);
    pushOp(pos, "%");
    pushIdent(pos, "yositake");
    close(pos, ")");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
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
    open(pos, "(");
    pushOp(pos, "%");
    open(pos, "(");
    close(pos, ")");
    close(pos, ")");

    pushOp(pos, "<");

    pushIdent(pos, "mituo");
    open(pos, "(");
    pushOp(pos, "%");
    open(pos, "(");
    close(pos, ")");
    pushComma(pos);
    pushIdent(pos, "JanSecond");
    close(pos, ")");

    pushOp(pos, "-");

    pushIdent(pos, "yuusuke");
    open(pos, "(");
    pushOp(pos, "%");
    open(pos, "(");
    pushIdent(pos, "ringochan");
    close(pos, ")");
    pushComma(pos);
    pushInteger(pos, "1106");
    close(pos, ")");

    pushOp(pos, "%");

    pushIdent(pos, "otoha");
    open(pos, "(");
    pushOp(pos, "%");
    open(pos, "(");
    pushIdent(pos, "yurine");
    pushComma(pos);
    pushIdent(pos, "homura");
    close(pos, ")");
    close(pos, ")");

    pushOp(pos, "/");

    pushIdent(pos, "ekou");
    open(pos, "(");
    pushOp(pos, "%");
    open(pos, "(");
    pushIdent(pos, "kabba");
    pushComma(pos);
    close(pos, ")");
    close(pos, ")");

    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
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

    open(pos, "[");
    pushOp(pos_a, "%");
    pushIdent(pos, "nue");
    pushComma(pos);
    pushIdent(pos, "narumi");
    close(pos, "]");

    pushOp(pos, "++");

    open(pos, "(");
    pushOp(pos_b, "%");
    close(pos, ")");

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
    open(pos, "(");
    pushOp(pos, "%");
    open(pos, "(");
    pushInteger(pos_a, "1129");
    pushComma(pos);
    pushBoolean(pos_b, false);
    close(pos, ")");
    close(pos, ")");

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
    open(pos, "(");
    pushOp(pos_a, "%");
    pushComma(pos);
    pushBoolean(pos_b, false);
    pushComma(pos);
    pushOp(pos_b, "%");
    pushIdent(pos, "tutigumo");
    close(pos, ")");

    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);
    clause.compile();
    ASSERT_TRUE(error::hasError());

    std::vector<MoreThanOneAsyncPlaceholderRec> recs(getMoreThanOneAsyncPlaceholderRecs());
    ASSERT_EQ(1, recs.size());
    ASSERT_EQ(pos_b, recs[0].pos);
}

TEST_F(AutomationTest, EmptyPipeSection)
{
    misc::position pos(29);
    misc::position pos_a(2900);
    misc::position pos_b(2901);
    misc::position pos_c(2902);
    misc::position pos_d(2903);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "sonozaki");
    open(pos_a, "(");
    pushPipeSep(pos_b, "|:");
    pushComma(pos_c);
    pushBoolean(pos_b, false);
    close(pos, ")");

    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    ASSERT_TRUE(error::hasError());

    ASSERT_EQ(2, getInvalidEmptyExprRecs().size());
    ASSERT_EQ(pos_b, getInvalidEmptyExprRecs()[0].pos);
    ASSERT_EQ(pos_c, getInvalidEmptyExprRecs()[1].pos);
}

TEST_F(AutomationTest, Conditional)
{
    misc::position pos(30);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "teru");

    pushIf(pos);
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    pushIdent(pos, "saki");
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    pushElse(pos);
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    pushOp(pos, "+");
    ASSERT_FALSE(stack->top()->finishOnBreak(true));

    pushIdent(pos, "kyotarou");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());
    ASSERT_TRUE(stack->empty());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, CONDITIONAL)
            (pos, OPERAND)
                (pos, IDENTIFIER, "saki")
            (pos, OPERAND)
                (pos, IDENTIFIER, "teru")
            (pos, OPERAND)
                (pos, PRE_UNARY_OP, "+")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "kyotarou")
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ConditionalTerminatedAfterIf)
{
    misc::position pos(31);
    misc::position pos_a(3100);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "kataoka");
    open(pos, "(");

    pushIdent(pos, "haramura");
    pushIf(pos);
    pushIdent(pos_a, "someya");
    close(pos, ")");

    finish(pos);
    clause.compile();
    ASSERT_TRUE(error::hasError());

    std::vector<IncompleteConditionalRec> recs(getIncompleteConditionalRecs());
    ASSERT_EQ(1, recs.size());
    ASSERT_EQ(pos_a, recs[0].pos);
}

TEST_F(AutomationTest, ConditionalWithEmptyPredicate)
{
    misc::position pos(32);
    misc::position pos_a(3200);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "ueno");
    open(pos, "(");

    pushIdent(pos, "kakei");
    pushIf(pos);
    pushElse(pos_a);
    pushIdent(pos, "fukuji");
    close(pos, ")");

    finish(pos);
    clause.compile();
    ASSERT_TRUE(error::hasError());

    std::vector<InvalidEmptyExprRec> recs(getInvalidEmptyExprRecs());
    ASSERT_EQ(1, recs.size());
    ASSERT_EQ(pos_a, recs[0].pos);
}

TEST_F(AutomationTest, ConditionalEncounterElseBeforeIf)
{
    misc::position pos(33);
    misc::position pos_a(3300);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "hisa");
    pushElse(pos);
    pushIdent(pos, "mihoko");
    pushElse(pos_a);

    ASSERT_TRUE(error::hasError());

    std::vector<UnexpectedTokenRec> recs(getUnexpectedTokenRecs());
    ASSERT_EQ(1, recs.size());
    ASSERT_EQ(pos_a, recs[0].pos);
    ASSERT_EQ("else", recs[0].image);
}

TEST_F(AutomationTest, ConditionalEncounterDuplateIf)
{
    misc::position pos(34);
    misc::position pos_a(3400);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "yuki");
    pushIf(pos);
    pushIdent(pos, "kyoutarou");
    pushIf(pos_a);

    ASSERT_TRUE(error::hasError());

    std::vector<UnexpectedTokenRec> recs(getUnexpectedTokenRecs());
    ASSERT_EQ(1, recs.size());
    ASSERT_EQ(pos_a, recs[0].pos);
    ASSERT_EQ("if", recs[0].image);
}

TEST_F(AutomationTest, ParenMismatch)
{
    misc::position pos(35);
    misc::position pos_a(3500);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));
    pushIdent(pos, "nagi");
    open(pos, "[");
    close(pos_a, ")");

    ASSERT_TRUE(error::hasError());
    std::vector<UnexpectedTokenRec> recs(getUnexpectedTokenRecs());
    ASSERT_EQ(1, recs.size());
    ASSERT_EQ(pos_a, recs[0].pos);
    ASSERT_EQ(")", recs[0].image);
}

TEST_F(AutomationTest, ReduceRegularAsyncCall)
{
    misc::position pos(36);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "yosino");
    open(pos, "(");
    pushIdent(pos, "saori");
    pushComma(pos);
    pushRegularAsyncParam(pos);
    pushComma(pos);
    pushIdent(pos, "kanako");
    open(pos, "(");
    pushRegularAsyncParam(pos);
    pushComma(pos);
    pushIdent(pos, "makoto");
    close(pos, ")");
    pushComma(pos);
    close(pos, ")");
    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());
    ASSERT_TRUE(stack->empty());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, ASYNC_CALL, 1)
            (pos, CALL_BEGIN)
                (pos, IDENTIFIER, "yosino")
            (pos, ARGUMENTS)
                (pos, IDENTIFIER, "saori")
                (pos, ASYNC_CALL, 0)
                (pos, CALL_BEGIN)
                    (pos, IDENTIFIER, "kanako")
                (pos, ARGUMENTS)
                    (pos, IDENTIFIER, "makoto")
                (pos, CALL_END)
            (pos, CALL_END)
        (BLOCK_END)
    ;
}

TEST_F(AutomationTest, ConditionalNested)
{
    misc::position pos(37);
    TestClause clause;
    stack->push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(clause))));

    pushIdent(pos, "kataoka");
    pushIf(pos);

    open(pos, "(");
    pushIdent(pos, "takei");
    pushIf(pos);
    pushIdent(pos, "someya");
    pushElse(pos);
    pushIdent(pos, "haramura");
    close(pos, ")");

    pushElse(pos);
    pushIdent(pos, "miyanaga");

    ASSERT_TRUE(stack->top()->finishOnBreak(true));
    finish(pos);

    clause.compile();
    clause.filter->deliver().compile(semantic::CompilingSpace());
    ASSERT_FALSE(error::hasError());
    ASSERT_TRUE(stack->empty());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, ARITHMETICS)
            (pos, CONDITIONAL)
            (pos, OPERAND)
                (pos, CONDITIONAL)
                (pos, OPERAND)
                    (pos, IDENTIFIER, "someya")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "takei")
                (pos, OPERAND)
                    (pos, IDENTIFIER, "haramura")
            (pos, OPERAND)
                (pos, IDENTIFIER, "kataoka")
            (pos, OPERAND)
                (pos, IDENTIFIER, "miyanaga")
        (BLOCK_END)
    ;
}
