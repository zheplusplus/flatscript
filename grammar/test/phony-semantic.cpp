#include <algorithm>

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

    util::sptr<output::Expression const> nulOutputExpr()
    {
        return util::sptr<output::Expression const>(nullptr);
    }

    util::sptr<output::Statement const> nulOutputStmt()
    {
        return util::sptr<output::Statement const>(nullptr);
    }

}

Constructor::Constructor(misc::position const& ps, std::vector<std::string> params, Block b
                       , std::string const&, bool, util::ptrarr<Expression const>)
    : pos(ps)
    , param_names(std::move(params))
    , super_init(nullptr)
    , body(std::move(b))
{}

util::sptr<output::Function const> Function::compile(util::sref<SymbolTable>, bool) const
{
    DataTree::actualOne()(pos, FUNC_DEF, name);
    std::for_each(param_names.begin()
                , param_names.end()
                , [&](std::string const& param)
                  {
                      DataTree::actualOne()(pos, PARAMETER, param);
                  });
    body.compile(nulSpace());
    return util::sptr<output::Function const>(nullptr);
}

util::sptr<output::Function const> RegularAsyncFunction::compile(
        util::sref<SymbolTable>, bool) const
{
    DataTree::actualOne()(pos, REGULAR_ASYNC_PARAM_INDEX, async_param_index);
    return Function::compile(nulSymbols(), false);
}

void Block::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(BLOCK_BEGIN);
    _funcs.iter([&](util::sptr<Function const> const& func, int)
                {
                    func->compile(nulSymbols());
                });
    _stmts.iter([&](util::sptr<Statement const> const& stmt, int)
                {
                    stmt->compile(nulSpace());
                });
    DataTree::actualOne()(BLOCK_END);
}

void Arithmetics::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, ARITHMETICS);
    expr->compile(nulSpace());
}

void Branch::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, BRANCH);
    predicate->compile(nulSpace());
    DataTree::actualOne()(CONSEQUENCE);
    consequence.compile(nulSpace());
    DataTree::actualOne()(ALTERNATIVE);
    alternative.compile(nulSpace());
}

void NameDef::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, NAME_DEF, name);
    init->compile(nulSpace());
}

void Export::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, EXPORT);
    std::for_each(export_point.begin()
                , export_point.end()
                , [&](std::string const& name)
                  {
                      DataTree::actualOne()(pos, IDENTIFIER, name);
                  });
    DataTree::actualOne()(pos, EXPORT_VALUE);
    value->compile(nulSpace());
}

void Import::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, IMPORT);
    std::for_each(names.begin()
                , names.end()
                , [&](std::string const& name)
                  {
                      DataTree::actualOne()(pos, IDENTIFIER, name);
                  });
}

void AttrSet::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, ATTR_SET);
    set_point->compile(nulSpace());
    value->compile(nulSpace());
}

void Return::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, RETURN);
    ret_val->compile(nulSpace());
}

void ExceptionStall::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, TRY);
    try_block.compile(nulSpace());
    DataTree::actualOne()(pos, CATCH);
    catch_block.compile(nulSpace());
}

void Throw::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, THROW);
    exception->compile(nulSpace());
}

util::sptr<output::Expression const> PreUnaryOp::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, PRE_UNARY_OP, op_img)(pos, OPERAND);
    rhs->compile(nulSpace());
    return nulOutputExpr();
}

util::sptr<output::Expression const> BinaryOp::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, BINARY_OP, op_img)(pos, OPERAND);
    lhs->compile(nulSpace());
    DataTree::actualOne()(pos, OPERAND);
    rhs->compile(nulSpace());
    return nulOutputExpr();
}

util::sptr<output::Expression const> TypeOf::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, PRE_UNARY_OP, "[ typeof ]")(pos, OPERAND);
    expr->compile(nulSpace());
    return nulOutputExpr();
}

util::sptr<output::Expression const> Reference::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, IDENTIFIER, name);
    return nulOutputExpr();
}

util::sptr<output::Expression const> BoolLiteral::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, BOOLEAN, util::str(value));
    return nulOutputExpr();
}

util::sptr<output::Expression const> IntLiteral::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, INTEGER, util::str(value));
    return nulOutputExpr();
}

util::sptr<output::Expression const> FloatLiteral::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, FLOATING, util::str(value));
    return nulOutputExpr();
}

util::sptr<output::Expression const> StringLiteral::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, STRING, value);
    return nulOutputExpr();
}

static void compileList(util::ptrarr<Expression const> const& values)
{
    values.iter([&](util::sptr<Expression const> const& v, int)
                {
                    v->compile(nulSpace());
                });
}

util::sptr<output::Expression const> ListLiteral::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, LIST_BEGIN);
    compileList(value);
    DataTree::actualOne()(pos, LIST_END);
    return nulOutputExpr();
}

util::sptr<output::Expression const> PipeElement::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, PIPE_ELEMENT);
    return nulOutputExpr();
}

util::sptr<output::Expression const> PipeIndex::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, PIPE_INDEX);
    return nulOutputExpr();
}

util::sptr<output::Expression const> PipeKey::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, PIPE_KEY);
    return nulOutputExpr();
}

