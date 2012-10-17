#include "node-base.h"
#include "expr-nodes.h"

using namespace flchk;

bool Expression::isLiteral() const
{
    return false;
}

bool Expression::boolValue() const
{
    return false;
}

util::sptr<Expression const> Expression::operate(misc::position const& op_pos
                                               , std::string const& op_img
                                               , mpz_class const& rhs) const
{
    return util::mkptr(
            new BinaryOp(pos, fold(), op_img, util::mkptr(new IntLiteral(op_pos, rhs))));
}

util::sptr<Expression const> Expression::operate(misc::position const& op_pos
                                               , std::string const& op_img
                                               , mpf_class const& rhs) const
{
    return util::mkptr(
            new BinaryOp(pos, fold(), op_img, util::mkptr(new FloatLiteral(op_pos, rhs))));
}

util::sptr<Expression const> Expression::operate(misc::position const& op_pos
                                               , std::string const& op_img
                                               , bool rhs) const
{
    return util::mkptr(
            new BinaryOp(pos, fold(), op_img, util::mkptr(new BoolLiteral(op_pos, rhs))));
}

util::sptr<Expression const> Expression::asRhs(misc::position const& op_pos
                                             , std::string const& op_img
                                             , util::sptr<Expression const> lhs) const
{
    return util::mkptr(new BinaryOp(op_pos, std::move(lhs), op_img, fold()));
}

util::sptr<Expression const> Expression::asRhs(misc::position const& op_pos
                                             , std::string const& op_img) const
{
    return util::mkptr(new PreUnaryOp(op_pos, op_img, fold()));
}
