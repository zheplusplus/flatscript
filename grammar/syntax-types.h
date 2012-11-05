#ifndef __STEKIN_GRAMMAR_SYNTAX_TYPE_H__
#define __STEKIN_GRAMMAR_SYNTAX_TYPE_H__

#include <string>
#include <vector>

#include <semantic/fwd-decl.h>
#include <report/errors.h>
#include <util/pointer.h>
#include <misc/pos-type.h>

#include "expr-tokens.h"

namespace grammar {

    struct OpImage {
        explicit OpImage(char const* image)
            : _img(image)
        {}

        std::string deliver();
    private:
        std::string const _img;
    };

    struct Ident {
        misc::position const pos;

        Ident(misc::position const& ps, char const* id_text)
            : pos(ps)
            , _id(id_text)
        {}

        std::string deliver();
    private:
        std::string _id;
    };

    struct NameList {
        NameList* add(std::string const& name);
        std::vector<std::string> deliver();
    private:
        std::vector<std::string> _names;
    };

    struct TokenSequence {
        explicit TokenSequence(Token* token);
        TokenSequence* add(Token* token);
        std::vector<util::sptr<Token>> deliver();
    private:
        std::vector<util::sptr<Token>> _list;
    };

}

#endif /* __STEKIN_GRAMMAR_SYNTAX_TYPE_H__ */
