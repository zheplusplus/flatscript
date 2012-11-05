#include <algorithm>

#include <semantic/func-body-filter.h>
#include <semantic/global-filter.h>
#include <semantic/symbol-def-filter.h>
#include <semantic/block.h>
#include <semantic/expr-nodes.h>
#include <semantic/list-pipe.h>
#include <semantic/stmt-nodes.h>
#include <semantic/function.h>
#include <output/list-pipe.h>
#include <output/function.h>
#include <util/string.h>

#include "test-common.h"

using namespace test;
using namespace semantic;

namespace {

    util::sref<output::Block> nulBlock()
    {
        return util::sref<output::Block>(nullptr);
    }

    util::sptr<output::Expression const> nulOutputExpr()
    {
        return util::sptr<output::Expression const>(nullptr);
    }

    struct BranchConsequence
        : Statement
    {
        BranchConsequence(misc::position const& pos, util::sptr<Expression const> p, Block c)
            : Statement(pos)
            , predicate(std::move(p))
            , consequence(std::move(c))
        {}

        void compile(util::sref<SymbolTable>, util::sref<output::Block>) const
        {
            DataTree::actualOne()(pos, BRANCH_CONSQ_ONLY);
            predicate->compile(nulSymbols());
            DataTree::actualOne()(CONSEQUENCE);
            consequence.compile(nulSymbols(), nulBlock());
        }

        util::sptr<Expression const> const predicate;
        Block const consequence;
    };

    struct BranchAlternative
        : Statement
    {
        BranchAlternative(misc::position const& pos, util::sptr<Expression const> p, Block a)
            : Statement(pos)
            , predicate(std::move(p))
            , alternative(std::move(a))
        {}

        void compile(util::sref<SymbolTable>, util::sref<output::Block>) const
        {
            DataTree::actualOne()(pos, BRANCH_ALTER_ONLY);
            predicate->compile(nulSymbols());
            DataTree::actualOne()(ALTERNATIVE);
            alternative.compile(nulSymbols(), nulBlock());
        }

        util::sptr<Expression const> const predicate;
        Block const alternative;
    };

}

SymbolTable::SymbolTable(misc::position const&
                       , util::sref<SymbolTable const>
                       , std::vector<std::string> const&)
    : ext_symbols(nullptr)
{}

util::sptr<output::Function const> Function::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, FUNC_DEF, name);
    std::for_each(param_names.begin()
                , param_names.end()
                , [&](std::string const& param)
                  {
                      DataTree::actualOne()(pos, PARAMETER, param);
                  });
    _body->compile(nulSymbols());
    return util::sptr<output::Function const>(nullptr);
}

void Block::addStmt(util::sptr<Statement const> stmt)
{
    _stmts.push_back(std::move(stmt));
}

void Block::defFunc(misc::position const& pos
                  , std::string const& name
                  , std::vector<std::string> const& param_names
                  , util::sptr<Filter> body)
{
    _funcs.push_back(util::mkptr(new Function(pos, name, param_names, std::move(body))));
}

void Block::compile(util::sref<SymbolTable>, util::sref<output::Block>) const
{
    DataTree::actualOne()(BLOCK_BEGIN);
    std::for_each(_funcs.begin()
                , _funcs.end()
                , [&](util::sptr<Function const> const& func)
                  {
                      func->compile(nulSymbols());
                  });

    std::for_each(_stmts.begin()
                , _stmts.end()
                , [&](util::sptr<Statement const> const& stmt)
                  {
                      stmt->compile(nulSymbols(), nulBlock());
                  });
    DataTree::actualOne()(BLOCK_END);
}

void Filter::addReturn(misc::position const& pos, util::sptr<Expression const> ret_val)
{
    _block.addStmt(util::mkptr(new Return(pos, std::move(ret_val))));
}

void Filter::addReturnNothing(misc::position const& pos)
{
    _block.addStmt(util::mkptr(new ReturnNothing(pos)));
}

void Filter::addArith(misc::position const& pos, util::sptr<Expression const> expr)
{
    _block.addStmt(util::mkptr(new Arithmetics(pos, std::move(expr))));
}

void Filter::addImport(misc::position const& pos, std::vector<std::string> const& names)
{
    _block.addStmt(util::mkptr(new Import(pos, names)));
}

