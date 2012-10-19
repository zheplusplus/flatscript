#include <algorithm>

#include <flowcheck/func-body-filter.h>
#include <flowcheck/global-filter.h>
#include <flowcheck/symbol-def-filter.h>
#include <flowcheck/accumulator.h>
#include <flowcheck/block.h>
#include <flowcheck/expr-nodes.h>
#include <flowcheck/list-pipe.h>
#include <flowcheck/stmt-nodes.h>
#include <flowcheck/function.h>
#include <proto/list-pipe.h>
#include <proto/function.h>
#include <util/string.h>

#include "test-common.h"

using namespace test;
using namespace flchk;

namespace {

    util::sptr<proto::Statement> nulProtoStmt()
    {
        return util::sptr<proto::Statement>(nullptr);
    }

    util::sptr<proto::Expression const> nulProtoExpr()
    {
        return util::sptr<proto::Expression const>(nullptr);
    }

    util::sptr<flchk::Expression const> nulFlchkExpr()
    {
        return util::sptr<flchk::Expression const>(nullptr);
    }

    struct BranchConsequence
        : public Statement
    {
        BranchConsequence(misc::position const& pos, util::sptr<Expression const> p, Block c)
            : Statement(pos)
            , predicate(std::move(p))
            , consequence(std::move(c))
        {}

        util::sptr<proto::Statement const> compile(util::sref<SymbolTable>) const
        {
            DataTree::actualOne()(pos, BRANCH_CONSQ_ONLY);
            predicate->compile(nulSymbols());
            DataTree::actualOne()(CONSEQUENCE);
            consequence.compile(nulSymbols());
            return nulProtoStmt();
        }

        util::sptr<Expression const> const predicate;
        Block const consequence;
    };

