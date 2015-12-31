%{
#include "syn-include.h"
%}

%union {
    int indent_type;
    int line_num_type;

    grammar::TokenSequence* token_sequence_type;
    grammar::Token* token_type;
}

%type <indent_type> indent
%type <line_num_type> eol

%type <token_type> token
%type <token_sequence_type> token_sequence

%token INDENT EOL
%token KW_FUNC KW_IF KW_IFNOT KW_ELSE KW_RETURN KW_EXTERN KW_EXPORT KW_RESERVED
%token KW_TRY KW_CATCH KW_THROW KW_FOR KW_BREAK KW_CONTINUE
%token OPERATOR PIPE_SEP
%token BOOL_TRUE BOOL_FALSE
%token INT_LITERAL DOUBLE_LITERAL STRING_LITERAL TRIPLE_QUOTED_STRING_LITERAL
%token REGEXP
%token IDENT
%token PIPE_ELEMENT PIPE_INDEX PIPE_KEY PIPE_RESULT EXCEPTION_OBJ REGULAR_ASYNC_PARAM
%token KW_CLASS KW_THIS KW_SUPER KW_CONSTRUCTOR

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
    indent token_sequence eol
    {
        grammar::builder.addArith($1, misc::position($3), $2->deliver());
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
    KW_RETURN
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::RETURN);
    }
    |
    KW_IF
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::IF);
    }
    |
    KW_IFNOT
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::IFNOT);
    }
    |
    KW_TRY
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::TRY);
    }
    |
    KW_CATCH
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::CATCH);
    }
    |
    KW_THROW
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::THROW);
    }
    |
    KW_FOR
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::FOR);
    }
    |
    KW_BREAK
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::BREAK);
    }
    |
    KW_CONTINUE
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::CONTINUE);
    }
    |
    KW_ELSE
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::ELSE);
    }
    |
    KW_CLASS
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::CLASS);
    }
    |
    KW_THIS
    {
        $$ = new grammar::FactorToken(
            grammar::here(), util::mkptr(new grammar::This(grammar::here())), "this");
    }
    |
    KW_SUPER
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::SUPER);
    }
    |
    KW_CONSTRUCTOR
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::CONSTRUCTOR);
    }
    |
    KW_EXTERN
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::EXTERN);
    }
    |
    KW_EXPORT
    {
        $$ = new grammar::TypedToken(grammar::here(), yytext, grammar::EXPORT);
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
                                    new grammar::StringLiteral(grammar::here(), image)), yytext);
        grammar::lineno += std::count(yytext + 1, yytext + strlen(yytext) - 1, '\n');
    }
    |
    TRIPLE_QUOTED_STRING_LITERAL
    {
        std::string image(util::comprehend(yytext, 3, -3));
        $$ = new grammar::FactorToken(grammar::here(), util::mkptr(
                                    new grammar::StringLiteral(grammar::here(), image)), yytext);
        grammar::lineno += std::count(yytext + 3, yytext + strlen(yytext) - 3, '\n');
    }
    |
    REGEXP
    {
        $$ = new grammar::FactorToken(grammar::here(), grammar::makeRegEx(
                                            grammar::here(), yytext), yytext);
        auto breaks = std::count(yytext + 1, yytext + strlen(yytext) - 1, '\n');
        if (breaks != 0) {
            error::invalidRegExp(grammar::here(), "line break in regexp;"
                                                  " add a space after / if you want a div op");
        }
        grammar::lineno += breaks;
    }
    |
    IDENT
    {
        misc::position here(grammar::here());
        $$ = new grammar::FactorToken(here, util::mkptr(
                new grammar::Identifier(here, yytext)), yytext);
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
    EXCEPTION_OBJ
    {
        misc::position here(grammar::here());
        $$ = new grammar::FactorToken(here, util::mkptr(new grammar::ExceptionObj(here)), yytext);
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
;
