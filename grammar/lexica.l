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

extern {
    return KW_EXTERN;
}

export {
    return KW_EXPORT;
}

from {
    return KW_RESERVED;
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
    return KW_TRY;
}

throw {
    return KW_THROW;
}

catch {
    return KW_CATCH;
}

finally {
    return KW_RESERVED;
}

with {
    return KW_RESERVED;
}

break {
    return KW_BREAK;
}

continue {
    return KW_CONTINUE;
}

yield {
    return KW_RESERVED;
}

for {
    return KW_FOR;
}

gen {
    return KW_RESERVED;
}

class {
    return KW_CLASS;
}

this {
    return KW_THIS;
}

super {
    return KW_SUPER;
}

ctor {
    return KW_CONSTRUCTOR;
}

while {
    return KW_RESERVED;
}

delete {
    return KW_RESERVED;
}

[0-9][0-9_]* {
    return INT_LITERAL;
}

[0-9][0-9_]*\.[0-9][0-9_]* {
    return DOUBLE_LITERAL;
}

\.[0-9][0-9_]* {
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

[/][^ \n](\\.|[^\\/])*[/][a-zA-Z]* {
    return REGEXP;
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

[$]e {
    return EXCEPTION_OBJ;
}

[$]exception {
    return EXCEPTION_OBJ;
}

[%][%] {
    return REGULAR_ASYNC_PARAM;
}

[-+*/%!=<>\|~&^] {
    return OPERATOR;
}

[()\[\],:.{}] {
    return *yytext;
}

. {
    error::invalidChar(grammar::here(), *yytext);
}
%%