    struct BranchAlternative
        : public Statement
    {
        BranchAlternative(misc::position const& pos, util::sptr<Expression const> p, Block a)
            : Statement(pos)
            , predicate(std::move(p))
            , alternative(std::move(a))
        {}

        util::sptr<proto::Statement const> compile(util::sref<SymbolTable>) const
        {
            DataTree::actualOne()(pos, BRANCH_ALTER_ONLY);
            predicate->compile(nulSymbols());
            DataTree::actualOne()(ALTERNATIVE);
            alternative.compile(nulSymbols());
            return nulProtoStmt();
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

util::sptr<proto::Function const> Function::compile() const
{
    DataTree::actualOne()(pos, FUNC_DEF, name);
    std::for_each(param_names.begin()
                , param_names.end()
                , [&](std::string const& param)
                  {
                      DataTree::actualOne()(pos, PARAMETER, param);
                  });
    _body->compile();
    return util::sptr<proto::Function const>(nullptr);
}

void Block::addStmt(util::sptr<Statement const> stmt)
{
    _stmts.push_back(std::move(stmt));
}

util::sref<Function const> Block::defFunc(misc::position const& pos
                                        , std::string const& name
                                        , std::vector<std::string> const& param_names
                                        , util::sptr<Filter> body)
{
    _funcs.push_back(util::mkptr(new Function(pos, name, param_names, std::move(body))));
    return *_funcs.back();
}

util::sptr<proto::Statement const> Block::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(BLOCK_BEGIN);
    std::for_each(_funcs.begin()
                , _funcs.end()
                , [&](util::sptr<Function const> const& func)
                  {
                      func->compile();
                  });

    std::for_each(_stmts.begin()
                , _stmts.end()
                , [&](util::sptr<Statement const> const& stmt)
                  {
                      stmt->compile(nulSymbols());
                  });
    DataTree::actualOne()(BLOCK_END);
    return nulProtoStmt();
}

void Accumulator::addReturn(misc::position const& pos, util::sptr<Expression const> ret_val)
{
    _block.addStmt(util::mkptr(new Return(pos, std::move(ret_val))));
}

void Accumulator::addReturnNothing(misc::position const& pos)
{
    _block.addStmt(util::mkptr(new ReturnNothing(pos)));
}

void Accumulator::addArith(misc::position const& pos, util::sptr<Expression const> expr)
{
    _block.addStmt(util::mkptr(new Arithmetics(pos, std::move(expr))));
}

void Accumulator::addImport(misc::position const& pos, std::vector<std::string> const& names)
{
    _block.addStmt(util::mkptr(new Import(pos, names)));
}

void Accumulator::addAttrSet(misc::position const& pos
                           , util::sptr<Expression const> set_point
                           , util::sptr<Expression const> value)
{
    _block.addStmt(util::mkptr(new AttrSet(pos, std::move(set_point), std::move(value))));
}

void Accumulator::addBranch(misc::position const& pos
                          , util::sptr<Expression const> predicate
                          , Accumulator consequence
                          , Accumulator alternative)
{
    _block.addStmt(util::mkptr(new Branch(pos
                                        , std::move(predicate)
                                        , std::move(consequence._block)
                                        , std::move(alternative._block))));
}

void Accumulator::addBranch(misc::position const& pos
                          , util::sptr<Expression const> predicate
                          , Accumulator consequence)
{
    _block.addStmt(util::mkptr(new BranchConsequence(pos
                                                   , std::move(predicate)
                                                   , std::move(consequence._block))));
}

void Accumulator::addBranchAlterOnly(misc::position const& pos
                                   , util::sptr<Expression const> predicate
                                   , Accumulator alternative)
{
    _block.addStmt(util::mkptr(new BranchAlternative(pos
                                                   , std::move(predicate)
                                                   , std::move(alternative._block))));
}

void Accumulator::defName(misc::position const& pos
                        , std::string const& name
                        , util::sptr<Expression const> init)
{
    _block.addStmt(util::mkptr(new NameDef(pos, name, std::move(init))));
}

util::sref<Function const> Accumulator::defFunc(misc::position const& pos
                                              , std::string const& name
                                              , std::vector<std::string> const& param_names
                                              , util::sptr<Filter> body)
{
    return _block.defFunc(pos, name, param_names, std::move(body));
}

util::sptr<proto::Statement const> Accumulator::compileBlock(util::sref<SymbolTable>) const
{
    _block.compile(nulSymbols());
    return nulProtoStmt();
}

void Filter::addReturn(misc::position const& pos, util::sptr<Expression const> ret_val)
{
    _accumulator.addReturn(pos, std::move(ret_val));
}

void Filter::addReturnNothing(misc::position const& pos)
{
    _accumulator.addReturnNothing(pos);
}

void Filter::addArith(misc::position const& pos, util::sptr<Expression const> expr)
{
    _accumulator.addArith(pos, std::move(expr));
}

void Filter::addImport(misc::position const& pos, std::vector<std::string> const& names)
{
    _accumulator.addImport(pos, names);
}

void Filter::addAttrSet(misc::position const& pos
                      , util::sptr<Expression const> set_point
                      , util::sptr<Expression const> value)
{
    _accumulator.addAttrSet(pos, std::move(set_point), std::move(value));
}

void Filter::addBranch(misc::position const& pos
                     , util::sptr<Expression const> predicate
                     , util::sptr<Filter> consequence
                     , util::sptr<Filter> alternative)
{
    _accumulator.addBranch(pos
                         , std::move(predicate)
                         , std::move(consequence->_accumulator)
                         , std::move(alternative->_accumulator));
}

void Filter::addBranch(misc::position const& pos
                     , util::sptr<Expression const> predicate
                     , util::sptr<Filter> consequence)
{
    _accumulator.addBranch(pos, std::move(predicate), std::move(consequence->_accumulator));
}

void Filter::addBranchAlterOnly(misc::position const& pos
                              , util::sptr<Expression const> predicate
                              , util::sptr<Filter> alternative)
{
    _accumulator.addBranchAlterOnly(pos
                                  , std::move(predicate)
                                  , std::move(alternative->_accumulator));
}

util::sptr<proto::Statement const> Filter::compile()
{
    _accumulator.compileBlock(nulSymbols());
    return nulProtoStmt();
}

void FuncBodyFilter::defName(misc::position const& pos
                           , std::string const& name
                           , util::sptr<Expression const> init)
{
    _accumulator.defName(pos, name, std::move(init));
}

void FuncBodyFilter::defFunc(misc::position const& pos
                           , std::string const& name
                           , std::vector<std::string> const& param_names
                           , util::sptr<Filter> body)
{
    _accumulator.defFunc(pos, name, param_names, std::move(body));
}

util::sref<SymbolTable> FuncBodyFilter::getSymbols()
{
    return nulSymbols();
}

void SymbolDefFilter::defName(misc::position const& pos
                            , std::string const& name
                            , util::sptr<Expression const> init)
{
    _accumulator.defName(pos, name + NAME_DEF_FILTERED, std::move(init));
}

void SymbolDefFilter::defFunc(misc::position const& pos
                            , std::string const& name
                            , std::vector<std::string> const& param_names
                            , util::sptr<Filter> body)
{
    _accumulator.defFunc(pos, name + FUNC_DEF_FILTERED, param_names, std::move(body));
}

util::sref<SymbolTable> SymbolDefFilter::getSymbols()
{
    return nulSymbols();
}

GlobalFilter::GlobalFilter() {}

util::sptr<proto::Statement const> Arithmetics::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, ARITHMETICS);
    expr->compile(nulSymbols());
    return nulProtoStmt();
}

