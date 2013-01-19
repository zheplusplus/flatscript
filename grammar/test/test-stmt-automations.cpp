#include "test-automations.h"

using namespace test;

TEST_F(AutomationTest, ReduceNameDef)
{
    misc::position pos(1);
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
    clause.filter->deliver().compile(semantic::CompilingSpace());
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

TEST_F(AutomationTest, ReduceInvalidNameDef)
{
    misc::position pos(2);
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
    misc::position pos(3);
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
