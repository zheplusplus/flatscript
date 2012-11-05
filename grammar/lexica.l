%{
#include "lex-include.h"
%}

%option nounput
%option always-interactive

%%
^[ |\t]*\n {
    ++yylineno;
}

\r {}

\n {
    return EOL;
}

^[ ]+ {
    grammar::last_indent = yyleng;
    return INDENT;
}

^[ |\t]+ {
    error::tabAsIndent(grammar::here());
    return INDENT;
}

[\ ]+ {}
[\t]+ {}

func {
    return KW_FUNC;
}

ifnot {
    return KW_IFNOT;
}

if {
    return KW_IF;
}

else {
    return KW_ELSE;
}

return {
    return KW_RETURN;
}

import {
    return KW_IMPORT;
}

export {
    return KW_EXPORT;
}

true {
    return BOOL_TRUE;
}

false {
    return BOOL_FALSE;
}

[0-9]+ {
    return INT_LITERAL;
}

[0-9]*\.[0-9]+ {
    return DOUBLE_LITERAL;
}

[_a-zA-Z][_a-zA-Z0-9]* {
    return IDENT;
}

\"(\\.|[^\\"])*\" {
    return STRING_LITERAL;
}

\'(\\.|[^\\'])*\' {
    return STRING_LITERAL;
}

[+][+] {
    return LIST_APPEND;
}

[<]= {
    return LE;
}

[>]= {
    return GE;
}

!= {
    return NE;
}

&& {
    return AND;
}

[\|][\|] {
    return OR;
}

[\|]: {
    return PIPE_MAP;
}

[\|][?] {
    return PIPE_FILTER;
}

[$] {
    return LIST_ELEMENT;
}

[$]i {
    return LIST_INDEX;
}

[-+*/%=<>!()\[\],:.\|{}] {
    return *yytext;
}

. {
    error::invalidChar(grammar::here(), *yytext);
}
%%