util::sptr<output::Expression const> PipeResult::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, PIPE_RESULT);
    return nulOutputExpr();
}

util::sptr<output::Expression const> ListAppend::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, BINARY_OP, "[ ++ ]")(pos, OPERAND);
    lhs->compile(nulSpace());
    DataTree::actualOne()(pos, OPERAND);
    rhs->compile(nulSpace());
    return nulOutputExpr();
}

util::sptr<output::Expression const> Call::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, CALL_BEGIN);
    callee->compile(nulSpace());
    DataTree::actualOne()(pos, ARGUMENTS);
    compileList(args);
    DataTree::actualOne()(pos, CALL_END);
    return nulOutputExpr();
}

util::sptr<output::Expression const> SuperConstructorCall::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, CALL_BEGIN, "SUPER");
    DataTree::actualOne()(pos, ARGUMENTS);
    compileList(args);
    DataTree::actualOne()(pos, CALL_END);
    return nulOutputExpr();
}

util::sptr<output::Expression const> MemberAccess::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, BINARY_OP, "[ . ]")(pos, OPERAND);
    referee->compile(nulSpace());
    DataTree::actualOne()(pos, OPERAND);
    DataTree::actualOne()(pos, IDENTIFIER, member);
    return nulOutputExpr();
}

util::sptr<output::Expression const> Lookup::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, BINARY_OP, "[]")(pos, OPERAND);
    collection->compile(nulSpace());
    DataTree::actualOne()(pos, OPERAND);
    key->compile(nulSpace());
    return nulOutputExpr();
}

util::sptr<output::Expression const> ListSlice::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, LIST_SLICE_BEGIN);
    list->compile(nulSpace());
    begin->compile(nulSpace());
    end->compile(nulSpace());
    step->compile(nulSpace());
    DataTree::actualOne()(pos, LIST_SLICE_END);
    return nulOutputExpr();
}

util::sptr<output::Expression const> Undefined::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, UNDEFINED);
    return nulOutputExpr();
}

util::sptr<output::Expression const> Dictionary::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, DICT_BEGIN);
    items.iter([&](util::ptrkv<Expression const> const& item, int)
               {
                   DataTree::actualOne()(pos, DICT_ITEM);
                   item.key->compile(nulSpace());
                   item.value->compile(nulSpace());
               });
    DataTree::actualOne()(pos, DICT_END);
    return nulOutputExpr();
}

util::sptr<output::Expression const> Lambda::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, FUNC_DEF);
    std::for_each(param_names.begin()
                , param_names.end()
                , [&](std::string const& param)
                  {
                      DataTree::actualOne()(pos, PARAMETER, param);
                  });
    body.compile(nulSpace());
    return nulOutputExpr();
}

util::sptr<output::Expression const> RegularAsyncLambda::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, REGULAR_ASYNC_PARAM_INDEX, async_param_index);
    return Lambda::compile(nulSpace());
}

util::sptr<output::Expression const> RegularAsyncCall::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, ASYNC_CALL, former_args.size());
    DataTree::actualOne()(pos, CALL_BEGIN);
    callee->compile(nulSpace());
    DataTree::actualOne()(pos, ARGUMENTS);
    compileList(former_args);
    compileList(latter_args);
    DataTree::actualOne()(pos, CALL_END);
    return nulOutputExpr();
}

util::sptr<output::Expression const> AsyncCall::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, ASYNC_CALL);
    DataTree::actualOne()(pos, CALL_BEGIN);
    callee->compile(nulSpace());
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
    return nulOutputExpr();
}

util::sptr<output::Expression const> This::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, THIS);
    return nulOutputExpr();
}

util::sptr<output::Expression const> SuperFunc::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, SUPER_FUNC, property);
    return nulOutputExpr();
}

util::sptr<output::Expression const> Conditional::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, CONDITIONAL)(pos, OPERAND);
    predicate->compile(nulSpace());
    DataTree::actualOne()(pos, OPERAND);
    consequence->compile(nulSpace());
    DataTree::actualOne()(pos, OPERAND);
    alternative->compile(nulSpace());
    return nulOutputExpr();
}

util::sptr<output::Expression const> ExceptionObj::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, EXCEPTION_OBJ);
    return nulOutputExpr();
}

util::sptr<output::Expression const> Pipeline::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, BINARY_OP, "[ pipeline ]")(pos, OPERAND);
    list->compile(nulSpace());
    DataTree::actualOne()(pos, OPERAND);
    section.compile(nulSpace());
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
bool Return::isAsync() const { return false; }
bool Export::isAsync() const { return false; }
bool AttrSet::isAsync() const { return false; }
bool ExceptionStall::isAsync() const { return false; }
bool Conditional::isLiteral(util::sref<SymbolTable const>) const { return false; }
std::string Conditional::literalType(util::sref<SymbolTable const>) const { return ""; }
bool Conditional::boolValue(util::sref<SymbolTable const>) const { return false; }
mpz_class Conditional::intValue(util::sref<SymbolTable const>) const { return 0; }
mpf_class Conditional::floatValue(util::sref<SymbolTable const>) const { return 0; }
std::string Conditional::stringValue(util::sref<SymbolTable const>) const { return ""; }
bool Conditional::isAsync() const { return false; }
