#ifndef __STEKIN_GRAMMAR_TEST_TEST_CLAUSES_H__
#define __STEKIN_GRAMMAR_TEST_TEST_CLAUSES_H__

#include <gtest/gtest.h>

#include <test/phony-errors.h>

#include "test-common.h"
#include "../clause-builder.h"
#include "../expr-nodes.h"
#include "../expr-tokens.h"
#include "../syntax-types.h"

namespace test {

    struct ClausesTest
        : GrammarTest
    {
        grammar::Token* id(misc::position const& pos, std::string const& i)
        {
            return new grammar::FactorToken(pos, util::mkptr(new grammar::Identifier(pos, i)), i);
        }

        grammar::Token* pipeElement(misc::position const& pos)
        {
            return new grammar::FactorToken(pos, util::mkptr(new grammar::PipeElement(pos)), "$");
        }

        grammar::Token* op(misc::position const& pos, std::string const& img)
        {
            return new grammar::OpToken(pos, img);
        }

        grammar::Token* pipeSep(misc::position const& pos, std::string const& img)
        {
            return new grammar::PipeSepToken(pos, img);
        }

        grammar::Token* openParen(misc::position const& pos)
        {
            return new grammar::OpenParenToken(pos);
        }

        grammar::Token* close(misc::position const& pos, std::string const& img)
        {
            return new grammar::CloserToken(pos, img);
        }

        grammar::Token* colon(misc::position const& pos)
        {
            return new grammar::ColonToken(pos);
        }

        grammar::Token* comma(misc::position const& pos)
        {
            return new grammar::CommaToken(pos);
        }
    };

}

#endif /* __STEKIN_GRAMMAR_TEST_TEST_CLAUSES_H__ */
