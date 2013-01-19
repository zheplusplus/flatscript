#ifndef __STEKIN_GRAMMAR_TEST_TEST_AUTOMATIONS_H__
#define __STEKIN_GRAMMAR_TEST_TEST_AUTOMATIONS_H__

#include <gtest/gtest.h>

#include <util/string.h>
#include <test/phony-errors.h>

#include "test-common.h"
#include "../expr-automations.h"
#include "../stmt-automations.h"

struct AutomationTest
    : test::GrammarTest
{
    AutomationTest()
        : stack(nullptr)
    {}

    void SetUp()
    {
        stkptr = new grammar::AutomationStack;
        stack.reset(stkptr);
        test::GrammarTest::SetUp();
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

#endif /* __STEKIN_GRAMMAR_TEST_TEST_AUTOMATIONS_H__ */
