#include <algorithm>

#include <semantic/block.h>
#include <semantic/stmt-nodes.h>
#include <semantic/expr-nodes.h>
#include <semantic/list-pipe.h>
#include <semantic/loop.h>
#include <semantic/function.h>
#include <output/list-pipe.h>
#include <output/function.h>
#include <util/string.h>

#include "test-common.h"

using namespace test;
using namespace semantic;

namespace {

    util::sptr<output::Expression const> nulOutputExpr()
    {
        return util::sptr<output::Expression const>(nullptr);
    }

    util::sptr<output::Statement const> nulOutputStmt()
    {
        return util::sptr<output::Statement const>(nullptr);
    }

}

Constructor::Constructor(misc::position const& ps, std::vector<std::string> params
                       , util::sptr<Statement const> b, std::string const&
                       , bool, util::ptrarr<Expression const>)
    : pos(ps)
    , param_names(std::move(params))
    , super_init(nullptr)
    , body(std::move(b))
{}

void Class::compile(util::sref<Scope>) const {}

util::sptr<output::Function const> Function::compile(util::sref<SymbolTable>, bool) const
{
    DataTree::actualOne()(pos, FUNC_DEF, name);
    std::for_each(param_names.begin()
                , param_names.end()
                , [&](std::string const& param)
                  {
                      DataTree::actualOne()(pos, PARAMETER, param);
                  });
    body->compile(nulScope());
    return util::sptr<output::Function const>(nullptr);
}

util::sptr<output::Function const> RegularAsyncFunction::compile(
        util::sref<SymbolTable>, bool) const
{
    DataTree::actualOne()(pos, REGULAR_ASYNC_PARAM_INDEX, async_param_index);
    return Function::compile(nulSymbols(), false);
}

void Block::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(BLOCK_BEGIN);
    _funcs.iter([&](util::sptr<Function const> const& func, int)
                {
                    func->compile(nulSymbols());
                });
    _stmts.iter([&](util::sptr<Statement const> const& stmt, int)
                {
                    stmt->compile(nulScope());
                });
    DataTree::actualOne()(BLOCK_END);
}

void Arithmetics::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, ARITHMETICS);
    expr->compile(nulScope());
}

void Branch::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, BRANCH);
    predicate->compile(nulScope());
    DataTree::actualOne()(CONSEQUENCE);
    consequence->compile(nulScope());
    DataTree::actualOne()(ALTERNATIVE);
    alternative->compile(nulScope());
}

void NameDef::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, NAME_DEF, name);
    init->compile(nulScope());
}

void Export::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, EXPORT);
    std::for_each(export_point.begin()
                , export_point.end()
                , [&](std::string const& name)
                  {
                      DataTree::actualOne()(pos, IDENTIFIER, name);
                  });
    DataTree::actualOne()(pos, EXPORT_VALUE);
    value->compile(nulScope());
}

void Extern::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, EXTERN);
    std::for_each(names.begin()
                , names.end()
                , [&](std::string const& name)
                  {
                      DataTree::actualOne()(pos, IDENTIFIER, name);
                  });
}

void AttrSet::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, ATTR_SET);
    set_point->compile(nulScope());
    value->compile(nulScope());
}

void Return::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, RETURN);
    ret_val->compile(nulScope());
}

void ExceptionStall::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, TRY);
    try_block->compile(nulScope());
    DataTree::actualOne()(pos, CATCH);
    catch_block->compile(nulScope());
}

void Throw::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, THROW);
    exception->compile(nulScope());
}

void RangeIteration::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, FOR_RANGE, this->reference);
    this->begin->compile(nulScope());
    this->end->compile(nulScope());
    this->step->compile(nulScope());
    this->loop->compile(nulScope());
}

void Break::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, BREAK);
}

void Continue::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, CONTINUE);
}

util::sptr<output::Expression const> PreUnaryOp::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, PRE_UNARY_OP, op_img)(pos, OPERAND);
    rhs->compile(nulScope());
    return nulOutputExpr();
}

util::sptr<output::Expression const> BinaryOp::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, BINARY_OP, op_img)(pos, OPERAND);
    lhs->compile(nulScope());
    DataTree::actualOne()(pos, OPERAND);
    rhs->compile(nulScope());
    return nulOutputExpr();
}