void Filter::addExport(misc::position const& pos
                     , std::vector<std::string> const& export_point
                     , util::sptr<Expression const> value)
{
    _block.addStmt(util::mkptr(new Export(pos, export_point, std::move(value))));
}

void Filter::addAttrSet(misc::position const& pos
                      , util::sptr<Expression const> set_point
                      , util::sptr<Expression const> value)
{
    _block.addStmt(util::mkptr(new AttrSet(pos, std::move(set_point), std::move(value))));
}

void Filter::addBranch(misc::position const& pos
                     , util::sptr<Expression const> predicate
                     , util::sptr<Filter> consequence
                     , util::sptr<Filter> alternative)
{
    _block.addStmt(util::mkptr(new Branch(pos
                                        , std::move(predicate)
                                        , std::move(consequence->_block)
                                        , std::move(alternative->_block))));
}

void Filter::addBranch(misc::position const& pos
                     , util::sptr<Expression const> predicate
                     , util::sptr<Filter> consequence)
{
    _block.addStmt(util::mkptr(
                new BranchConsequence(pos, std::move(predicate), std::move(consequence->_block))));
}

void Filter::addBranchAlterOnly(misc::position const& pos
                              , util::sptr<Expression const> predicate
                              , util::sptr<Filter> alternative)
{
    _block.addStmt(util::mkptr(
                new BranchAlternative(pos, std::move(predicate), std::move(alternative->_block))));
}

util::sptr<output::Statement const> Filter::compile(util::sref<SymbolTable>) const
{
    _block.compile(nulSymbols(), nulBlock());
    return util::sptr<output::Statement const>(nullptr);
}

void FuncBodyFilter::defName(misc::position const& pos
                           , std::string const& name
                           , util::sptr<Expression const> init)
{
    _block.addStmt(util::mkptr(new NameDef(pos, name, std::move(init))));
}

void FuncBodyFilter::defFunc(misc::position const& pos
                           , std::string const& name
                           , std::vector<std::string> const& param_names
                           , util::sptr<Filter> body)
{
    _block.defFunc(pos, name, param_names, std::move(body));
}

void SymbolDefFilter::defName(misc::position const& pos
                            , std::string const& name
                            , util::sptr<Expression const> init)
{
    _block.addStmt(util::mkptr(new NameDef(pos, name + NAME_DEF_FILTERED, std::move(init))));
}

void SymbolDefFilter::defFunc(misc::position const& pos
                            , std::string const& name
                            , std::vector<std::string> const& param_names
                            , util::sptr<Filter> body)
{
    _block.defFunc(pos, name + FUNC_DEF_FILTERED, param_names, std::move(body));
}

util::sptr<output::Statement const> GlobalFilter::compile(util::sref<SymbolTable> st) const
{
    return Filter::compile(st);
}

void Arithmetics::compile(util::sref<SymbolTable>, util::sref<output::Block>) const
{
    DataTree::actualOne()(pos, ARITHMETICS);
    expr->compile(nulSymbols());
}

void Branch::compile(util::sref<SymbolTable>, util::sref<output::Block>) const
{
    DataTree::actualOne()(pos, BRANCH);
    predicate->compile(nulSymbols());
    DataTree::actualOne()(CONSEQUENCE);
    consequence.compile(nulSymbols(), nulBlock());
    DataTree::actualOne()(ALTERNATIVE);
    alternative.compile(nulSymbols(), nulBlock());
}

void NameDef::compile(util::sref<SymbolTable>, util::sref<output::Block>) const
{
    DataTree::actualOne()(pos, NAME_DEF, name);
    init->compile(nulSymbols());
}

void Export::compile(util::sref<SymbolTable>, util::sref<output::Block>) const
{
    DataTree::actualOne()(pos, EXPORT);
    std::for_each(export_point.begin()
                , export_point.end()
                , [&](std::string const& name)
                  {
                      DataTree::actualOne()(pos, IDENTIFIER, name);
                  });
    DataTree::actualOne()(pos, EXPORT_VALUE);
    value->compile(nulSymbols());
}

void Import::compile(util::sref<SymbolTable>, util::sref<output::Block>) const
{
    DataTree::actualOne()(pos, IMPORT);
    std::for_each(names.begin()
                , names.end()
                , [&](std::string const& name)
                  {
                      DataTree::actualOne()(pos, IDENTIFIER, name);
                  });
}

