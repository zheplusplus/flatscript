#ifndef __STEKIN_GRAMMAR_TEST_TEST_AUTOMATIONS_H__
#define __STEKIN_GRAMMAR_TEST_TEST_AUTOMATIONS_H__

#include <gtest/gtest.h>

#include <grammar/expr-automations.h>
#include <grammar/stmt-automations.h>
#include <util/string.h>
#include <test/phony-errors.h>

#include "test-common.h"

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
        grammar::FactorToken t(pos, util::mkptr(new grammar::BoolLiteral(pos, value)),
                               value ? "true" : "false");
        stack->top()->pushFactor(*stkptr, t);
    }

    void pushInteger(misc::position const& pos, std::string const& image)
    {
        grammar::FactorToken t(pos, util::mkptr(new grammar::IntLiteral(pos, image)), image);
        stack->top()->pushFactor(*stkptr, t);
    }

    void pushString(misc::position const& pos, std::string const& image)
    {
        grammar::FactorToken t(pos, util::mkptr(new grammar::StringLiteral(pos, image)), image);
        stack->top()->pushFactor(*stkptr, t);
    }

    void pushIdent(misc::position const& pos, std::string const& image)
    {
        grammar::FactorToken t(pos, util::mkptr(new grammar::Identifier(pos, image)), image);
        stack->top()->pushFactor(*stkptr, t);
    }

    void pushPipeElement(misc::position const& pos)
    {
        grammar::FactorToken t(pos, util::mkptr(new grammar::PipeElement(pos)), "$");
        stack->top()->pushFactor(*stkptr, t);
    }

    void pushOp(misc::position const& pos, std::string const& op)
    {
        stack->top()->nextToken(*stkptr, grammar::TypedToken(pos, op, grammar::OPERATOR));
    }

    void pushRegularAsyncParam(misc::position const& pos)
    {
        grammar::FactorToken t(pos, util::mkptr(new grammar::RegularAsyncParam(pos)), "%%");
        stack->top()->pushFactor(*stkptr, t);
    }

    void pushThis(misc::position const& pos)
    {
        grammar::FactorToken t(pos, util::mkptr(new grammar::This(pos)), "this");
        stack->top()->pushFactor(*stkptr, t);
    }

    void pushPipeSep(misc::position const& pos, std::string const& op)
    {
        stack->top()->nextToken(*stkptr, grammar::TypedToken(pos, op, grammar::PIPE_SEP));
    }

    void pushColon(misc::position const& pos)
    {
        stack->top()->nextToken(*stkptr, grammar::TypedToken(pos, ":", grammar::COLON));
    }

    void pushComma(misc::position const& pos)
    {
        stack->top()->nextToken(*stkptr, grammar::TypedToken(pos, ",", grammar::COMMA));
    }

    void pushIf(misc::position const& pos)
    {
        stack->top()->nextToken(*stkptr, grammar::TypedToken(pos, "if", grammar::IF));
    }

    void pushElse(misc::position const& pos)
    {
        stack->top()->nextToken(*stkptr, grammar::TypedToken(pos, "else", grammar::ELSE));
    }

    void open(misc::position const& pos, std::string const& image)
    {
        stack->top()->nextToken(
                *stkptr, grammar::TypedToken(pos, image, test::IMAGE_TYPE_MAP.find(image)->second));
    }

    void close(misc::position const& pos, std::string const& image)
    {
        stack->top()->nextToken(
                *stkptr, grammar::TypedToken(pos, image, test::IMAGE_TYPE_MAP.find(image)->second));
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
