#ifndef __STEKIN_GRAMMAR_EXPRESSION_TOKEN_H__
#define __STEKIN_GRAMMAR_EXPRESSION_TOKEN_H__

#include <util/pointer.h>
#include <misc/pos-type.h>

#include "node-base.h"

namespace grammar {

    struct AutomationStack;

    enum TokenType {
        COMMA, OPERATOR, OPEN_PAREN, CLOSE_PAREN, OPEN_BRACKET, CLOSE_BRACKET,
        OPEN_BRACE, CLOSE_BRACE, PIPE_SEP, COLON, IF, ELSE, IFNOT, FOR, BREAK,
        CONTINUE, TRY, CATCH, THROW, FUNC, RETURN, CLASS, SUPER, CONSTRUCTOR,
        EXTERN, EXPORT, ENUM, INCLUDE, TOKEN_TYPE_COUNT
    };

    struct Token {
        Token(Token const&) = delete;
        virtual ~Token() {}

        misc::position const pos;
        std::string const image;

        Token(misc::position const& ps, std::string img)
            : pos(ps)
            , image(std::move(img))
        {}

        void unexpected() const;

        virtual void act(AutomationStack& stack) = 0;
    };

    struct TypedToken
        : Token
    {
        TypedToken(misc::position const& ps, std::string img, TokenType tp)
            : Token(ps, std::move(img))
            , type(tp)
        {}

        void act(AutomationStack& stack);

        TokenType const type;
    };

    struct FactorToken
        : Token
    {
        FactorToken(misc::position const& pos, util::sptr<Expression const> e, std::string value)
            : Token(pos, std::move(value))
            , expr(std::move(e))
        {}

        void act(AutomationStack& stack);

        util::sptr<Expression const> expr;
    };

}

#endif /* __STEKIN_GRAMMAR_EXPRESSION_TOKEN_H__ */
