%{
#include "syn-include.h"
%}

%union {
    int indent_type;
    int line_num_type;

    parser::OpImage* op_type;
    parser::Identifier* ident_type;
    parser::Strings* strings_type;
    parser::ParamNames* param_names_type;
    parser::ArgList* args_type;
    parser::Pipeline* pipeline_type;
    parser::Pipeline::PipeBase* pipe_base_type;

    grammar::Expression* expr_node;
    grammar::Call* call_node;
}

%type <indent_type> indent

%type <line_num_type> eol

%type <ident_type> ident

%type <strings_type> strings

%type <param_names_type> param_list
%type <param_names_type> additional_params

%type <args_type> arg_list
%type <args_type> additional_args

%type <pipeline_type> pipeline
%type <pipe_base_type> pipe_token

%type <expr_node> expr_root
%type <expr_node> list_pipe
%type <expr_node> cond
%type <expr_node> conj_cond
%type <expr_node> nega_cond
%type <expr_node> comp
%type <expr_node> expr
%type <expr_node> term
%type <expr_node> unary_factor
%type <expr_node> factor
%type <expr_node> ref
%type <expr_node> list_literal
%type <expr_node> member_access
%type <call_node> call

%type <op_type> add_op
%type <op_type> mul_op
%type <op_type> cmp_op
%type <op_type> pm_sign

%token INDENT EOL
%token KW_FUNC KW_IF KW_IFNOT KW_ELSE KW_RETURN
%token KW_TRUE KW_FALSE
%token LIST_APPEND LE GE NE AND OR
%token COMMA COLON
%token BOOL_TRUE BOOL_FALSE
%token INT_LITERAL DOUBLE_LITERAL STRING_LITERAL
%token IDENT
%token LIST_ELEMENT LIST_INDEX

%%

root:
    stmt_list
;

indent:
    INDENT
    {
        $$ = parser::last_indent;
    }
    |
    {
        $$ = 0;
    }
;

