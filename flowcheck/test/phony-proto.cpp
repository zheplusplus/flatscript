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
    ret_val->stringify(false);
}

void ReturnNothing::write() const
{
    DataTree::actualOne()(RETURN_NOTHING);
}

void Import::write() const
{
    DataTree::actualOne()(IMPORT);
    std::for_each(names.begin()
                , names.end()
                , [&](std::string const& name)
                  {
                      DataTree::actualOne()(PARAMETER, name);
                  });
}

void AttrSet::write() const
{
    DataTree::actualOne()(ATTR_SET);
    set_point->stringify(false);
    value->stringify(false);
}

void NameDef::write() const
{
    DataTree::actualOne()(NAME_DEF, name);
    init->stringify(false);
}

void Branch::write() const
{
    DataTree::actualOne()(BRANCH);
    predicate->stringify(false);
    consequence->write();
    alternative->write();
}

void Arithmetics::write() const
{
    DataTree::actualOne()(ARITHMETICS);
    expr->stringify(false);
}

std::string BoolLiteral::stringify(bool) const
{
    DataTree::actualOne()(pos, BOOLEAN, util::str(value));
    return "";
}

std::string IntLiteral::stringify(bool) const
{
    DataTree::actualOne()(pos, INTEGER, util::str(value));
    return "";
}

std::string FloatLiteral::stringify(bool) const
{
    DataTree::actualOne()(pos, FLOATING, util::str(value));
    return "";
}

std::string StringLiteral::stringify(bool) const
{
    DataTree::actualOne()(pos, STRING, value);
    return "";
}

static void writeList(std::vector<util::sptr<Expression const>> const& list)
{
    std::for_each(list.begin()
                , list.end()
                , [&](util::sptr<Expression const> const& member)
                  {
                      member->stringify(false);
                  });
}

std::string ListLiteral::stringify(bool) const
{
    DataTree::actualOne()(pos, LIST, value.size());
    writeList(value);
    return "";
}

std::string ListElement::stringify(bool) const
{
    DataTree::actualOne()(pos, LIST_ELEMENT);
    return "";
}

std::string ListIndex::stringify(bool) const
{
    DataTree::actualOne()(pos, LIST_INDEX);
    return "";
}

std::string ListAppend::stringify(bool) const
{
    DataTree::actualOne()(pos, BINARY_OP, "++");
    lhs->stringify(false);
    rhs->stringify(false);
    return "";
}

std::string Reference::stringify(bool) const
{
    DataTree::actualOne()(pos, REFERENCE, name);
    return "";
}

std::string Call::stringify(bool) const
{
    DataTree::actualOne()(pos, CALL, args.size());
    callee->stringify(false);
    writeList(args);
    return "";
}

std::string MemberAccess::stringify(bool) const
{
    DataTree::actualOne()(pos, BINARY_OP, ".");
    referee->stringify(false);
    DataTree::actualOne()(pos, REFERENCE, member);
    return "";
}

std::string Lookup::stringify(bool) const
{
    DataTree::actualOne()(pos, BINARY_OP, "[]");
    collection->stringify(false);
    key->stringify(false);
    return "";
}

std::string ListSlice::stringify(bool) const
{
    DataTree::actualOne()(pos, LIST_SLICE);
    list->stringify(false);
    begin->stringify(false);
    end->stringify(false);
    step->stringify(false);
    return "";
}

std::string ListSlice::Default::stringify(bool) const
{
    DataTree::actualOne()(pos, LIST_SLICE_DEFAULT);
    return "";
}

std::string Dictionary::stringify(bool) const
{
    DataTree::actualOne()(pos, DICT_BEGIN);
    std::for_each(items.begin()
                , items.end()
                , [&](ItemType const& item)
                  {
                      DataTree::actualOne()(pos, DICT_ITEM);
                      item.first->stringify(false);
                      item.second->stringify(false);
                  });
    DataTree::actualOne()(pos, DICT_END);
    return "";
}

std::string BinaryOp::stringify(bool) const
{
    DataTree::actualOne()(pos, BINARY_OP, op);
    lhs->stringify(false);
    rhs->stringify(false);
    return "";
}

std::string PreUnaryOp::stringify(bool) const
{
    DataTree::actualOne()(pos, PRE_UNARY_OP, op);
    rhs->stringify(false);
    return "";
}

std::string Conjunction::stringify(bool) const
{
    DataTree::actualOne()(pos, BINARY_OP, "&&");
    lhs->stringify(false);
    rhs->stringify(false);
    return "";
}

std::string Disjunction::stringify(bool) const
{
    DataTree::actualOne()(pos, BINARY_OP, "||");
    lhs->stringify(false);
    rhs->stringify(false);
    return "";
}

std::string Negation::stringify(bool) const
{
    DataTree::actualOne()(pos, PRE_UNARY_OP, "!");
    rhs->stringify(false);
    return "";
}

std::string ListPipeline::stringify(bool) const
{
    DataTree::actualOne()(pos, LIST_PIPELINE, pipeline.size());
    list->stringify(false);
    std::for_each(pipeline.begin()
                , pipeline.end()
                , [&](util::sptr<PipeBase const> const& pipe)
                  {
                      pipe->stringify("");
                  });
    return "";
}

std::string PipeMap::stringify(std::string const&) const
{
    DataTree::actualOne()(PIPE_MAP);
    expr->stringify(false);
    return "";
}

std::string PipeFilter::stringify(std::string const&) const
{
    DataTree::actualOne()(PIPE_FILTER);
    expr->stringify(false);
    return "";
}
