%{
#include "syn-include.h"
%}

%union {
    int indent_type;
    int line_num_type;

    grammar::OpImage* op_type;
    grammar::Identifier* ident_type;
    grammar::Strings* strings_type;
    grammar::NameList* names_type;
    grammar::ArgList* args_type;
    grammar::Pipeline* pipeline_type;
    grammar::Pipeline::PipeBase* pipe_base_type;
    grammar::DictContent* dict_content_type;

    flchk::Expression* expr_node;
}

%type <indent_type> indent

%type <line_num_type> eol

%type <ident_type> ident

%type <strings_type> strings

%type <names_type> name_list
%type <names_type> additional_name
%type <names_type> param_list
%type <names_type> member_name

%type <args_type> arg_list
%type <args_type> additional_args

%type <pipeline_type> pipeline
%type <pipe_base_type> pipe_token

%type <dict_content_type> dict_content

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
%type <expr_node> call
%type <expr_node> callable
%type <expr_node> slice
%type <expr_node> slice_begin
%type <expr_node> slice_end
%type <expr_node> slice_step

%type <op_type> add_op
%type <op_type> mul_op
%type <op_type> cmp_op
%type <op_type> pm_sign

%token INDENT EOL
%token KW_FUNC KW_SET KW_IF KW_IFNOT KW_ELSE KW_RETURN KW_IMPORT KW_EXPORT
%token KW_TRUE KW_FALSE
%token LIST_APPEND LE GE NE AND OR
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
        $$ = yylineno;
        ++yylineno;
   }
;

may_break:
    eol indent {}
    |
    {}
;

stmt_list:
    stmt_list stmt {}
    |
    stmt_list clue {}
    |
    {}
;

stmt:
    indent eol {}
    |
    set_attr_value {}
    |
    name_def {}
    |
    arithmetics {}
    |
    func_return {}
    |
    import {}
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