util::sptr<output::Expression const> TypeOf::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, PRE_UNARY_OP, "[ typeof ]")(pos, OPERAND);
    expr->compile(nulScope());
    return nulOutputExpr();
}

util::sptr<output::Expression const> Reference::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, IDENTIFIER, name);
    return nulOutputExpr();
}

util::sptr<output::Expression const> BoolLiteral::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, BOOLEAN, util::str(value));
    return nulOutputExpr();
}

util::sptr<output::Expression const> IntLiteral::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, INTEGER, util::str(value));
    return nulOutputExpr();
}

util::sptr<output::Expression const> FloatLiteral::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, FLOATING, util::str(value));
    return nulOutputExpr();
}

util::sptr<output::Expression const> StringLiteral::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, STRING, value);
    return nulOutputExpr();
}

util::sptr<output::Expression const> RegEx::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, REGEXP, value);
    return nulOutputExpr();
}

static void compileList(util::ptrarr<Expression const> const& values)
{
    values.iter([&](util::sptr<Expression const> const& v, int)
                {
                    v->compile(nulScope());
                });
}

util::sptr<output::Expression const> ListLiteral::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, LIST_BEGIN);
    compileList(value);
    DataTree::actualOne()(pos, LIST_END);
    return nulOutputExpr();
}

util::sptr<output::Expression const> PipeElement::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, PIPE_ELEMENT);
    return nulOutputExpr();
}

util::sptr<output::Expression const> PipeIndex::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, PIPE_INDEX);
    return nulOutputExpr();
}

util::sptr<output::Expression const> PipeKey::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, PIPE_KEY);
    return nulOutputExpr();
}

util::sptr<output::Expression const> PipeResult::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, PIPE_RESULT);
    return nulOutputExpr();
}

util::sptr<output::Expression const> ListAppend::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, BINARY_OP, "[ ++ ]")(pos, OPERAND);
    lhs->compile(nulScope());
    DataTree::actualOne()(pos, OPERAND);
    rhs->compile(nulScope());
    return nulOutputExpr();
}

util::sptr<output::Expression const> Call::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, CALL_BEGIN);
    callee->compile(nulScope());
    DataTree::actualOne()(pos, ARGUMENTS);
    compileList(args);
    DataTree::actualOne()(pos, CALL_END);
    return nulOutputExpr();
}

util::sptr<output::Expression const> SuperConstructorCall::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, CALL_BEGIN, "SUPER");
    DataTree::actualOne()(pos, ARGUMENTS);
    compileList(args);
    DataTree::actualOne()(pos, CALL_END);
    return nulOutputExpr();
}

util::sptr<output::Expression const> MemberAccess::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, BINARY_OP, "[ . ]")(pos, OPERAND);
    referee->compile(nulScope());
    DataTree::actualOne()(pos, OPERAND);
    DataTree::actualOne()(pos, IDENTIFIER, member);
    return nulOutputExpr();
}

util::sptr<output::Expression const> Lookup::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, BINARY_OP, "[]")(pos, OPERAND);
    collection->compile(nulScope());
    DataTree::actualOne()(pos, OPERAND);
    key->compile(nulScope());
    return nulOutputExpr();
}

util::sptr<output::Expression const> ListSlice::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, LIST_SLICE_BEGIN);
    list->compile(nulScope());
    begin->compile(nulScope());
    end->compile(nulScope());
    step->compile(nulScope());
    DataTree::actualOne()(pos, LIST_SLICE_END);
    return nulOutputExpr();
}

util::sptr<output::Expression const> Undefined::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, UNDEFINED);
    return nulOutputExpr();
}

util::sptr<output::Expression const> Dictionary::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, DICT_BEGIN);
    items.iter([&](util::ptrkv<Expression const> const& item, int)
               {
                   DataTree::actualOne()(pos, DICT_ITEM);
                   item.key->compile(nulScope());
                   item.value->compile(nulScope());
               });
    DataTree::actualOne()(pos, DICT_END);
    return nulOutputExpr();
}

