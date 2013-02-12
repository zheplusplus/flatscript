#include <algorithm>

#include <semantic/filter.h>
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

    struct BranchConsequence
        : Statement
    {
        BranchConsequence(misc::position const& pos, util::sptr<Expression const> p, Block c)
            : Statement(pos)
            , predicate(std::move(p))
            , consequence(std::move(c))
        {}

        void compile(BaseCompilingSpace&) const
        {
            DataTree::actualOne()(pos, BRANCH_CONSQ_ONLY);
            predicate->compile(nulSpace());
            DataTree::actualOne()(CONSEQUENCE);
            consequence.compile(CompilingSpace());
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

        void compile(BaseCompilingSpace&) const
        {
            DataTree::actualOne()(pos, BRANCH_ALTER_ONLY);
            predicate->compile(nulSpace());
            DataTree::actualOne()(ALTERNATIVE);
            alternative.compile(CompilingSpace());
        }

        util::sptr<Expression const> const predicate;
        Block const alternative;
    };

    struct ReturnNothing
        : Statement
    {
        explicit ReturnNothing(misc::position const& pos)
            : Statement(pos)
        {}

        void compile(BaseCompilingSpace&) const
        {
            DataTree::actualOne()(pos, RETURN_NOTHING);
        }
    };

}

util::sptr<output::Function const> Function::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, FUNC_DEF, name);
    std::for_each(param_names.begin()
                , param_names.end()
                , [&](std::string const& param)
                  {
                      DataTree::actualOne()(pos, PARAMETER, param);
                  });
    body.compile(CompilingSpace());
    return util::sptr<output::Function const>(nullptr);
}

util::sptr<output::Function const> RegularAsyncFunction::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, REGULAR_ASYNC_PARAM_INDEX, async_param_index);
    return Function::compile(nulSymbols());
}

void Block::addStmt(util::sptr<Statement const> stmt)
{
    _stmts.append(std::move(stmt));
}

void Block::addFunc(util::sptr<Function const> func)
{
    _funcs.append(std::move(func));
}

util::sptr<output::Statement const> Block::compile(BaseCompilingSpace&&) const
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
    return util::sptr<output::Statement const>(nullptr);
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

Block Filter::deliver()
{
    return std::move(_block);
}

void Filter::defName(misc::position const& pos
                   , std::string const& name
                   , util::sptr<Expression const> init)
{
    _block.addStmt(util::mkptr(new NameDef(pos, name, std::move(init))));
}

void Filter::defFunc(util::sptr<Function const> func)
{
    _block.addFunc(std::move(func));
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
    consequence.compile(CompilingSpace());
    DataTree::actualOne()(ALTERNATIVE);
    alternative.compile(CompilingSpace());
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
    body.compile(CompilingSpace());
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

util::sptr<output::Expression const> Pipeline::compile(BaseCompilingSpace&) const
{
    DataTree::actualOne()(pos, BINARY_OP, "[ pipeline ]")(pos, OPERAND);
    list->compile(nulSpace());
    DataTree::actualOne()(pos, OPERAND);
    section.compile(CompilingSpace());
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

BaseCompilingSpace::BaseCompilingSpace(util::sptr<SymbolTable>)
    : _symbols(nullptr)
    , _main_block(nullptr)
    , _current_block(nullptr)
{}

util::sptr<output::Expression const> BaseCompilingSpace::ret(util::sref<Expression const>)
{
    return nulOutputExpr();
}

output::Method BaseCompilingSpace::raiseMethod() const
{
    return output::Method(nullptr);
}

util::sptr<output::Block> BaseCompilingSpace::deliver()
{
    return util::sptr<output::Block>(nullptr);
}

CompilingSpace::CompilingSpace()
    : BaseCompilingSpace(util::sptr<SymbolTable>(nullptr))
{}

util::sptr<output::Block> CompilingSpace::deliver()
{
    return util::sptr<output::Block>(nullptr);
}

void CompilingSpace::referenceThis() {}
bool Expression::boolValue(util::sref<SymbolTable const>) const { return false; }
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
bool Conditional::isLiteral(util::sref<SymbolTable const>) const { return false; }
std::string Conditional::literalType(util::sref<SymbolTable const>) const { return ""; }
bool Conditional::boolValue(util::sref<SymbolTable const>) const { return false; }
mpz_class Conditional::intValue(util::sref<SymbolTable const>) const { return 0; }
mpf_class Conditional::floatValue(util::sref<SymbolTable const>) const { return 0; }
std::string Conditional::stringValue(util::sref<SymbolTable const>) const { return ""; }
bool Conditional::isAsync() const { return false; }
