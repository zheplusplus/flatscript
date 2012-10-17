#include <algorithm>

#include <flowcheck/filter.h>
#include <flowcheck/function.h>
#include <flowcheck/expr-nodes.h>

#include "expr-nodes.h"

using namespace grammar;

util::sptr<flchk::Expression const> PreUnaryOp::compile() const
{
    return util::mkptr(new flchk::PreUnaryOp(pos, op_img, rhs->compile()));
}

util::sptr<flchk::Expression const> BinaryOp::compile() const
{
    return util::mkptr(new flchk::BinaryOp(pos, lhs->compile(), op_img, rhs->compile()));
}

util::sptr<flchk::Expression const> Conjunction::compile() const
{
    return util::mkptr(new flchk::Conjunction(pos, lhs->compile(), rhs->compile()));
}

util::sptr<flchk::Expression const> Disjunction::compile() const
{
    return util::mkptr(new flchk::Disjunction(pos, lhs->compile(), rhs->compile()));
}

util::sptr<flchk::Expression const> Negation::compile() const
{
    return util::mkptr(new flchk::Negation(pos, rhs->compile()));
}

util::sptr<flchk::Expression const> Reference::compile() const
{
    return util::mkptr(new flchk::Reference(pos, name));
}

util::sptr<flchk::Expression const> BoolLiteral::compile() const
{
    return util::mkptr(new flchk::BoolLiteral(pos, value));
}

util::sptr<flchk::Expression const> IntLiteral::compile() const
{
    return util::mkptr(new flchk::IntLiteral(pos, value));
}

util::sptr<flchk::Expression const> FloatLiteral::compile() const
{
    return util::mkptr(new flchk::FloatLiteral(pos, value));
}

util::sptr<flchk::Expression const> StringLiteral::compile() const
{
    return util::mkptr(new flchk::StringLiteral(pos, value));
}

static std::vector<util::sptr<flchk::Expression const>> compileList(
                            std::vector<util::sptr<Expression const>> const& values)
{
    std::vector<util::sptr<flchk::Expression const>> result;
    result.reserve(values.size());
    std::for_each(values.begin()
                , values.end()
                , [&](util::sptr<Expression const> const& expr)
                  {
                      result.push_back(expr->compile());
                  });
    return std::move(result);
}

util::sptr<flchk::Expression const> ListLiteral::compile() const
{
    return util::mkptr(new flchk::ListLiteral(pos, compileList(value)));
}

util::sptr<flchk::Expression const> ListElement::compile() const
{
    return util::mkptr(new flchk::ListElement(pos));
}

util::sptr<flchk::Expression const> ListIndex::compile() const
{
    return util::mkptr(new flchk::ListIndex(pos));
}

util::sptr<flchk::Expression const> ListAppend::compile() const
{
    return util::mkptr(new flchk::ListAppend(pos, lhs->compile(), rhs->compile()));
}

util::sptr<flchk::Expression const> Call::compile() const
{
    return cplMemberCall();
}

util::sptr<flchk::Call const> Call::cplMemberCall() const
{
    return util::mkptr(new flchk::Call(pos, name, compileList(args)));
}