util::sptr<proto::Statement const> Branch::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, BRANCH);
    predicate->compile(nulSymbols());
    DataTree::actualOne()(CONSEQUENCE);
    consequence.compile(nulSymbols());
    DataTree::actualOne()(ALTERNATIVE);
    alternative.compile(nulSymbols());
    return nulProtoStmt();
}

util::sptr<proto::Statement const> NameDef::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, NAME_DEF, name);
    init->compile(nulSymbols());
    return nulProtoStmt();
}

util::sptr<proto::Statement const> Import::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, IMPORT);
    std::for_each(names.begin()
                , names.end()
                , [&](std::string const& name)
                  {
                      DataTree::actualOne()(pos, IDENTIFIER, name);
                  });
    return nulProtoStmt();
}

util::sptr<proto::Statement const> AttrSet::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, ATTR_SET);
    set_point->compile(nulSymbols());
    value->compile(nulSymbols());
    return nulProtoStmt();
}

util::sptr<proto::Statement const> Return::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, RETURN);
    ret_val->compile(nulSymbols());
    return nulProtoStmt();
}

util::sptr<proto::Statement const> ReturnNothing::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, RETURN_NOTHING);
    return nulProtoStmt();
}

util::sptr<proto::Expression const> PreUnaryOp::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, PRE_UNARY_OP, op_img)(pos, OPERAND);
    rhs->compile(nulSymbols());
    return nulProtoExpr();
}

util::sptr<proto::Expression const> BinaryOp::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, BINARY_OP, op_img)(pos, OPERAND);
    lhs->compile(nulSymbols());
    DataTree::actualOne()(pos, OPERAND);
    rhs->compile(nulSymbols());
    return nulProtoExpr();
}

util::sptr<proto::Expression const> Conjunction::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, BINARY_OP, "&&")(pos, OPERAND);
    lhs->compile(nulSymbols());
    DataTree::actualOne()(pos, OPERAND);
    rhs->compile(nulSymbols());
    return nulProtoExpr();
}

util::sptr<proto::Expression const> Disjunction::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, BINARY_OP, "||")(pos, OPERAND);
    lhs->compile(nulSymbols());
    DataTree::actualOne()(pos, OPERAND);
    rhs->compile(nulSymbols());
    return nulProtoExpr();
}

util::sptr<proto::Expression const> Negation::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, PRE_UNARY_OP, "!")(pos, OPERAND);
    rhs->compile(nulSymbols());
    return nulProtoExpr();
}

util::sptr<proto::Expression const> Reference::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, IDENTIFIER, name);
    return nulProtoExpr();
}

util::sptr<proto::Expression const> BoolLiteral::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, BOOLEAN, util::str(value));
    return nulProtoExpr();
}

util::sptr<proto::Expression const> IntLiteral::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, INTEGER, util::str(value));
    return nulProtoExpr();
}

util::sptr<proto::Expression const> FloatLiteral::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, FLOATING, util::str(value));
    return nulProtoExpr();
}

util::sptr<proto::Expression const> StringLiteral::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, STRING, value);
    return nulProtoExpr();
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

util::sptr<proto::Expression const> ListLiteral::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, LIST_BEGIN);
    compileList(value);
    DataTree::actualOne()(pos, LIST_END);
    return nulProtoExpr();
}

util::sptr<proto::Expression const> ListElement::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, LIST_ELEMENT);
    return nulProtoExpr();
}

util::sptr<proto::Expression const> ListIndex::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, LIST_INDEX);
    return nulProtoExpr();
}

util::sptr<proto::Expression const> ListAppend::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, BINARY_OP, "++")(pos, OPERAND);
    lhs->compile(nulSymbols());
    DataTree::actualOne()(pos, OPERAND);
    rhs->compile(nulSymbols());
    return nulProtoExpr();
}

util::sptr<proto::Expression const> Call::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, CALL_BEGIN);
    callee->compile(nulSymbols());
    DataTree::actualOne()(pos, ARGUMENTS);
    compileList(args);
    DataTree::actualOne()(pos, CALL_END);
    return nulProtoExpr();
}

