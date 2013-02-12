%{
#include "syn-include.h"
%}

%union {
    int indent_type;
    int line_num_type;

    grammar::Ident* ident_type;
    grammar::NameList* names_type;
    grammar::TokenSequence* token_sequence_type;
    grammar::Token* token_type;
}

%type <indent_type> indent

%type <line_num_type> eol

%type <ident_type> ident

%type <names_type> name_list
%type <names_type> additional_name
%type <names_type> member_name

%type <token_type> token
%type <token_sequence_type> token_sequence

%token INDENT EOL
%token KW_FUNC KW_IF KW_IFNOT KW_ELSE KW_RETURN KW_IMPORT KW_EXPORT KW_RESERVED
%token PROP_SEP OPERATOR PIPE_SEP
%token BOOL_TRUE BOOL_FALSE
%token INT_LITERAL DOUBLE_LITERAL STRING_LITERAL TRIPLE_QUOTED_STRING_LITERAL
%token IDENT
%token PIPE_ELEMENT PIPE_INDEX PIPE_KEY PIPE_RESULT REGULAR_ASYNC_PARAM

%%

root:
    stmt_list
;

indent:
    INDENT
    {
        $$ = grammar::last_indent;
    }
    |
    {
        $$ = 0;
    }
;

eol:
   EOL
   {
        $$ = grammar::lineno;
        ++grammar::lineno;
   }
;

stmt_list:
    stmt_list stmt {}
    |
    {}
;

stmt:
    arithmetics {}
    |
    func_return {}
    |
    import {}
    |
    export {}
    |
    ifnot_clue {}
;

arithmetics:
    indent token_sequence eol
    {
        grammar::builder.addArith($1, misc::position($3), $2->deliver());
    }
;

func_return:
    indent KW_RETURN token_sequence eol
    {
        grammar::builder.addReturn($1, misc::position($4), $3->deliver());
    }
    |
    indent KW_RETURN eol
    {
        grammar::builder.addReturn(
                    $1, misc::position($3), std::vector<util::sptr<grammar::Token>>());
    }
;

import:
    indent KW_IMPORT name_list eol
    {
        grammar::builder.addImport($1, misc::position($4), $3->deliver());
    }
;

export:
    indent KW_EXPORT member_name ':' token_sequence eol
    {
        std::vector<std::string> names = $3->deliver();
        if (names.size() == 1) {
            error::exportToIdent(misc::position($6), names[0]);
        }
        grammar::builder.addExport($1, misc::position($6), names, $5->deliver());
    }
;

member_name:
    member_name '.' ident
    {
        $$ = $1->add($3->deliver());
    }
    |
    ident
    {
        $$ = (new grammar::NameList)->add($1->deliver());
    }
;

token_sequence:
    token_sequence token
    {
        $$ = $1->add($2);
    }
    |
    token
    {
        $$ = new grammar::TokenSequence($1);
    }
;

token:
    KW_FUNC
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::FUNC);
    }
    |
    KW_IF
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::IF);
    }
    |
    KW_ELSE
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::ELSE);
    }
    |
    OPERATOR
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::OPERATOR);
    }
    |
    '.'
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::OPERATOR);
    }
    |
    PIPE_SEP
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::PIPE_SEP);
    }
    |
    PROP_SEP
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::PROP_SEP);
    }
    |
    BOOL_TRUE
    {
        $$ = new grammar::FactorToken(
            grammar::here(), util::mkptr(new grammar::BoolLiteral(grammar::here(), true)), "true");
    }
    |
    BOOL_FALSE
    {
        $$ = new grammar::FactorToken(
          grammar::here(), util::mkptr(new grammar::BoolLiteral(grammar::here(), false)), "false");
    }
    |
    INT_LITERAL
    {
        std::string image(util::replace_all(yytext, "_", ""));
        $$ = new grammar::FactorToken(
            grammar::here(), util::mkptr(new grammar::IntLiteral(grammar::here(), image)), yytext);
    }
    |
    DOUBLE_LITERAL
    {
        std::string image(util::replace_all(yytext, "_", ""));
        $$ = new grammar::FactorToken(
           grammar::here(), util::mkptr(new grammar::FloatLiteral(grammar::here(), image)), yytext);
    }
    |
    STRING_LITERAL
    {
        std::string image(util::comprehend(yytext, 1, -1));
        $$ = new grammar::FactorToken(grammar::here(), util::mkptr(
                                    new grammar::StringLiteral(grammar::here(), image)), image);
        grammar::lineno += std::count(yytext + 1, yytext + strlen(yytext) - 1, '\n');
    }
    |
    TRIPLE_QUOTED_STRING_LITERAL
    {
        std::string image(util::comprehend(yytext, 3, -3));
        $$ = new grammar::FactorToken(grammar::here(), util::mkptr(
                                    new grammar::StringLiteral(grammar::here(), image)), image);
        grammar::lineno += std::count(yytext + 3, yytext + strlen(yytext) - 3, '\n');
    }
    |
    ident
    {
        misc::position here(grammar::here());
        std::string id($1->deliver());
        $$ = new grammar::FactorToken(here, util::mkptr(new grammar::Identifier(here, id)), id);
    }
    |
    PIPE_ELEMENT
    {
        misc::position here(grammar::here());
        $$ = new grammar::FactorToken(here, util::mkptr(new grammar::PipeElement(here)), yytext);
    }
    |
    PIPE_INDEX
    {
        misc::position here(grammar::here());
        $$ = new grammar::FactorToken(here, util::mkptr(new grammar::PipeIndex(here)), yytext);
    }
    |
    PIPE_KEY
    {
        misc::position here(grammar::here());
        $$ = new grammar::FactorToken(here, util::mkptr(new grammar::PipeKey(here)), yytext);
    }
    |
    PIPE_RESULT
    {
        misc::position here(grammar::here());
        $$ = new grammar::FactorToken(here, util::mkptr(new grammar::PipeResult(here)), yytext);
    }
    |
    REGULAR_ASYNC_PARAM
    {
        misc::position here(grammar::here());
        $$ = new grammar::FactorToken(
                            here, util::mkptr(new grammar::RegularAsyncParam(here)), yytext);
    }
    |
    '('
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::OPEN_PAREN);
    }
    |
    '['
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::OPEN_BRACKET);
    }
    |
    '{'
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::OPEN_BRACE);
    }
    |
    '}'
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::CLOSE_BRACE);
    }
    |
    ']'
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::CLOSE_BRACKET);
    }
    |
    ')'
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::CLOSE_PAREN);
    }
    |
    ':'
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::COLON);
    }
    |
    ','
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::COMMA);
    }
    |
    '@'
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::THIS);
    }
;

name_list:
    additional_name ident
    {
        $$ = $1->add($2->deliver());
    }
;

additional_name:
    name_list ','
    {
        $$ = $1;
    }
    |
    {
        $$ = new grammar::NameList;
    }
;

ifnot_clue:
    indent KW_IFNOT token_sequence eol
    {
        grammar::builder.addIfnot($1, misc::position($4), $3->deliver());
    }
;

ident:
    IDENT
    {
        $$ = new grammar::Ident(grammar::here(), yytext);
    }
;