eol:
   EOL
   {
        $$ = yylineno;
        ++yylineno;
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
    INDENT eol {}
    |
    eol {}
    |
    var_def {}
    |
    arithmetics {}
    |
    func_return {}
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

var_def:
    indent ident ':' expr_root eol
    {
        parser::builder.addVarDef($1, $2->id, util::mkptr($4));
        delete $2;
    }
;

arithmetics:
    indent expr_root eol
    {
        parser::builder.addArith($1, util::mkptr($2));
    }
;

func_return:
    indent KW_RETURN expr_root eol
    {
        parser::builder.addReturn($1, util::mkptr($3));
    }
    |
    indent KW_RETURN eol
    {
        parser::builder.addReturnNothing($1, parser::here($3));
    }
;

func_clue:
    indent KW_FUNC ident '(' param_list ')' eol
    {
        parser::builder.addFunction($1, parser::here($7), $3->id, $5->get());
        delete $3;
        delete $5;
    }
;

param_list:
    additional_params ident
    {
        $$ = $1->add($2->id);
        delete $2;
    }
    |
    {
        $$ = new parser::ParamNames;
    }
;

additional_params:
    param_list ','
    {
        $$ = $1;
    }
    |
    {
        $$ = new parser::ParamNames;
    }
;

if_clue:
    indent KW_IF expr_root eol
    {
        parser::builder.addIf($1, util::mkptr($3));
    }
;

ifnot_clue:
    indent KW_IFNOT expr_root eol
    {
        parser::builder.addIfnot($1, util::mkptr($3));
    }
;

else_clue:
    indent KW_ELSE eol
    {
        parser::builder.addElse($1, parser::here($3));
    }
;

ref:
    ident
    {
        $$ = new grammar::Reference($1->pos, $1->id);
        delete $1;
    }
;

ident:
    IDENT
    {
        $$ = new parser::Identifier(parser::here(), yytext);
    }
;

expr_root:
    list_pipe
    {
        $$ = $1;
    }

list_pipe:
    cond pipeline
    {
        $$ = new grammar::ListPipeline(parser::here(), util::mkptr($1), $2->deliverCompile());
        delete $2;
    }
    |
    cond
    {
        $$ = $1;
    }
;

pipeline:
    pipeline '|' pipe_token
    {
        $$ = $1->add(util::mkptr($3));
    }
    |
    '|' pipe_token
    {
        $$ = (new parser::Pipeline)->add(util::mkptr($2));
    }
;

pipe_token:
    KW_RETURN cond
    {
        $$ = new parser::Pipeline::PipeMap(util::mkptr($2));
    }
    |
    KW_IF cond
    {
        $$ = new parser::Pipeline::PipeFilter(util::mkptr($2));
    }
;

cond:
    cond OR conj_cond
    {
        $$ = new grammar::Disjunction($1->pos, util::mkptr($1), util::mkptr($3));
    }
    |
    conj_cond
    {
        $$ = $1;
    }
;

conj_cond:
    conj_cond AND nega_cond
    {
        $$ = new grammar::Conjunction($1->pos, util::mkptr($1), util::mkptr($3));
    }
    |
    nega_cond
    {
        $$ = $1;
    }
;

nega_cond:
    '!' comp
    {
        $$ = new grammar::Negation($2->pos, util::mkptr($2));
    }
    |
    comp
    {
        $$ = $1;
    }
;

comp:
    comp cmp_op expr
    {
        $$ = new grammar::BinaryOp($1->pos, util::mkptr($1), $2->img, util::mkptr($3));
        delete $2;
    }
    |
    expr
    {
        $$ = $1;
    }
;

expr:
    expr LIST_APPEND term
    {
        $$ = new grammar::ListAppend($1->pos, util::mkptr($1), util::mkptr($3));
    }
    |
    expr add_op term
    {
        $$ = new grammar::BinaryOp($1->pos, util::mkptr($1), $2->img, util::mkptr($3));
        delete $2;
    }
    |
    term
    {
        $$ = $1;
    }
;

term:
    term mul_op unary_factor
    {
        $$ = new grammar::BinaryOp($1->pos, util::mkptr($1), $2->img, util::mkptr($3));
        delete $2;
    }
    |
    unary_factor
    {
        $$ = $1;
    }
;

unary_factor:
    pm_sign factor
    {
        $$ = new grammar::PreUnaryOp($2->pos, $1->img, util::mkptr($2));
        delete $1;
    }
    |
    factor
    {
        $$ = $1;
    }
;

factor:
    BOOL_TRUE
    {
        $$ = new grammar::BoolLiteral(parser::here(), true);
    }
    |
    BOOL_FALSE
    {
        $$ = new grammar::BoolLiteral(parser::here(), false);
    }
    |
    INT_LITERAL
    {
        $$ = new grammar::IntLiteral(parser::here(), yytext);
    }
    |
    DOUBLE_LITERAL
    {
        $$ = new grammar::FloatLiteral(parser::here(), yytext);
    }
    |
    strings
    {
        $$ = new grammar::StringLiteral($1->pos, $1->deliver());
        delete $1;
    }
    |
    member_access
    {
        $$ = $1;
    }
    |
    LIST_ELEMENT
    {
        $$ = new grammar::ListElement(parser::here());
    }
    |
    LIST_INDEX
    {
        $$ = new grammar::ListIndex(parser::here());
    }
;

strings:
    STRING_LITERAL
    {
        $$ = new parser::Strings(parser::here(), util::comprehend(yytext, 1, -1));
    }
    |
    strings STRING_LITERAL
    {
        $$ = $1->append(util::comprehend(yytext, 1, -1));
    }
;

member_access:
    ref
    {
        $$ = $1;
    }
    |
    list_literal
    {
        $$ = $1;
    }
    |
    '(' expr_root ')'
    {
        $$ = $2;
    }
    |
    call
    {
        $$ = $1;
    }
;

cmp_op:
    '<'
    {
        $$ = new parser::OpImage(yytext);
    }
    |
    '>'
    {
        $$ = new parser::OpImage(yytext);
    }
    |
    GE
    {
        $$ = new parser::OpImage(yytext);
    }
    |
    LE
    {
        $$ = new parser::OpImage(yytext);
    }
    |
    '='
    {
        $$ = new parser::OpImage(yytext);
    }
    |
    NE
    {
        $$ = new parser::OpImage(yytext);
    }
;

add_op:
    '+'
    {
        $$ = new parser::OpImage(yytext);
    }
    |
    '-'
    {
        $$ = new parser::OpImage(yytext);
    }
;

mul_op:
    '*'
    {
        $$ = new parser::OpImage(yytext);
    }
    |
    '/'
    {
        $$ = new parser::OpImage(yytext);
    }
    |
    '%'
    {
        $$ = new parser::OpImage(yytext);
    }
;

pm_sign:
    '+'
    {
        $$ = new parser::OpImage(yytext);
    }
    |
    '-'
    {
        $$ = new parser::OpImage(yytext);
    }
;

list_literal:
    '[' arg_list ']'
    {
        $$ = new grammar::ListLiteral(parser::here(), $2->deliver());
        delete $2;
    }
;

call:
    ident '(' arg_list ')'
    {
        $$ = new grammar::Call($1->pos, $1->id, $3->deliver());
        delete $1;
        delete $3;
    }
;

arg_list:
    additional_args expr_root
    {
        $$ = $1->add($2);
    }
    |
    {
        $$ = new parser::ArgList; 
    }
;

additional_args:
    additional_args expr_root ','
    {
        $$ = $1->add($2);
    }
    |
    {
        $$ = new parser::ArgList; 
    }
;