util::sptr<proto::Expression const> MemberAccess::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, BINARY_OP, ".")(pos, OPERAND);
    referee->compile(nulSymbols());
    DataTree::actualOne()(pos, OPERAND);
    DataTree::actualOne()(pos, IDENTIFIER, member);
    return nulProtoExpr();
}

util::sptr<proto::Expression const> Lookup::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, BINARY_OP, "[]")(pos, OPERAND);
    collection->compile(nulSymbols());
    DataTree::actualOne()(pos, OPERAND);
    key->compile(nulSymbols());
    return nulProtoExpr();
}

util::sptr<proto::Expression const> ListSlice::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, LIST_SLICE_BEGIN);
    list->compile(nulSymbols());
    begin->compile(nulSymbols());
    end->compile(nulSymbols());
    step->compile(nulSymbols());
    DataTree::actualOne()(pos, LIST_SLICE_END);
    return nulProtoExpr();
}

util::sptr<proto::Expression const> ListSlice::Default::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, LIST_SLICE_DEFAULT);
    return nulProtoExpr();
}

util::sptr<proto::Expression const> Dictionary::compile(util::sref<SymbolTable>) const
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
    return nulProtoExpr();
}

util::sptr<proto::Expression const> ListPipeline::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(pos, LIST_PIPELINE_BEGIN);
    list->compile(nulSymbols());
    std::for_each(pipeline.begin()
                , pipeline.end()
                , [&](util::sptr<PipeBase const> const& pipe)
                  {
                      pipe->compile(nulSymbols());
                  });
    DataTree::actualOne()(pos, LIST_PIPELINE_END);
    return nulProtoExpr();
}

util::sptr<proto::PipeBase const> PipeMap::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(expr->pos, PIPE_MAP);
    expr->compile(nulSymbols());
    return util::sptr<proto::PipeBase const>(nullptr);
}

util::sptr<proto::PipeBase const> PipeFilter::compile(util::sref<SymbolTable>) const
{
    DataTree::actualOne()(expr->pos, PIPE_FILTER);
    expr->compile(nulSymbols());
    return util::sptr<proto::PipeBase const>(nullptr);
}

bool Expression::isLiteral() const
{
    return false;
}

bool Expression::boolValue() const
{
    return false;
}

util::sptr<Expression const> Expression::operate(misc::position const&
                                               , std::string const&
                                               , mpz_class const&) const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> Expression::operate(misc::position const&
                                               , std::string const&
                                               , mpf_class const&) const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> Expression::operate(misc::position const&
                                               , std::string const&
                                               , bool) const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> Expression::asRhs(misc::position const&
                                             , std::string const&
                                             , util::sptr<Expression const>) const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> Expression::asRhs(misc::position const&, std::string const&) const
{
    return nulFlchkExpr();
}

bool PreUnaryOp::isLiteral() const
{
    return false;
}

bool PreUnaryOp::boolValue() const
{
    return false;
}

std::string PreUnaryOp::typeName() const
{
    return "";
}

util::sptr<Expression const> PreUnaryOp::fold() const
{
    return nulFlchkExpr();
}

bool BinaryOp::isLiteral() const
{
    return false;
}

bool BinaryOp::boolValue() const
{
    return false;
}

std::string BinaryOp::typeName() const
{
    return "";
}

util::sptr<Expression const> BinaryOp::fold() const
{
    return nulFlchkExpr();
}

bool Conjunction::isLiteral() const
{
    return false;
}

bool Conjunction::boolValue() const
{
    return false;
}

std::string Conjunction::typeName() const
{
    return "";
}

util::sptr<Expression const> Conjunction::fold() const
{
    return nulFlchkExpr();
}

bool Disjunction::isLiteral() const
{
    return false;
}

bool Disjunction::boolValue() const
{
    return false;
}

std::string Disjunction::typeName() const
{
    return "";
}

util::sptr<Expression const> Disjunction::fold() const
{
    return nulFlchkExpr();
}

bool Negation::isLiteral() const
{
    return false;
}

bool Negation::boolValue() const
{
    return false;
}

std::string Negation::typeName() const
{
    return "";
}

util::sptr<Expression const> Negation::fold() const
{
    return nulFlchkExpr();
}

std::string Reference::typeName() const
{
    return "";
}

util::sptr<Expression const> Reference::fold() const
{
    return nulFlchkExpr();
}

bool BoolLiteral::isLiteral() const
{
    return false;
}

