%{
#include "syn-include.h"
%}

%union {
    int indent_type;
    int line_num_type;

    grammar::OpImage* op_type;
    grammar::Ident* ident_type;
    grammar::NameList* names_type;
    grammar::TokenSequence* expr_seq_type;
    grammar::Token* expr_token_type;
}

%type <indent_type> indent

%type <line_num_type> eol

%type <ident_type> ident

%type <names_type> name_list
%type <names_type> additional_name
%type <names_type> param_list
%type <names_type> member_name

%type <expr_token_type> expr_token
%type <expr_seq_type> expr_sequence

%type <op_type> op

%token INDENT EOL
%token KW_FUNC KW_IF KW_IFNOT KW_ELSE KW_RETURN KW_IMPORT KW_EXPORT KW_RESERVED
%token LIST_APPEND LE GE NE AND OR PIPE_MAP PIPE_FILTER PROP_SEP
%token KW_TYPEOF BOOL_TRUE BOOL_FALSE
%token INT_LITERAL DOUBLE_LITERAL STRING_LITERAL TRIPLE_QUOTED_STRING_LITERAL
%token IDENT
%token PIPE_ELEMENT PIPE_INDEX PIPE_KEY

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
    stmt_list clue {}
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
;

clue:
    func_clue {}
    |
    if_clue {}
    |
    ifnot_clue {}
    |
    else_clue {}
;

arithmetics:
    indent expr_sequence eol
    {
        grammar::builder.addArith($1, misc::position($3), $2->deliver());
    }
;

func_return:
    indent KW_RETURN expr_sequence eol
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
    indent KW_EXPORT member_name ':' expr_sequence eol
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

func_clue:
    indent KW_FUNC ident '(' param_list ')' eol
    {
        grammar::builder.addFunction($1, misc::position($7), $3->deliver(), $5->deliver());
    }
;

expr_sequence:
    expr_sequence expr_token
    {
        $$ = $1->add($2);
    }
    |
    expr_token
    {
        $$ = new grammar::TokenSequence($1);
    }
;

expr_token:
    '.'
    {
        $$ = new grammar::OpToken(grammar::here(), ".");
    }
    |
    '!'
    {
        $$ = new grammar::OpToken(grammar::here(), "!");
    }
    |
    AND
    {
        $$ = new grammar::OpToken(grammar::here(), "&&");
    }
    |
    OR
    {
        $$ = new grammar::OpToken(grammar::here(), "||");
    }
    |
    KW_TYPEOF
    {
        $$ = new grammar::OpToken(grammar::here(), "typeof");
    }
    |
    op
    {
        $$ = new grammar::OpToken(grammar::here(), $1->deliver());
    }
    |
    LIST_APPEND
    {
        $$ = new grammar::OpToken(grammar::here(), "++");
    }
    |
    PIPE_MAP
    {
        $$ = new grammar::OpToken(grammar::here(), "|:");
    }
    |
    PIPE_FILTER
    {
        $$ = new grammar::OpToken(grammar::here(), "|?");
    }
    |
    PROP_SEP
    {
        $$ = new grammar::PropertySeparatorToken(grammar::here());
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
    '('
    {
        $$ = new grammar::OpenParenToken(grammar::here());
    }
    |
    '['
    {
        $$ = new grammar::OpenBracketToken(grammar::here());
    }
    |
    '{'
    {
        $$ = new grammar::OpenBraceToken(grammar::here());
    }
    |
    '}'
    {
        $$ = new grammar::CloserToken(grammar::here(), "}");
    }
    |
    ']'
    {
        $$ = new grammar::CloserToken(grammar::here(), "]");
    }
    |
    ')'
    {
        $$ = new grammar::CloserToken(grammar::here(), ")");
    }
    |
    ':'
    {
        $$ = new grammar::ColonToken(grammar::here());
    }
    |
    ','
    {
        $$ = new grammar::CommaToken(grammar::here());
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

param_list:
    name_list
    {
        $$ = $1;
    }
    |
    {
        $$ = new grammar::NameList;
    }
;

if_clue:
    indent KW_IF expr_sequence eol
    {
        grammar::builder.addIf($1, misc::position($4), $3->deliver());
    }
;

ifnot_clue:
    indent KW_IFNOT expr_sequence eol
    {
        grammar::builder.addIfnot($1, misc::position($4), $3->deliver());
    }
;

else_clue:
    indent KW_ELSE eol
    {
        grammar::builder.addElse($1, misc::position($3));
    }
;

ident:
    IDENT
    {
        $$ = new grammar::Ident(grammar::here(), yytext);
    }
;

op:
    '<'
    {
        $$ = new grammar::OpImage(yytext);
    }
    |
    '>'
    {
        $$ = new grammar::OpImage(yytext);
    }
    |
    GE
    {
        $$ = new grammar::OpImage(yytext);
    }
    |
    LE
    {
        $$ = new grammar::OpImage(yytext);
    }
    |
    '='
    {
        $$ = new grammar::OpImage(yytext);
    }
    |
    NE
    {
        $$ = new grammar::OpImage(yytext);
    }
    |
    '+'
    {
        $$ = new grammar::OpImage(yytext);
    }
    |
    '-'
    {
        $$ = new grammar::OpImage(yytext);
    }
    |
    '*'
    {
        $$ = new grammar::OpImage(yytext);
    }
    |
    '/'
    {
        $$ = new grammar::OpImage(yytext);
    }
    |
    '%'
    {
        $$ = new grammar::OpImage(yytext);
    }
;
