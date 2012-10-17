#include <algorithm>

#include <grammar/expr-nodes.h>
#include <grammar/list-pipe.h>
#include <grammar/clause-builder.h>
#include <flowcheck/list-pipe.h>
#include <flowcheck/function.h>
#include <flowcheck/filter.h>
#include <util/string.h>

#include "test-common.h"

using namespace grammar;
using namespace test;

static util::sptr<flchk::Expression const> nulptr(nullptr);

util::sptr<flchk::Expression const> PreUnaryOp::compile() const
{
    DataTree::actualOne()(pos, PRE_UNARY_OP_BEGIN, op_img)(pos, OPERAND);
    rhs->compile();
    DataTree::actualOne()(pos, PRE_UNARY_OP_END);
    return std::move(nulptr);
}

util::sptr<flchk::Expression const> BinaryOp::compile() const
{
    DataTree::actualOne()(pos, BINARY_OP_BEGIN, op_img)(pos, OPERAND);
    lhs->compile();
    DataTree::actualOne()(pos, OPERAND);
    rhs->compile();
    DataTree::actualOne()(pos, BINARY_OP_END);
    return std::move(nulptr);
}

util::sptr<flchk::Expression const> Conjunction::compile() const
{
    DataTree::actualOne()(pos, BINARY_OP_BEGIN, "&&")(pos, OPERAND);
    lhs->compile();
    DataTree::actualOne()(pos, OPERAND);
    rhs->compile();
    DataTree::actualOne()(pos, BINARY_OP_END);
    return std::move(nulptr);
}

util::sptr<flchk::Expression const> Disjunction::compile() const
{
    DataTree::actualOne()(pos, BINARY_OP_BEGIN, "||")(pos, OPERAND);
    lhs->compile();
    DataTree::actualOne()(pos, OPERAND);
    rhs->compile();
    DataTree::actualOne()(pos, BINARY_OP_END);
    return std::move(nulptr);
}

util::sptr<flchk::Expression const> Negation::compile() const
{
    DataTree::actualOne()(pos, PRE_UNARY_OP_BEGIN, "!")(pos, OPERAND);
    rhs->compile();
    DataTree::actualOne()(pos, PRE_UNARY_OP_END);
    return std::move(nulptr);
}

util::sptr<flchk::Expression const> Reference::compile() const
{
    DataTree::actualOne()(pos, IDENTIFIER, name);
    return std::move(nulptr);
}

util::sptr<flchk::Expression const> BoolLiteral::compile() const
{
    DataTree::actualOne()(pos, BOOLEAN, util::str(value));
    return std::move(nulptr);
}

util::sptr<flchk::Expression const> IntLiteral::compile() const
{
    DataTree::actualOne()(pos, INTEGER, value);
    return std::move(nulptr);
}

util::sptr<flchk::Expression const> FloatLiteral::compile() const
{
    DataTree::actualOne()(pos, FLOATING, value);
    return std::move(nulptr);
}

util::sptr<flchk::Expression const> StringLiteral::compile() const
{
    DataTree::actualOne()(pos, STRING, value);
    return std::move(nulptr);
}

util::sptr<flchk::Expression const> ListLiteral::compile() const
{
    DataTree::actualOne()(pos, LIST_BEGIN);
    std::for_each(value.begin()
                , value.end()
                , [&](util::sptr<Expression const> const& v)
                  {
                      v->compile();
                  });
    DataTree::actualOne()(pos, LIST_END);
    return std::move(nulptr);
}

util::sptr<flchk::Expression const> ListElement::compile() const
{
    DataTree::actualOne()(pos, LIST_ELEMENT);
    return std::move(nulptr);
}

util::sptr<flchk::Expression const> ListIndex::compile() const
{
    DataTree::actualOne()(pos, LIST_INDEX);
    return std::move(nulptr);
}

util::sptr<flchk::Expression const> ListAppend::compile() const
{
    DataTree::actualOne()(pos, BINARY_OP_BEGIN, "++")(pos, OPERAND);
    lhs->compile();
    DataTree::actualOne()(pos, OPERAND);
    rhs->compile();
    DataTree::actualOne()(pos, BINARY_OP_END);
    return std::move(nulptr);
}

