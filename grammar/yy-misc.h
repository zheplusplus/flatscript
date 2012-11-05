#ifndef __STEKIN_GRAMMAR_YY_MISC_H__
#define __STEKIN_GRAMMAR_YY_MISC_H__

#include <string>

#include <misc/pos-type.h>

#include "clause-builder.h"
#include "node-base.h"
#include "function.h"

void yyerror(std::string const& msg);
int yyparse();
int yylex();
extern "C" int yywrap(void);

extern char* yytext;
extern int yylineno;

namespace grammar {

    extern ClauseBuilder builder;
    extern int last_indent;

    misc::position here();

}

#endif /* __STEKIN_GRAMMAR_YY_MISC_H__ */
