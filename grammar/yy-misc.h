#ifndef __STEKIN_GRAMMAR_YY_MISC_H__
#define __STEKIN_GRAMMAR_YY_MISC_H__

#include <misc/pos-type.h>

#include "clause-builder.h"
#include "tokens.h"

void yyerror(std::string const& msg);
int yylex();
extern "C" int yywrap(void);

extern char* yytext;

namespace grammar {

    extern ClauseBuilder builder;
    extern int last_indent;
    extern int lineno;

    misc::position here();
    void parse();

    struct TokenSequence {
        explicit TokenSequence(Token* token);
        TokenSequence* add(Token* token);
        std::vector<util::sptr<Token>> deliver();
    private:
        std::vector<util::sptr<Token>> _list;
    };

}

#endif /* __STEKIN_GRAMMAR_YY_MISC_H__ */
