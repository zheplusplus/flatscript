#ifndef __STEKIN_GRAMMAR_YY_MISC_H__
#define __STEKIN_GRAMMAR_YY_MISC_H__

#include <string>

#include <misc/pos-type.h>

#include "clause-builder.h"

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

}

#endif /* __STEKIN_GRAMMAR_YY_MISC_H__ */
