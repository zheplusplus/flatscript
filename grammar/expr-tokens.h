#ifndef __STEKIN_GRAMMAR_EXPRESSION_TOKEN_H__
#define __STEKIN_GRAMMAR_EXPRESSION_TOKEN_H__

#include "automation-base.h"

namespace grammar {

    struct OpToken
        : Token
    {
        OpToken(misc::position const& pos, std::string const& image)
            : Token(pos, image)
        {}

        void act(AutomationStack& stack);
    };

    struct FactorToken
        : Token
    {
        FactorToken(misc::position const& pos
                  , util::sptr<Expression const> f
                  , std::string const& value)
            : Token(pos, value)
            , factor(std::move(f))
        {}

        void act(AutomationStack& stack);

        util::sptr<Expression const> factor;
    };

    struct OpenParenToken
        : Token
    {
        explicit OpenParenToken(misc::position const& pos)
            : Token(pos, "")
        {}

        void act(AutomationStack& stack);
    };

    struct OpenBracketToken
        : Token
    {
        explicit OpenBracketToken(misc::position const& pos)
            : Token(pos, "")
        {}

        void act(AutomationStack& stack);
    };

    struct OpenBraceToken
        : Token
    {
        explicit OpenBraceToken(misc::position const& pos)
            : Token(pos, "")
        {}

        void act(AutomationStack& stack);
    };

    struct CloserToken
        : Token
    {
        CloserToken(misc::position const& pos, std::string const& value)
            : Token(pos, value)
        {}

        void act(AutomationStack& stack);
    };

    struct ColonToken
        : Token
    {
        explicit ColonToken(misc::position const& pos)
            : Token(pos, "")
        {}

        void act(AutomationStack& stack);
    };

    struct CommaToken
        : Token
    {
        explicit CommaToken(misc::position const& pos)
            : Token(pos, "")
        {}

        void act(AutomationStack& stack);
    };

}

#endif /* __STEKIN_GRAMMAR_EXPRESSION_TOKEN_H__ */