void AttrSet::compile(util::sref<SymbolTable>, util::sref<output::Block>) const
{
    DataTree::actualOne()(pos, ATTR_SET);
    set_point->compile(nulSymbols());
    value->compile(nulSymbols());
}

void Return::compile(util::sref<SymbolTable>, util::sref<output::Block>) const
{
    DataTree::actualOne()(pos, RETURN);
    ret_val->compile(nulSymbols());
}

void ReturnNothing::compile(util::sref<SymbolTable>, util::sref<output::Block>) const
{
    DataTree::actualOne()(pos, RETURN_NOTHING);
}

util::sptr<output::Expression const> PreUnaryOp::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, PRE_UNARY_OP, op_img)(pos, OPERAND);
    rhs->compile(nulSymbols());
    return nulOutputExpr();
}

util::sptr<output::Expression const> BinaryOp::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, BINARY_OP, op_img)(pos, OPERAND);
    lhs->compile(nulSymbols());
    DataTree::actualOne()(pos, OPERAND);
    rhs->compile(nulSymbols());
    return nulOutputExpr();
}

util::sptr<output::Expression const> Reference::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, IDENTIFIER, name);
    return nulOutputExpr();
}

util::sptr<output::Expression const> BoolLiteral::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, BOOLEAN, util::str(value));
    return nulOutputExpr();
}

util::sptr<output::Expression const> IntLiteral::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, INTEGER, util::str(value));
    return nulOutputExpr();
}

util::sptr<output::Expression const> FloatLiteral::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, FLOATING, util::str(value));
    return nulOutputExpr();
}

util::sptr<output::Expression const> StringLiteral::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, STRING, value);
    return nulOutputExpr();
}

static void compileList(std::vector<util::sptr<Expression const>> const& values)
{
    std::for_each(values.begin()
                , values.end()
                , [&](util::sptr<Expression const> const& v)
                  {
                      v->compile(nulSymbols());
                  });
}

util::sptr<output::Expression const> ListLiteral::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, LIST_BEGIN);
    compileList(value);
    DataTree::actualOne()(pos, LIST_END);
    return nulOutputExpr();
}

util::sptr<output::Expression const> ListElement::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, LIST_ELEMENT);
    return nulOutputExpr();
}

util::sptr<output::Expression const> ListIndex::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, LIST_INDEX);
    return nulOutputExpr();
}

util::sptr<output::Expression const> ListAppend::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, BINARY_OP, "++")(pos, OPERAND);
    lhs->compile(nulSymbols());
    DataTree::actualOne()(pos, OPERAND);
    rhs->compile(nulSymbols());
    return nulOutputExpr();
}

util::sptr<output::Expression const> Call::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, CALL_BEGIN);
    callee->compile(nulSymbols());
    DataTree::actualOne()(pos, ARGUMENTS);
    compileList(args);
    DataTree::actualOne()(pos, CALL_END);
    return nulOutputExpr();
}

util::sptr<output::Expression const> MemberAccess::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, BINARY_OP, ".")(pos, OPERAND);
    referee->compile(nulSymbols());
    DataTree::actualOne()(pos, OPERAND);
    DataTree::actualOne()(pos, IDENTIFIER, member);
    return nulOutputExpr();
}

util::sptr<output::Expression const> Lookup::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, BINARY_OP, "[]")(pos, OPERAND);
    collection->compile(nulSymbols());
    DataTree::actualOne()(pos, OPERAND);
    key->compile(nulSymbols());
    return nulOutputExpr();
}

util::sptr<output::Expression const> ListSlice::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, LIST_SLICE_BEGIN);
    list->compile(nulSymbols());
    begin->compile(nulSymbols());
    end->compile(nulSymbols());
    step->compile(nulSymbols());
    DataTree::actualOne()(pos, LIST_SLICE_END);
    return nulOutputExpr();
}

util::sptr<output::Expression const> ListSlice::Default::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, LIST_SLICE_DEFAULT);
    return nulOutputExpr();
}

util::sptr<output::Expression const> Dictionary::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, DICT_BEGIN);
    std::for_each(items.begin()
                , items.end()
                , [&](ItemType const& item)
                  {
                      DataTree::actualOne()(pos, DICT_ITEM);
                      item.first->compile(nulSymbols());
                      item.second->compile(nulSymbols());
                  });
    DataTree::actualOne()(pos, DICT_END);
    return nulOutputExpr();
}

