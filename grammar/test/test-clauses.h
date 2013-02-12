#ifndef __STEKIN_GRAMMAR_TEST_TEST_CLAUSES_H__
#define __STEKIN_GRAMMAR_TEST_TEST_CLAUSES_H__

#include <gtest/gtest.h>

#include <test/phony-errors.h>

#include "test-common.h"
#include "../clause-builder.h"
#include "../expr-nodes.h"
#include "../tokens.h"
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
            return new grammar::TypedToken(pos, img, grammar::OPERATOR);
        }

        grammar::Token* pipeSep(misc::position const& pos, std::string const& img)
        {
            return new grammar::TypedToken(pos, img, grammar::PIPE_SEP);
        }

        grammar::Token* open(misc::position const& pos, std::string const& img)
        {
            return new grammar::TypedToken(pos, img, IMAGE_TYPE_MAP.find(img)->second);
        }

        grammar::Token* close(misc::position const& pos, std::string const& img)
        {
            return new grammar::TypedToken(pos, img, IMAGE_TYPE_MAP.find(img)->second);
        }

        grammar::Token* comma(misc::position const& pos)
        {
            return new grammar::TypedToken(pos, ",", grammar::COMMA);
        }

        grammar::Token* colon(misc::position const& pos)
        {
            return new grammar::TypedToken(pos, ":", grammar::COLON);
        }

        grammar::Token* regularAsyncParam(misc::position const& pos)
        {
            return new grammar::FactorToken(
                        pos, util::mkptr(new grammar::RegularAsyncParam(pos)), "%%");
        }
    };

}

#endif /* __STEKIN_GRAMMAR_TEST_TEST_CLAUSES_H__ */
