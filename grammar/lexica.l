%{
#include "lex-include.h"
%}

%option nounput
%option always-interactive

%%
^[ ]*#[^\n]*\n {
    ++grammar::lineno;
}

#[^\n]*\n {
    return EOL;
}

^[ |\t]*\n {
    ++grammar::lineno;
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

typeof {
    return OPERATOR;
}

true {
    return BOOL_TRUE;
}

false {
    return BOOL_FALSE;
}

try {
    return KW_RESERVED;
}

raise {
    return KW_RESERVED;
}

catch {
    return KW_RESERVED;
}

finally {
    return KW_RESERVED;
}

with {
    return KW_RESERVED;
}

break {
    return KW_RESERVED;
}

continue {
    return KW_RESERVED;
}

yield {
    return KW_RESERVED;
}

for {
    return KW_RESERVED;
}

gen {
    return KW_RESERVED;
}

ctor {
    return KW_RESERVED;
}

while {
    return KW_RESERVED;
}

[A-Z] {
    error::reservedWord(grammar::here(), yytext);
    return IDENT;
}

[_]+ {
    error::reservedWord(grammar::here(), yytext);
    return IDENT;
}

[0-9][0-9_]* {
    return INT_LITERAL;
}

[0-9][0-9_]*\.[0-9_]+ {
    return DOUBLE_LITERAL;
}

\.[0-9_]+ {
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

\"\"\"((\\.|[^\\"])|\"[^\"]|\"\"[^\"])*\"\"\" {
    return TRIPLE_QUOTED_STRING_LITERAL;
}

\'\'\'((\\.|[^\\'])|\'[^\']|\'\'[^\'])*\'\'\' {
    return TRIPLE_QUOTED_STRING_LITERAL;
}

[+][+] {
    return OPERATOR;
}

[<][<] {
    return OPERATOR;
}

[>][>] {
    return OPERATOR;
}

[>][>][>] {
    return OPERATOR;
}

[<]= {
    return OPERATOR;
}

[>]= {
    return OPERATOR;
}

!= {
    return OPERATOR;
}

&& {
    return OPERATOR;
}

[\|][\|] {
    return OPERATOR;
}

[\|][:?] {
    return PIPE_SEP;
}

:: {
    return PROP_SEP;
}

[$] {
    return PIPE_ELEMENT;
}

[$]i {
    return PIPE_INDEX;
}

[$]index {
    return PIPE_INDEX;
}

[$]k {
    return PIPE_KEY;
}

[$]key {
    return PIPE_KEY;
}

[$]r {
    return PIPE_RESULT;
}

[$]result {
    return PIPE_RESULT;
}

[%][%] {
    return REGULAR_ASYNC_PARAM;
}

[-+*/%!=<>\|~&^] {
    return OPERATOR;
}

[()\[\],:.{}@] {
    return *yytext;
}

. {
    error::invalidChar(grammar::here(), *yytext);
}
%%
