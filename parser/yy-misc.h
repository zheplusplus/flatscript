#ifndef __STEKIN_PARSER_YY_MISC_H__
#define __STEKIN_PARSER_YY_MISC_H__

#include <string>

#include <grammar/clause-builder.h>
#include <misc/pos-type.h>

void yyerror(std::string const& msg);
int yyparse();
int yylex();
extern "C" int yywrap(void);

extern char* yytext;
extern int yylineno;

namespace parser {

    int const SPACES_PER_INDENT = 4;

    extern grammar::ClauseBuilder builder;
    extern int last_indent;

    misc::position here();
    misc::position here(int lineno);

}

#endif /* __STEKIN_PARSER_YY_MISC_H__ */
