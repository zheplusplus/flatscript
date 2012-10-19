#ifndef __STEKIN_PARSER_YY_MISC_H__
#define __STEKIN_PARSER_YY_MISC_H__

#include <string>

#include "clause-builder.h"
#include <misc/pos-type.h>

void yyerror(std::string const& msg);
int yyparse();
int yylex();
extern "C" int yywrap(void);

extern char* yytext;
extern int yylineno;

namespace grammar {

    int const SPACES_PER_INDENT = 4;

    extern ClauseBuilder builder;
    extern int last_indent;

    misc::position here();

}

#endif /* __STEKIN_PARSER_YY_MISC_H__ */
