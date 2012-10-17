#include <algorithm>

#include <output/expr-writer.h>
#include <output/func-writer.h>
#include <report/errors.h>

#include "expr-nodes.h"
#include "function.h"
#include "block.h"

using namespace proto;

void BoolLiteral::write() const
{
    output::writeBool(value);
}

void IntLiteral::write() const
{
    output::writeInt(value);
}

void FloatLiteral::write() const
{
    output::writeFloat(value);
}

void StringLiteral::write() const
{
    output::writeString(value.c_str(), value.size());
}

void ListLiteral::write() const
{
    if (0 == value.size()) {
        output::emptyList();
        return;
    }
    output::listBegin();
    (*value.begin())->write();
    std::for_each(++value.begin()
                , value.end()
                , [&](util::sptr<Expression const> const& member)
                  {
                      output::listNextMember();
                      member->write();
                  });
    output::listEnd();
}

void ListLiteral::writeAsPipe() const
{
    if (0 == value.size()) {
        write();
        return;
    }
    output::listBegin();
    (*value.begin())->write();
    std::for_each(++value.begin()
                , value.end()
                , [&](util::sptr<Expression const> const& member)
                  {
                      output::listNextMember();
                      member->writeAsPipe();
                  });
    output::listEnd();
}

void ListElement::write() const
{
    error::pipeReferenceNotInListContext(pos);
}

void ListElement::writeAsPipe() const
{
    output::pipeElement();
}

void ListIndex::write() const
{
    error::pipeReferenceNotInListContext(pos);
}

void ListIndex::writeAsPipe() const
{
    output::pipeIndex();
}

void Reference::write() const
{
    output::reference(name);
}

void Call::write() const
{
    output::reference(name);
    output::writeArgsBegin();
    if (!args.empty()) {
        (*args.begin())->write();
        std::for_each(++args.begin()
                    , args.end()
                    , [&](util::sptr<Expression const> const& arg)
                      {
                          output::writeArgSeparator();
                          arg->write();
                      });
    }
    output::writeArgsEnd();
}

void Call::writeAsPipe() const
{
    output::reference(name);
    output::writeArgsBegin();
    if (!args.empty()) {
        (*args.begin())->writeAsPipe();
        std::for_each(++args.begin()
                    , args.end()
                    , [&](util::sptr<Expression const> const& arg)
                      {
                          output::writeArgSeparator();
                          arg->writeAsPipe();
                      });
    }
    output::writeArgsEnd();
}

void FuncReference::write() const
{
    output::reference(func->name);
}

void ListAppend::write() const
{
    lhs->write(); 
    output::listAppendBegin();
    rhs->write();
    output::listAppendEnd();
}

void ListAppend::writeAsPipe() const
{
    lhs->writeAsPipe(); 
    output::listAppendBegin();
    rhs->writeAsPipe();
    output::listAppendEnd();
}

void BinaryOp::write() const
{
    output::beginExpr();
    lhs->write();
    output::writeOperator(op);
    rhs->write();
    output::endExpr();
}

void BinaryOp::writeAsPipe() const
{
    output::beginExpr();
    lhs->writeAsPipe();
    output::writeOperator(op);
    rhs->writeAsPipe();
    output::endExpr();
}

void PreUnaryOp::write() const
{
    output::beginExpr();
    output::writeOperator(op);
    rhs->write();
    output::endExpr();
}

void PreUnaryOp::writeAsPipe() const
{
    output::beginExpr();
    output::writeOperator(op);
    rhs->writeAsPipe();
    output::endExpr();
}

void Conjunction::write() const
{
    output::beginExpr();
    lhs->write();
    output::writeOperator("&&");
    rhs->write();
    output::endExpr();
}

void Conjunction::writeAsPipe() const
{
    output::beginExpr();
    lhs->writeAsPipe();
    output::writeOperator("&&");
    rhs->writeAsPipe();
    output::endExpr();
}

void Disjunction::write() const
{
    output::beginExpr();
    lhs->write();
    output::writeOperator("||");
    rhs->write();
    output::endExpr();
}

void Disjunction::writeAsPipe() const
{
    output::beginExpr();
    lhs->writeAsPipe();
    output::writeOperator("||");
    rhs->writeAsPipe();
    output::endExpr();
}

void Negation::write() const
{
    output::beginExpr();
    output::writeOperator("!");
    rhs->write();
    output::endExpr();
}

void Negation::writeAsPipe() const
{
    output::beginExpr();
    output::writeOperator("!");
    rhs->writeAsPipe();
    output::endExpr();
}