set_attr_value:
    indent KW_SET member_name expr_root eol
    {
        misc::position here = $4->pos;
        std::vector<std::string> names = $3->deliver();

        if (names.size() == 1) {
            error::modifyName(here, names[0]);
        }
        flchk::Expression const* expr = new flchk::Reference(here, names[0]);
        for (auto i = ++names.begin(); i < names.end(); ++i) {
            expr = new flchk::MemberAccess(here, util::mkptr(expr), *i);
        }
        grammar::builder.addAttrSet($1, util::mkptr(expr), util::mkptr($4));
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

name_def:
    indent ident ':' expr_root eol
    {
        grammar::builder.addNameDef($1, $2->deliver(), util::mkptr($4));
    }
;

arithmetics:
    indent expr_root eol
    {
        grammar::builder.addArith($1, util::mkptr($2));
    }
;

func_return:
    indent KW_RETURN expr_root eol
    {
        grammar::builder.addReturn($1, util::mkptr($3));
    }
    |
    indent KW_RETURN eol
    {
        grammar::builder.addReturnNothing($1, misc::position($3));
    }
;

import:
    indent KW_IMPORT name_list eol
    {
        grammar::builder.addImport($1, misc::position($4), $3->deliver());
    }
;

func_clue:
    indent KW_FUNC ident '(' param_list ')' eol
    {
        grammar::builder.addFunction($1, misc::position($7), $3->deliver(), $5->deliver());
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
    indent KW_IF expr_root eol
    {
        grammar::builder.addIf($1, util::mkptr($3));
    }
;

ifnot_clue:
    indent KW_IFNOT expr_root eol
    {
        grammar::builder.addIfnot($1, util::mkptr($3));
    }
;

else_clue:
    indent KW_ELSE eol
    {
        grammar::builder.addElse($1, misc::position($3));
    }
;

ref:
    ident
    {
        misc::position pos($1->pos);
        $$ = new flchk::Reference(pos, $1->deliver());
    }
;

ident:
    IDENT
    {
        $$ = new grammar::Identifier(grammar::here(), yytext);
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
        $$ = new flchk::ListPipeline(grammar::here(), util::mkptr($1), $2->deliverCompile());
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
        $$ = (new grammar::Pipeline)->add(util::mkptr($2));
    }
;

pipe_token:
    KW_RETURN cond
    {
        $$ = new grammar::Pipeline::PipeMap(util::mkptr($2));
    }
    |
    KW_IF cond
    {
        $$ = new grammar::Pipeline::PipeFilter(util::mkptr($2));
    }
;

cond:
    cond OR may_break conj_cond
    {
        $$ = new flchk::Disjunction($1->pos, util::mkptr($1), util::mkptr($4));
    }
    |
    conj_cond
    {
        $$ = $1;
    }
;

conj_cond:
    conj_cond AND may_break nega_cond
    {
        $$ = new flchk::Conjunction($1->pos, util::mkptr($1), util::mkptr($4));
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
        $$ = new flchk::Negation($2->pos, util::mkptr($2));
    }
    |
    comp
    {
        $$ = $1;
    }
;

comp:
    comp cmp_op may_break expr
    {
        $$ = new flchk::BinaryOp($1->pos, util::mkptr($1), $2->deliver(), util::mkptr($4));
    }
    |
    expr
    {
        $$ = $1;
    }
;

expr:
    expr LIST_APPEND may_break term
    {
        $$ = new flchk::ListAppend($1->pos, util::mkptr($1), util::mkptr($4));
    }
    |
    expr add_op may_break term
    {
        $$ = new flchk::BinaryOp($1->pos, util::mkptr($1), $2->deliver(), util::mkptr($4));
    }
    |
    term
    {
        $$ = $1;
    }
;

term:
    term mul_op may_break unary_factor
    {
        $$ = new flchk::BinaryOp($1->pos, util::mkptr($1), $2->deliver(), util::mkptr($4));
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
        $$ = new flchk::PreUnaryOp($2->pos, $1->deliver(), util::mkptr($2));
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
        $$ = new flchk::BoolLiteral(grammar::here(), true);
    }
    |
    BOOL_FALSE
    {
        $$ = new flchk::BoolLiteral(grammar::here(), false);
    }
    |
    INT_LITERAL
    {
        $$ = new flchk::IntLiteral(grammar::here(), yytext);
    }
    |
    DOUBLE_LITERAL
    {
        $$ = new flchk::FloatLiteral(grammar::here(), yytext);
    }
    |
    strings
    {
        $$ = new flchk::StringLiteral(grammar::here(), $1->deliver());
    }
    |
    callable
    {
        $$ = $1;
    }
;

strings:
    STRING_LITERAL
    {
        $$ = new grammar::Strings(util::comprehend(yytext, 1, -1));
    }
    |
    strings STRING_LITERAL
    {
        $$ = $1->append(util::comprehend(yytext, 1, -1));
    }
;

callable:
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
    '{' may_break dict_content may_break '}'
    {
        $$ = new flchk::Dictionary(grammar::here(), $3->deliver());
    }
    |
    '{' '}'
    {
        $$ = new flchk::Dictionary(grammar::here(), std::vector<flchk::Dictionary::ItemType>());
    }
    |
    call
    {
        $$ = $1;
    }
    |
    slice
    {
        $$ = $1;
    }
    |
    LIST_ELEMENT
    {
        $$ = new flchk::ListElement(grammar::here());
    }
    |
    LIST_INDEX
    {
        $$ = new flchk::ListIndex(grammar::here());
    }
    |
    callable '.' may_break ident
    {
        misc::position pos($4->pos);
        $$ = new flchk::MemberAccess(pos, util::mkptr($1), $4->deliver());
    }
;

cmp_op:
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
;

add_op:
    '+'
    {
        $$ = new grammar::OpImage(yytext);
    }
    |
    '-'
    {
        $$ = new grammar::OpImage(yytext);
    }
;

mul_op:
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

pm_sign:
    '+'
    {
        $$ = new grammar::OpImage(yytext);
    }
    |
    '-'
    {
        $$ = new grammar::OpImage(yytext);
    }
;

list_literal:
    '[' may_break arg_list ']'
    {
        $$ = new flchk::ListLiteral(grammar::here(), $3->deliver());
    }
;

call:
    callable '(' may_break arg_list ')'
    {
        $$ = new flchk::Call($1->pos, util::mkptr($1), $4->deliver());
    }
;

slice:
    callable '[' expr_root ']'
    {
        $$ = new flchk::Lookup($1->pos, util::mkptr($1), util::mkptr($3));
    }
    |
    callable '[' slice_begin slice_end slice_step ']'
    {
        $$ = new flchk::ListSlice(
                    $1->pos, util::mkptr($1), util::mkptr($3), util::mkptr($4), util::mkptr($5));
    }
;

slice_begin:
    expr_root
    {
        $$ = $1;
    }
    |
    {
        $$ = new flchk::ListSlice::Default(grammar::here());
    }
;

slice_end:
    ':' expr_root
    {
        $$ = $2;
    }
    |
    ':'
    {
        $$ = new flchk::ListSlice::Default(grammar::here());
    }
;

slice_step:
    ':' expr_root
    {
        $$ = $2;
    }
    |
    {
        $$ = new flchk::ListSlice::Default(grammar::here());
    }
    |
    ':'
    {
        error::sliceStepOmitted(grammar::here());
        $$ = new flchk::ListSlice::Default(grammar::here());
    }
;

arg_list:
    additional_args expr_root
    {
        $$ = $1->add($2);
    }
    |
    {
        $$ = new grammar::ArgList;
    }
;

additional_args:
    additional_args expr_root ',' may_break
    {
        $$ = $1->add($2);
    }
    |
    {
        $$ = new grammar::ArgList;
    }
;

dict_content:
    dict_content ',' may_break expr_root ':' expr_root
    {
        $$ = $1->add($4, $6);
    }
    |
    expr_root ':' expr_root
    {
        $$ = (new grammar::DictContent())->add($1, $3);
    }
;