util::sptr<output::Expression const> Lambda::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, FUNC_DEF);
    std::for_each(param_names.begin()
                , param_names.end()
                , [&](std::string const& param)
                  {
                      DataTree::actualOne()(pos, PARAMETER, param);
                  });
    body->compile(nulSymbols());
    return nulOutputExpr();
}

util::sptr<output::Expression const> ListPipeMapper::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, BINARY_OP, "|:")(pos, OPERAND);
    list->compile(nulSymbols());
    DataTree::actualOne()(pos, OPERAND);
    mapper->compile(nulSymbols());
    return nulOutputExpr();
}

util::sptr<output::Expression const> ListPipeFilter::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, BINARY_OP, "|?")(pos, OPERAND);
    list->compile(nulSymbols());
    DataTree::actualOne()(pos, OPERAND);
    filter->compile(nulSymbols());
    return nulOutputExpr();
}

bool Expression::isLiteral(util::sref<SymbolTable const>) const { return false; }
bool Expression::boolValue(util::sref<SymbolTable const>) const { return false; }
std::string Expression::literalType(util::sref<SymbolTable const>) const { return ""; }
mpz_class Expression::intValue(util::sref<SymbolTable const>) const { return 0; }
mpf_class Expression::floatValue(util::sref<SymbolTable const>) const { return 0; }
std::string Expression::stringValue(util::sref<SymbolTable const>) const { return ""; }
bool PreUnaryOp::isLiteral(util::sref<SymbolTable const>) const { return false; }
std::string PreUnaryOp::literalType(util::sref<SymbolTable const>) const { return ""; }
bool PreUnaryOp::boolValue(util::sref<SymbolTable const>) const { return false; }
mpz_class PreUnaryOp::intValue(util::sref<SymbolTable const>) const { return 0; }
mpf_class PreUnaryOp::floatValue(util::sref<SymbolTable const>) const { return 0; }
std::string PreUnaryOp::stringValue(util::sref<SymbolTable const>) const { return ""; }
bool Reference::isLiteral(util::sref<SymbolTable const>) const { return false; }
std::string Reference::literalType(util::sref<SymbolTable const>) const { return ""; }
bool Reference::boolValue(util::sref<SymbolTable const>) const { return false; }
mpz_class Reference::intValue(util::sref<SymbolTable const>) const { return 0; }
mpf_class Reference::floatValue(util::sref<SymbolTable const>) const { return 0; }
std::string Reference::stringValue(util::sref<SymbolTable const>) const { return ""; }
bool BinaryOp::isLiteral(util::sref<SymbolTable const>) const { return false; }
std::string BinaryOp::literalType(util::sref<SymbolTable const>) const { return ""; }
bool BinaryOp::boolValue(util::sref<SymbolTable const>) const { return false; }
mpz_class BinaryOp::intValue(util::sref<SymbolTable const>) const { return 0; }
mpf_class BinaryOp::floatValue(util::sref<SymbolTable const>) const { return 0; }
std::string BinaryOp::stringValue(util::sref<SymbolTable const>) const { return ""; }
bool BoolLiteral::isLiteral(util::sref<SymbolTable const>) const { return false; }
bool BoolLiteral::boolValue(util::sref<SymbolTable const>) const { return false; }
std::string BoolLiteral::literalType(util::sref<SymbolTable const>) const { return ""; }
bool IntLiteral::isLiteral(util::sref<SymbolTable const>) const { return false; }
std::string IntLiteral::literalType(util::sref<SymbolTable const>) const { return ""; }
mpz_class IntLiteral::intValue(util::sref<SymbolTable const>) const { return 0; }
bool FloatLiteral::isLiteral(util::sref<SymbolTable const>) const { return false; }
std::string FloatLiteral::literalType(util::sref<SymbolTable const>) const { return ""; }
mpf_class FloatLiteral::floatValue(util::sref<SymbolTable const>) const { return 0; }
bool StringLiteral::isLiteral(util::sref<SymbolTable const>) const { return false; }
std::string StringLiteral::literalType(util::sref<SymbolTable const>) const { return ""; }
std::string StringLiteral::stringValue(util::sref<SymbolTable const>) const { return ""; }