util::sptr<output::Expression const> Lambda::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, FUNC_DEF);
    std::for_each(param_names.begin()
                , param_names.end()
                , [&](std::string const& param)
                  {
                      DataTree::actualOne()(pos, PARAMETER, param);
                  });
    body->compile(nulScope());
    return nulOutputExpr();
}

util::sptr<output::Expression const> RegularAsyncLambda::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, REGULAR_ASYNC_PARAM_INDEX, async_param_index);
    return Lambda::compile(nulScope());
}

util::sptr<output::Expression const> RegularAsyncCall::compile(util::sref<Scope> s) const
{
    this->_compile(s, false);
    return nulOutputExpr();
}

util::sptr<output::Expression const> RegularAsyncCall::compileAsRoot(util::sref<Scope> s) const
{
    this->_compile(s, false);
    return nulOutputExpr();
}

util::uid RegularAsyncCall::_compile(util::sref<Scope>, bool) const
{
    DataTree::actualOne()(pos, ASYNC_CALL, former_args.size());
    DataTree::actualOne()(pos, CALL_BEGIN);
    callee->compile(nulScope());
    DataTree::actualOne()(pos, ARGUMENTS);
    compileList(former_args);
    compileList(latter_args);
    DataTree::actualOne()(pos, CALL_END);
    return util::uid::next_id();
}

util::uid AsyncCall::_compile(util::sref<Scope>, bool) const
{
    DataTree::actualOne()(pos, ASYNC_CALL);
    DataTree::actualOne()(pos, CALL_BEGIN);
    callee->compile(nulScope());
    DataTree::actualOne()(pos, ARGUMENTS);
    compileList(former_args);
    DataTree::actualOne()(pos, ASYNC_PLACEHOLDER_BEGIN);
    std::for_each(async_params.begin()
                , async_params.end()
                , [&](std::string const& param)
                  {
                      DataTree::actualOne()(pos, PARAMETER, param);
                  });
    DataTree::actualOne()(pos, ASYNC_PLACEHOLDER_END);
    compileList(latter_args);
    DataTree::actualOne()(pos, CALL_END);
    return util::uid::next_id();
}

util::sptr<output::Expression const> This::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, THIS);
    return nulOutputExpr();
}

util::sptr<output::Expression const> SuperFunc::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, SUPER_FUNC, property);
    return nulOutputExpr();
}

util::sptr<output::Expression const> Conditional::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, CONDITIONAL)(pos, OPERAND);
    predicate->compile(nulScope());
    DataTree::actualOne()(pos, OPERAND);
    consequence->compile(nulScope());
    DataTree::actualOne()(pos, OPERAND);
    alternative->compile(nulScope());
    return nulOutputExpr();
}

util::sptr<output::Expression const> ExceptionObj::compile(util::sref<Scope>) const
{
    DataTree::actualOne()(pos, EXCEPTION_OBJ);
    return nulOutputExpr();
}

util::sptr<output::Expression const> Pipeline::_compile(util::sref<Scope>, bool) const
{
    DataTree::actualOne()(pos, BINARY_OP, "[ pipeline ]")(pos, OPERAND);
    list->compile(nulScope());
    DataTree::actualOne()(pos, OPERAND);
    section->compile(nulScope());
    return nulOutputExpr();
}

util::sptr<Expression const> Pipeline::createMapper(misc::position const& pos
                                                  , util::sptr<semantic::Expression const> list
                                                  , util::sptr<semantic::Expression const> section)
{
    return util::mkptr(new BinaryOp(pos, std::move(list), "[ |: ]", std::move(section)));
}

util::sptr<Expression const> Pipeline::createFilter(misc::position const& pos
                                                  , util::sptr<semantic::Expression const> list
                                                  , util::sptr<semantic::Expression const> section)
{
    return util::mkptr(new BinaryOp(pos, std::move(list), "[ |? ]", std::move(section)));
}

util::sptr<output::Statement const> Function::_compileBody(
        util::sref<SymbolTable>, bool) const { return nulOutputStmt(); }
util::sptr<output::Statement const> RegularAsyncFunction::_compileBody(
        util::sref<SymbolTable>, bool) const { return nulOutputStmt(); }
