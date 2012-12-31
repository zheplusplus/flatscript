#include <algorithm>

#include <semantic/expr-nodes.h>
#include <semantic/list-pipe.h>
#include <semantic/func-body-filter.h>
#include <report/errors.h>

#include "expr-nodes.h"
#include "function.h"

using namespace grammar;

bool EmptyExpr::empty() const
{
    return true;
}

util::sptr<semantic::Expression const> EmptyExpr::reduceAsExpr(bool) const
{
    return util::mkptr(new semantic::ListSlice::Default(pos));
}

util::sptr<semantic::Expression const> PreUnaryOp::reduceAsExpr(bool in_pipe) const
{
    if ("typeof" == op_img) {
        return util::mkptr(new semantic::TypeOf(pos, rhs->reduceAsExpr(in_pipe)));
    }
    return util::mkptr(new semantic::PreUnaryOp(pos, op_img, rhs->reduceAsExpr(in_pipe)));
}

util::sptr<semantic::Expression const> BinaryOp::reduceAsExpr(bool in_pipe) const
{
    if ("." == op_img) {
        return util::mkptr(new semantic::MemberAccess(
                            pos, lhs->reduceAsExpr(in_pipe), rhs->reduceAsName()));
    }
    if ("++" == op_img) {
        return util::mkptr(new semantic::ListAppend(
                            pos, lhs->reduceAsExpr(in_pipe), rhs->reduceAsExpr(in_pipe)));
    }
    if ("|:" == op_img) {
        return util::mkptr(new semantic::ListPipeMapper(
                            pos, lhs->reduceAsExpr(in_pipe), rhs->reduceAsExpr(true)));
    }
    if ("|?" == op_img) {
        return util::mkptr(new semantic::ListPipeFilter(
                            pos, lhs->reduceAsExpr(in_pipe), rhs->reduceAsExpr(true)));
    }
    return util::mkptr(new semantic::BinaryOp(
                            pos, lhs->reduceAsExpr(in_pipe), op_img, rhs->reduceAsExpr(in_pipe)));
}

util::sptr<semantic::Expression const> BinaryOp::reduceAsLeftValue(bool in_pipe) const
{
    if ("." != op_img) {
        return Expression::reduceAsLeftValue(in_pipe);
    }
    return util::mkptr(
            new semantic::MemberAccess(pos, lhs->reduceAsExpr(in_pipe), rhs->reduceAsName()));
}

bool Identifier::isName() const
{
    return true;
}

std::string Identifier::reduceAsName() const
{
    return name;
}

util::sptr<semantic::Expression const> Identifier::reduceAsExpr(bool) const
{
    return util::mkptr(new semantic::Reference(pos, name));
}

util::sptr<semantic::Expression const> BoolLiteral::reduceAsExpr(bool) const
{
    return util::mkptr(new semantic::BoolLiteral(pos, value));
}

util::sptr<semantic::Expression const> IntLiteral::reduceAsExpr(bool) const
{
    return util::mkptr(new semantic::IntLiteral(pos, value));
}

util::sptr<semantic::Expression const> FloatLiteral::reduceAsExpr(bool) const
{
    return util::mkptr(new semantic::FloatLiteral(pos, value));
}

util::sptr<semantic::Expression const> StringLiteral::reduceAsExpr(bool) const
{
    return util::mkptr(new semantic::StringLiteral(pos, value));
}

static std::vector<util::sptr<semantic::Expression const>> reduceList(
                        std::vector<util::sptr<Expression const>> const& list, bool in_pipe)
{
    std::vector<util::sptr<semantic::Expression const>> result;
    std::for_each(list.begin()
                , list.end()
                , [&](util::sptr<Expression const> const& i)
                  {
                      result.push_back(i->reduceAsExpr(in_pipe));
                  });
    return std::move(result);
}

util::sptr<semantic::Expression const> ListLiteral::reduceAsExpr(bool in_pipe) const
{
    return util::mkptr(new semantic::ListLiteral(pos, reduceList(value, in_pipe)));
}

util::sptr<semantic::Expression const> PipeElement::reduceAsExpr(bool in_pipe) const
{
    if (!in_pipe) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new semantic::PipeElement(pos));
}

util::sptr<semantic::Expression const> PipeIndex::reduceAsExpr(bool in_pipe) const
{
    if (!in_pipe) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new semantic::PipeIndex(pos));
}

util::sptr<semantic::Expression const> PipeKey::reduceAsExpr(bool in_pipe) const
{
    if (!in_pipe) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new semantic::PipeKey(pos));
}

util::sptr<semantic::Expression const> Call::reduceAsExpr(bool in_pipe) const
{
    return util::mkptr(
            new semantic::Call(pos, callee->reduceAsExpr(in_pipe), reduceList(args, in_pipe)));
}

util::sptr<semantic::Expression const> Lookup::reduceAsExpr(bool in_pipe) const
{
    return util::mkptr(
          new semantic::Lookup(pos, collection->reduceAsExpr(in_pipe), key->reduceAsExpr(in_pipe)));
}

util::sptr<semantic::Expression const> Lookup::reduceAsLeftValue(bool in_pipe) const
{
    return reduceAsExpr(in_pipe);
}

util::sptr<semantic::Expression const> ListSlice::Default::reduceAsExpr(bool) const
{
    return util::mkptr(new semantic::ListSlice::Default(pos));
}

util::sptr<semantic::Expression const> ListSlice::reduceAsExpr(bool in_pipe) const
{
    return util::mkptr(new semantic::ListSlice(pos
                                             , list->reduceAsExpr(in_pipe)
                                             , begin->reduceAsExpr(in_pipe)
                                             , end->reduceAsExpr(in_pipe)
                                             , step->reduceAsExpr(in_pipe)));
}

util::sptr<semantic::Expression const> Dictionary::reduceAsExpr(bool in_pipe) const
{
    std::vector<semantic::Dictionary::ItemType> reduced_items;
    std::for_each(items.begin()
                , items.end()
                , [&](ItemType const& item)
                  {
                      reduced_items.push_back(std::make_pair(item.first->reduceAsExpr(in_pipe)
                                                           , item.second->reduceAsExpr(in_pipe)));
                  });
    return util::mkptr(new semantic::Dictionary(pos, std::move(reduced_items)));
}

util::sptr<semantic::Expression const> Lambda::reduceAsExpr(bool) const
{
    return util::mkptr(new semantic::Lambda(pos, param_names, body.compile(
                                    util::mkptr(new semantic::FuncBodyFilter(pos, param_names)))));
}
