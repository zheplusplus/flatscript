#include <gtest/gtest.h>

#include <test/phony-errors.h>

#include "test-common.h"
#include "../expr-nodes.h"

using namespace test;

typedef GrammarTest ExprNodesTest;

TEST_F(ExprNodesTest, Pipeline)
{
    misc::position pos(1);
    semantic::CompilingSpace space;

    util::sptr<grammar::Expression const> p(new grammar::Pipeline(
                pos
              , util::mkptr(new grammar::Identifier(pos, "x20130109"))
              , "|:"
              , util::mkptr(new grammar::PipeElement(pos))));
    p->reduceAsExpr()->compile(space);
    ASSERT_FALSE(error::hasError());

    DataTree::expectOne()
        (pos, BINARY_OP, "[ |: ]")
        (pos, OPERAND)
            (pos, IDENTIFIER, "x20130109")
        (pos, OPERAND)
            (pos, PIPE_ELEMENT)
    ;
}

TEST_F(ExprNodesTest, RegularAsyncParamAsExpr)
{
    misc::position pos(2);
    misc::position pos_a(200);

    util::sptr<grammar::Expression const> p(new grammar::Pipeline(
                pos
              , util::mkptr(new grammar::Identifier(pos, "x20130306"))
              , "|:"
              , util::mkptr(new grammar::RegularAsyncParam(pos_a))));
    p->reduceAsExpr();
    ASSERT_TRUE(error::hasError());

    std::vector<AsyncParamNotExprRec> recs(getAsyncParamNotExprRecs());
    ASSERT_EQ(1, recs.size());
    ASSERT_EQ(pos_a, recs[0].pos);
}