bool BoolLiteral::boolValue() const
{
    return false;
}

std::string BoolLiteral::typeName() const
{
    return "";
}

util::sptr<Expression const> BoolLiteral::fold() const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> BoolLiteral::operate(misc::position const&
                                                , std::string const&
                                                , mpz_class const&) const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> BoolLiteral::operate(misc::position const&
                                                , std::string const&
                                                , mpf_class const&) const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> BoolLiteral::operate(misc::position const&
                                                , std::string const&
                                                , bool) const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> BoolLiteral::asRhs(misc::position const&
                                              , std::string const&
                                              , util::sptr<Expression const>) const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> BoolLiteral::asRhs(misc::position const&, std::string const&) const
{
    return nulFlchkExpr();
}

bool IntLiteral::isLiteral() const
{
    return false;
}

bool IntLiteral::boolValue() const
{
    return false;
}

std::string IntLiteral::typeName() const
{
    return "";
}

util::sptr<Expression const> IntLiteral::fold() const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> IntLiteral::operate(misc::position const&
                                               , std::string const&
                                               , mpz_class const&) const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> IntLiteral::operate(misc::position const&
                                               , std::string const&
                                               , mpf_class const&) const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> IntLiteral::operate(misc::position const&
                                               , std::string const&
                                               , bool) const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> IntLiteral::asRhs(misc::position const&
                                             , std::string const&
                                             , util::sptr<Expression const>) const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> IntLiteral::asRhs(misc::position const&, std::string const&) const
{
    return nulFlchkExpr();
}

bool FloatLiteral::isLiteral() const
{
    return false;
}

bool FloatLiteral::boolValue() const
{
    return false;
}

std::string FloatLiteral::typeName() const
{
    return "";
}

util::sptr<Expression const> FloatLiteral::fold() const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> FloatLiteral::operate(misc::position const&
                                                 , std::string const&
                                                 , mpz_class const&) const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> FloatLiteral::operate(misc::position const&
                                                 , std::string const&
                                                 , mpf_class const&) const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> FloatLiteral::operate(misc::position const&
                                                 , std::string const&
                                                 , bool) const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> FloatLiteral::asRhs(misc::position const&
                                               , std::string const&
                                               , util::sptr<Expression const>) const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> FloatLiteral::asRhs(misc::position const&, std::string const&) const
{
    return nulFlchkExpr();
}

std::string StringLiteral::typeName() const
{
    return "";
}

util::sptr<Expression const> StringLiteral::fold() const
{
    return nulFlchkExpr();
}

std::string ListLiteral::typeName() const
{
    return "";
}

util::sptr<Expression const> ListLiteral::fold() const
{
    return nulFlchkExpr();
}

std::string ListElement::typeName() const
{
    return "";
}

util::sptr<Expression const> ListElement::fold() const
{
    return nulFlchkExpr();
}

std::string ListIndex::typeName() const
{
    return "";
}

util::sptr<Expression const> ListIndex::fold() const
{
    return nulFlchkExpr();
}

std::string ListAppend::typeName() const
{
    return "";
}

util::sptr<Expression const> ListAppend::fold() const
{
    return nulFlchkExpr();
}

std::string Call::typeName() const
{
    return "";
}

util::sptr<Expression const> Call::fold() const
{
    return nulFlchkExpr();
}

std::string MemberAccess::typeName() const
{
    return "";
}

util::sptr<Expression const> MemberAccess::fold() const
{
    return nulFlchkExpr();
}

std::string Lookup::typeName() const
{
    return "";
}

util::sptr<Expression const> Lookup::fold() const
{
    return nulFlchkExpr();
}

std::string ListSlice::typeName() const
{
    return "";
}

util::sptr<Expression const> ListSlice::fold() const
{
    return nulFlchkExpr();
}

std::string ListSlice::Default::typeName() const
{
    return "";
}

util::sptr<Expression const> ListSlice::Default::fold() const
{
    return nulFlchkExpr();
}

util::sptr<Expression const> Dictionary::fold() const
{
    return nulFlchkExpr();
}

std::string Dictionary::typeName() const
{
    return "";
}

std::string ListPipeline::typeName() const
{
    return "";
}

util::sptr<Expression const> ListPipeline::fold() const
{
    return nulFlchkExpr();
}

util::sptr<PipeBase const> PipeMap::fold() const
{
    return util::sptr<PipeBase const>(nullptr);
}

util::sptr<PipeBase const> PipeFilter::fold() const
{
    return util::sptr<PipeBase const>(nullptr);
}
