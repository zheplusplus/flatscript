#ifndef __FLSC_GRAMMAR_TOKENIZE_H__
#define __FLSC_GRAMMAR_TOKENIZE_H__

#include <vector>
#include <string>

#include <misc/pos-type.h>

namespace grammar {

    struct RoughToken {
        std::string image;
        std::string type;
        misc::position pos;

        RoughToken(std::string i, std::string t, std::string file, int lineno)
            : image(std::move(i))
            , type(std::move(t))
            , pos(std::move(file), lineno)
        {}

        RoughToken(RoughToken const&) = default;

        RoughToken(RoughToken&& rhs)
            : image(std::move(rhs.image))
            , type(std::move(rhs.type))
            , pos(rhs.pos)
        {}
    };

    struct TokenizeResult {
        typedef std::string::const_iterator Iterator;

        std::vector<RoughToken> tokens;
        Iterator iterator;
        int lineno_inc;

        TokenizeResult(std::vector<RoughToken> t, Iterator i, int ln)
            : tokens(std::move(t))
            , iterator(i)
            , lineno_inc(ln)
        {}

        TokenizeResult(TokenizeResult&& rhs)
            : tokens(std::move(rhs.tokens))
            , iterator(rhs.iterator)
            , lineno_inc(rhs.lineno_inc)
        {}

        void push(RoughToken t)
        {
            this->tokens.push_back(std::move(t));
        }
    };

    TokenizeResult tokenize(TokenizeResult::Iterator begin, TokenizeResult::Iterator end,
                            std::string const& file, int lineno);

}

#endif /* __FLSC_GRAMMAR_TOKENIZE_H__ */