util::sptr<flchk::Expression const> Call::compile() const
{
    DataTree::actualOne()(pos, FUNC_CALL_BEGIN, name);
    std::for_each(args.begin()
                , args.end()
                , [&](util::sptr<Expression const> const& expr)
                  {
                      DataTree::actualOne()(pos, ARGUMENT);
                      expr->compile();
                  });
    DataTree::actualOne()(pos, FUNC_CALL_END);
    return std::move(nulptr);
}

util::sptr<flchk::PipeBase const> PipeMap::compile() const
{
    DataTree::actualOne()(expr->pos, PIPE_MAP);
    expr->compile();
    return util::sptr<flchk::PipeBase const>(nullptr);
}

util::sptr<flchk::PipeBase const> PipeFilter::compile() const
{
    DataTree::actualOne()(expr->pos, PIPE_FILTER);
    expr->compile();
    return util::sptr<flchk::PipeBase const>(nullptr);
}

util::sptr<flchk::Expression const> ListPipeline::compile() const
{
    DataTree::actualOne()(pos, LIST_PIPELINE, util::str(int(pipeline.size())));
    list->compile();
    std::for_each(pipeline.begin()
                , pipeline.end()
                , [&](util::sptr<PipeBase const> const& pipe)
                  {
                      DataTree::actualOne()(pos, PIPE_BEGIN);
                      pipe->compile();
                      DataTree::actualOne()(pos, PIPE_END);
                  });
    return std::move(nulptr);
}

void ClauseBuilder::addArith(int indent_level, util::sptr<Expression const> arith)
{
    DataTree::actualOne()(arith->pos, indent_level, ARITHMETICS);
    arith->compile();
}

void ClauseBuilder::addVarDef(int indent_level
                            , std::string const& name
                            , util::sptr<Expression const> init)
{
    DataTree::actualOne()(init->pos, indent_level, VAR_DEF, name);
    init->compile();
}

void ClauseBuilder::addReturn(int indent_level, util::sptr<Expression const> ret_val)
{
    DataTree::actualOne()(ret_val->pos, indent_level, RETURN);
    ret_val->compile();
}

void ClauseBuilder::addReturnNothing(int indent_level, misc::position const& pos)
{
    DataTree::actualOne()(pos, indent_level, RETURN, "");
}

void ClauseBuilder::addFunction(int indent_level
                              , misc::position const& pos
                              , std::string const& name
                              , std::vector<std::string> const& params)
{
    DataTree::actualOne()(pos, indent_level, FUNC_DEF_HEAD_BEGIN, name);
    std::for_each(params.begin()
                , params.end()
                , [&](std::string const& param)
                  {
                      DataTree::actualOne()(pos, indent_level, IDENTIFIER, param);
                  });
    DataTree::actualOne()(pos, indent_level, FUNC_DEF_HEAD_END);
}

void ClauseBuilder::addIf(int indent_level, util::sptr<Expression const> condition)
{
    misc::position pos(condition->pos);
    DataTree::actualOne()(pos, indent_level, BRANCH_IF)(pos, indent_level, CONDITION_BEGIN);
    condition->compile();
    DataTree::actualOne()(pos, indent_level, CONDITION_END);
}

void ClauseBuilder::addIfnot(int indent_level, util::sptr<Expression const> condition)
{
    misc::position pos(condition->pos);
    DataTree::actualOne()(pos, indent_level, BRANCH_IFNOT)(pos, indent_level, CONDITION_BEGIN);
    condition->compile();
    DataTree::actualOne()(pos, indent_level, CONDITION_END);
}

void ClauseBuilder::addElse(int indent_level, misc::position const& pos)
{
    DataTree::actualOne()(pos, indent_level, BRANCH_ELSE);
}

util::sptr<flchk::Filter> ClauseBuilder::buildAndClear()
{
    return util::sptr<flchk::Filter>(nullptr);
}

AcceptorStack::AcceptorStack()
    : _packer(nullptr)
{}
