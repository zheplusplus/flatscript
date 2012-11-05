#include <algorithm>

#include <semantic/expr-nodes.h>
#include <semantic/list-pipe.h>
#include <semantic/func-body-filter.h>

#include "expr-nodes.h"
#include "function.h"

using namespace grammar;

bool EmptyExpr::empty() const
{
    return true;
}

util::sptr<semantic::Expression const> EmptyExpr::reduceAsExpr() const
{
    return util::mkptr(new semantic::ListSlice::Default(pos));
}

util::sptr<semantic::Expression const> PreUnaryOp::reduceAsExpr() const
{
    return util::mkptr(new semantic::PreUnaryOp(pos, op_img, rhs->reduceAsExpr()));
}

util::sptr<semantic::Expression const> BinaryOp::reduceAsExpr() const
{
    if ("." == op_img) {
        return util::mkptr(new semantic::MemberAccess(
                                    pos, lhs->reduceAsExpr(), rhs->reduceAsName()));
    }
    if ("++" == op_img) {
        return util::mkptr(new semantic::ListAppend(pos, lhs->reduceAsExpr(), rhs->reduceAsExpr()));
    }
    if ("|:" == op_img) {
        return util::mkptr(new semantic::ListPipeMapper(
                                    pos, lhs->reduceAsExpr(), rhs->reduceAsExpr()));
    }
    if ("|?" == op_img) {
        return util::mkptr(new semantic::ListPipeFilter(
                                    pos, lhs->reduceAsExpr(), rhs->reduceAsExpr()));
    }
    return util::mkptr(new semantic::BinaryOp(
                                    pos, lhs->reduceAsExpr(), op_img, rhs->reduceAsExpr()));
}

util::sptr<semantic::Expression const> BinaryOp::reduceAsLeftValue() const
{
    if ("." != op_img) {
        return Expression::reduceAsLeftValue();
    }
    return util::mkptr(new semantic::MemberAccess(pos, lhs->reduceAsExpr(), rhs->reduceAsName()));
}

bool Identifier::isName() const
{
    return true;
}

std::string Identifier::reduceAsName() const
{
    return name;
}

util::sptr<semantic::Expression const> Identifier::reduceAsExpr() const
{
    return util::mkptr(new semantic::Reference(pos, name));
}

util::sptr<semantic::Expression const> BoolLiteral::reduceAsExpr() const
{
    return util::mkptr(new semantic::BoolLiteral(pos, value));
}

util::sptr<semantic::Expression const> IntLiteral::reduceAsExpr() const
{
    return util::mkptr(new semantic::IntLiteral(pos, value));
}

util::sptr<semantic::Expression const> FloatLiteral::reduceAsExpr() const
{
    return util::mkptr(new semantic::FloatLiteral(pos, value));
}

util::sptr<semantic::Expression const> StringLiteral::reduceAsExpr() const
{
    return util::mkptr(new semantic::StringLiteral(pos, value));
}

static std::vector<util::sptr<semantic::Expression const>> reduceList(
                                std::vector<util::sptr<Expression const>> const& list)
{
    std::vector<util::sptr<semantic::Expression const>> result;
    std::for_each(list.begin()
                , list.end()
                , [&](util::sptr<Expression const> const& i)
                  {
                      result.push_back(i->reduceAsExpr());
                  });
    return std::move(result);
}

util::sptr<semantic::Expression const> ListLiteral::reduceAsExpr() const
{
    return util::mkptr(new semantic::ListLiteral(pos, reduceList(value)));
}

util::sptr<semantic::Expression const> ListElement::reduceAsExpr() const
{
    return util::mkptr(new semantic::ListElement(pos));
}

util::sptr<semantic::Expression const> ListIndex::reduceAsExpr() const
{
    return util::mkptr(new semantic::ListIndex(pos));
}

util::sptr<semantic::Expression const> Call::reduceAsExpr() const
{
    return util::mkptr(new semantic::Call(pos, callee->reduceAsExpr(), reduceList(args)));
}

util::sptr<semantic::Expression const> Lookup::reduceAsExpr() const
{
    return util::mkptr(new semantic::Lookup(pos, collection->reduceAsExpr(), key->reduceAsExpr()));
}

util::sptr<semantic::Expression const> ListSlice::Default::reduceAsExpr() const
{
    return util::mkptr(new semantic::ListSlice::Default(pos));
}

util::sptr<semantic::Expression const> ListSlice::reduceAsExpr() const
{
    return util::mkptr(new semantic::ListSlice(pos
                                             , list->reduceAsExpr()
                                             , begin->reduceAsExpr()
                                             , end->reduceAsExpr()
                                             , step->reduceAsExpr()));
}

util::sptr<semantic::Expression const> Dictionary::reduceAsExpr() const
{
    std::vector<semantic::Dictionary::ItemType> reduced_items;
    std::for_each(items.begin()
                , items.end()
                , [&](ItemType const& item)
                  {
                      reduced_items.push_back(std::make_pair(item.first->reduceAsExpr()
                                                           , item.second->reduceAsExpr()));
                  });
    return util::mkptr(new semantic::Dictionary(pos, std::move(reduced_items)));
}

util::sptr<semantic::Expression const> Lambda::reduceAsExpr() const
{
    return util::mkptr(new semantic::Lambda(pos, param_names, body.compile(
                                    util::mkptr(new semantic::FuncBodyFilter(pos, param_names)))));
}
