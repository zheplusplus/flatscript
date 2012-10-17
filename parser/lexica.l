%{
#include "lex-include.h"
%}

%option nounput
%option always-interactive

%%
\r\n {
    return EOL;
}

\r {
    return EOL;
}

\n {
    return EOL;
}

^[ ]+ {
    if (0 != yyleng % parser::SPACES_PER_INDENT) {
        error::badIndent(parser::here());
    }
    parser::last_indent = yyleng / parser::SPACES_PER_INDENT;
    return INDENT;
}

^[ |\t]+ {
    error::tabAsIndent(parser::here());
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

[$]element {
    return LIST_ELEMENT;
}

[$]index {
    return LIST_INDEX;
}

[-+*/%=<>!()\[\],:.\|] {
    return *yytext;
}

. {
    error::invalidChar(parser::here(), *yytext);
}
%%
