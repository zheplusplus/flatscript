#include <algorithm>
#include <vector>
#include <map>

#include <proto/stmt-nodes.h>
#include <proto/expr-nodes.h>
#include <proto/list-pipe.h>
#include <proto/function.h>
#include <util/string.h>

#include "test-common.h"

using namespace test;
using namespace proto;

void Block::addStmt(util::sptr<Statement const> stmt)
{
    _stmts.push_back(std::move(stmt));
}

void Block::addFunc(util::sptr<Function const> func)
{
    _funcs.push_back(std::move(func));
}

void Block::write() const
{
    DataTree::actualOne()(SCOPE_BEGIN);
    std::for_each(_funcs.begin()
                , _funcs.end()
                , [&](util::sptr<Function const> const& func)
                  {
                      func->write();
                  });
    std::for_each(_stmts.begin()
                , _stmts.end()
                , [&](util::sptr<Statement const> const& stmt)
                  {
                      stmt->write();
                  });
    DataTree::actualOne()(SCOPE_END);
}

void Function::write() const
{
    DataTree::actualOne()(pos, FUNC_DECL, name, param_names.size());
    std::for_each(param_names.begin()
                , param_names.end()
                , [&](std::string const& pn)
                  {
                      DataTree::actualOne()(PARAMETER, pn);
                  });
    body->write();
}

void Return::write() const
{
    DataTree::actualOne()(RETURN);
    ret_val->write();
}

void ReturnNothing::write() const
{
    DataTree::actualOne()(RETURN_NOTHING);
}

void VarDef::write() const
{
    DataTree::actualOne()(VAR_DEF, name);
    init->write();
}

void Branch::write() const
{
    DataTree::actualOne()(BRANCH);
    predicate->write();
    consequence->write();
    alternative->write();
}

void Arithmetics::write() const
{
    DataTree::actualOne()(ARITHMETICS);
    expr->write();
}

void BoolLiteral::write() const
{
    DataTree::actualOne()(pos, BOOLEAN, util::str(value));
}

void IntLiteral::write() const
{
    DataTree::actualOne()(pos, INTEGER, util::str(value));
}

void FloatLiteral::write() const
{
    DataTree::actualOne()(pos, FLOATING, util::str(value));
}

void StringLiteral::write() const
{
    DataTree::actualOne()(pos, STRING, value);
}

static void writeList(std::vector<util::sptr<Expression const>> const& list)
{
    std::for_each(list.begin()
                , list.end()
                , [&](util::sptr<Expression const> const& member)
                  {
                      member->write();
                  });
}

void ListLiteral::write() const
{
    DataTree::actualOne()(pos, LIST, value.size());
    writeList(value);
}

void ListElement::write() const
{
    DataTree::actualOne()(pos, LIST_ELEMENT);
}

void ListIndex::write() const
{
    DataTree::actualOne()(pos, LIST_INDEX);
}

void ListAppend::write() const
{
    DataTree::actualOne()(pos, BINARY_OP, "++");
    lhs->write();
    rhs->write();
}

void Reference::write() const
{
    DataTree::actualOne()(pos, REFERENCE, name);
}

void Call::write() const
{
    DataTree::actualOne()(pos, CALL, name, args.size());
    writeList(args);
}

void BinaryOp::write() const
{
    DataTree::actualOne()(pos, BINARY_OP, op);
    lhs->write();
    rhs->write();
}

void PreUnaryOp::write() const
{
    DataTree::actualOne()(pos, PRE_UNARY_OP, op);
    rhs->write();
}

void Conjunction::write() const
{
    DataTree::actualOne()(pos, BINARY_OP, "&&");
    lhs->write();
    rhs->write();
}

void Disjunction::write() const
{
    DataTree::actualOne()(pos, BINARY_OP, "||");
    lhs->write();
    rhs->write();
}

void Negation::write() const
{
    DataTree::actualOne()(pos, PRE_UNARY_OP, "!");
    rhs->write();
}

void ListPipeline::write() const
{
    DataTree::actualOne()(pos, LIST_PIPELINE, pipeline.size());
    list->write();
    std::for_each(pipeline.begin()
                , pipeline.end()
                , [&](util::sptr<PipeBase const> const& pipe)
                  {
                      pipe->writeBegin();
                  });
}

void PipeMap::writeBegin() const
{
    DataTree::actualOne()(PIPE_MAP);
    expr->write();
}

void PipeMap::writeEnd() const {}

void PipeFilter::writeBegin() const
{
    DataTree::actualOne()(PIPE_FILTER);
    expr->write();
}

void PipeFilter::writeEnd() const {}

void Expression::writeAsPipe() const {}
void ListLiteral::writeAsPipe() const {}
void ListElement::writeAsPipe() const {}
void ListIndex::writeAsPipe() const {}
void Call::writeAsPipe() const {}
void ListAppend::writeAsPipe() const {}
void BinaryOp::writeAsPipe() const {}
void PreUnaryOp::writeAsPipe() const {}
void Conjunction::writeAsPipe() const {}
void Disjunction::writeAsPipe() const {}
void Negation::writeAsPipe() const {}
void ListPipeline::writeAsPipe() const {}