bool Reference::isLiteral(util::sref<SymbolTable const>) const { return false; }
std::string Reference::literalType(util::sref<SymbolTable const>) const { return ""; }
bool Reference::boolValue(util::sref<SymbolTable const>) const { return false; }
mpz_class Reference::intValue(util::sref<SymbolTable const>) const { return 0; }
mpf_class Reference::floatValue(util::sref<SymbolTable const>) const { return 0; }
std::string Reference::stringValue(util::sref<SymbolTable const>) const { return ""; }
bool TypeOf::isLiteral(util::sref<SymbolTable const>) const { return false; }
std::string TypeOf::stringValue(util::sref<SymbolTable const>) const { return ""; }
bool PreUnaryOp::isLiteral(util::sref<SymbolTable const>) const { return false; }
std::string PreUnaryOp::literalType(util::sref<SymbolTable const>) const { return ""; }
bool PreUnaryOp::boolValue(util::sref<SymbolTable const>) const { return false; }
mpz_class PreUnaryOp::intValue(util::sref<SymbolTable const>) const { return 0; }
mpf_class PreUnaryOp::floatValue(util::sref<SymbolTable const>) const { return 0; }
std::string PreUnaryOp::stringValue(util::sref<SymbolTable const>) const { return ""; }
bool PreUnaryOp::isAsync() const { return false; }
bool BinaryOp::isLiteral(util::sref<SymbolTable const>) const { return false; }
std::string BinaryOp::literalType(util::sref<SymbolTable const>) const { return ""; }
bool BinaryOp::boolValue(util::sref<SymbolTable const>) const { return false; }
mpz_class BinaryOp::intValue(util::sref<SymbolTable const>) const { return 0; }
mpf_class BinaryOp::floatValue(util::sref<SymbolTable const>) const { return 0; }
std::string BinaryOp::stringValue(util::sref<SymbolTable const>) const { return ""; }
bool BinaryOp::isAsync() const { return false; }
bool BoolLiteral::boolValue(util::sref<SymbolTable const>) const { return false; }
mpz_class IntLiteral::intValue(util::sref<SymbolTable const>) const { return 0; }
mpf_class FloatLiteral::floatValue(util::sref<SymbolTable const>) const { return 0; }
bool StringLiteral::boolValue(util::sref<SymbolTable const>) const { return false; }
std::string StringLiteral::stringValue(util::sref<SymbolTable const>) const { return ""; }
bool ListLiteral::isAsync() const { return false; }
bool ListAppend::isAsync() const { return false; }
bool Call::isAsync() const { return false; }
bool SuperConstructorCall::isAsync() const { return false; }
bool MemberAccess::isAsync() const { return false; }
bool Lookup::isAsync() const { return false; }
bool ListSlice::isAsync() const { return false; }
bool Dictionary::isAsync() const { return false; }
bool Pipeline::isAsync() const { return false; }
bool Arithmetics::isAsync() const { return false; }
bool Branch::isAsync() const { return false; }
bool NameDef::isAsync() const { return false; }
bool RangeIteration::isAsync() const { return false; }
bool Return::isAsync() const { return false; }
bool Export::isAsync() const { return false; }
bool AttrSet::isAsync() const { return false; }
bool ExceptionStall::isAsync() const { return false; }
bool Block::isAsync() const { return true; }
bool Conditional::isLiteral(util::sref<SymbolTable const>) const { return false; }
std::string Conditional::literalType(util::sref<SymbolTable const>) const { return ""; }
bool Conditional::boolValue(util::sref<SymbolTable const>) const { return false; }
mpz_class Conditional::intValue(util::sref<SymbolTable const>) const { return 0; }
mpf_class Conditional::floatValue(util::sref<SymbolTable const>) const { return 0; }
std::string Conditional::stringValue(util::sref<SymbolTable const>) const { return ""; }
bool Conditional::isAsync() const { return false; }
util::sptr<output::Expression const> Function::compileToLambda(
        util::sref<SymbolTable>, bool) const { return nulOutputExpr(); }
util::sptr<output::Expression const> RegularAsyncFunction::compileToLambda(
        util::sref<SymbolTable>, bool) const { return nulOutputExpr(); }
