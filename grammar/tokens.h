#ifndef __STEKIN_GRAMMAR_EXPRESSION_TOKEN_H__
#define __STEKIN_GRAMMAR_EXPRESSION_TOKEN_H__

#include <util/pointer.h>
#include <misc/pos-type.h>

#include "fwd-decl.h"

namespace grammar {

    enum TokenType {
        IF, ELSE, IFNOT, PIPE_SEP, COMMA, COLON, OPEN_PAREN, CLOSE_PAREN,
        OPEN_BRACKET, CLOSE_BRACKET, OPEN_BRACE, CLOSE_BRACE, OPERATOR, FUNC,
        RETURN, TRY, CATCH, THROW, CLASS, SUPER, CONSTRUCTOR, TOKEN_TYPE_COUNT
    };

    struct Token {
        Token(Token const&) = delete;
        virtual ~Token() {}

        misc::position const pos;
        std::string const image;

        Token(misc::position const& ps, std::string const& img)
            : pos(ps)
            , image(img)
        {}

        virtual void act(AutomationStack& stack) = 0;
    };

    struct TypedToken
        : Token
    {
        TypedToken(misc::position const& ps, std::string const& img, TokenType tp)
            : Token(ps, img)
            , type(tp)
        {}

        void act(AutomationStack& stack);

        TokenType const type;
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

}

#endif /* __STEKIN_GRAMMAR_EXPRESSION_TOKEN_H__ */
