#include <algorithm>
#include <vector>
#include <map>

#include <env.h>
#include <output/stmt-nodes.h>
#include <output/expr-nodes.h>
#include <output/list-pipe.h>
#include <output/function.h>
#include <util/string.h>

#include "test-common.h"

using namespace test;
using namespace output;

static std::set<std::string> nul_set;

std::set<std::string> const& stekin::preImported()
{
    return nul_set;
}

void Block::write(std::ostream&) const
{
    DataTree::actualOne()(SCOPE_BEGIN);
    std::for_each(_funcs.begin()
                , _funcs.end()
                , [&](util::sptr<Function const> const& func)
                  {
                      func->write(dummyos());
                  });
    std::for_each(_stmts.begin()
                , _stmts.end()
                , [&](util::sptr<Statement const> const& stmt)
                  {
                      stmt->write(dummyos());
                  });
    DataTree::actualOne()(SCOPE_END);
}

void Block::addStmt(util::sptr<Statement const> stmt)
{
    _stmts.push_back(std::move(stmt));
}

void Block::addFunc(util::sptr<Function const> func)
{
    _funcs.push_back(std::move(func));
}

void Function::write(std::ostream&) const
{
    DataTree::actualOne()(pos, FUNC_DECL, name, param_names.size());
    std::for_each(param_names.begin()
                , param_names.end()
                , [&](std::string const& pn)
                  {
                      DataTree::actualOne()(PARAMETER, pn);
                  });
    body->write(dummyos());
}

void Return::write(std::ostream&) const
{
    DataTree::actualOne()(RETURN);
    ret_val->str(false);
}

void ReturnNothing::write(std::ostream&) const
{
    DataTree::actualOne()(RETURN_NOTHING);
}

void Export::write(std::ostream&) const
{
    DataTree::actualOne()(EXPORT);
    std::for_each(export_point.begin()
                , export_point.end()
                , [&](std::string const& name)
                  {
                      DataTree::actualOne()(PARAMETER, name);
                  });
    DataTree::actualOne()(EXPORT_VALUE);
    value->str(false);
}

void AttrSet::write(std::ostream&) const
{
    DataTree::actualOne()(ATTR_SET);
    set_point->str(false);
    value->str(false);
}

void NameDef::write(std::ostream&) const
{
    DataTree::actualOne()(NAME_DEF, name);
    init->str(false);
}

void Branch::write(std::ostream&) const
{
    DataTree::actualOne()(BRANCH);
    predicate->str(false);
    consequence->write(dummyos());
    alternative->write(dummyos());
}

void Arithmetics::write(std::ostream&) const
{
    DataTree::actualOne()(ARITHMETICS);
    expr->str(false);
}

std::string BoolLiteral::str(bool) const
{
    DataTree::actualOne()(pos, BOOLEAN, util::str(value));
    return "";
}

std::string IntLiteral::str(bool) const
{
    DataTree::actualOne()(pos, INTEGER, util::str(value));
    return "";
}

std::string FloatLiteral::str(bool) const
{
    DataTree::actualOne()(pos, FLOATING, util::str(value));
    return "";
}

std::string StringLiteral::str(bool) const
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
                      member->str(false);
                  });
}

std::string ListLiteral::str(bool) const
{
    DataTree::actualOne()(pos, LIST, value.size());
    writeList(value);
    return "";
}

std::string ListElement::str(bool) const
{
    DataTree::actualOne()(pos, LIST_ELEMENT);
    return "";
}

std::string ListIndex::str(bool) const
{
    DataTree::actualOne()(pos, LIST_INDEX);
    return "";
}

std::string ListAppend::str(bool) const
{
    DataTree::actualOne()(pos, BINARY_OP, "++");
    lhs->str(false);
    rhs->str(false);
    return "";
}

std::string Reference::str(bool) const
{
    DataTree::actualOne()(pos, REFERENCE, name);
    return "";
}

std::string ImportedName::str(bool) const
{
    DataTree::actualOne()(pos, IMPORTED_NAME, name);
    return "";
}

std::string Call::str(bool) const
{
    DataTree::actualOne()(pos, CALL, args.size());
    callee->str(false);
    writeList(args);
    return "";
}

std::string MemberAccess::str(bool) const
{
    DataTree::actualOne()(pos, BINARY_OP, ".");
    referee->str(false);
    DataTree::actualOne()(pos, REFERENCE, member);
    return "";
}

std::string Lookup::str(bool) const
{
    DataTree::actualOne()(pos, BINARY_OP, "[]");
    collection->str(false);
    key->str(false);
    return "";
}

std::string ListSlice::str(bool) const
{
    DataTree::actualOne()(pos, LIST_SLICE);
    list->str(false);
    begin->str(false);
    end->str(false);
    step->str(false);
    return "";
}

std::string ListSlice::Default::str(bool) const
{
    DataTree::actualOne()(pos, LIST_SLICE_DEFAULT);
    return "";
}

std::string Dictionary::str(bool) const
{
    DataTree::actualOne()(pos, DICT_BEGIN);
    std::for_each(items.begin()
                , items.end()
                , [&](ItemType const& item)
                  {
                      DataTree::actualOne()(pos, DICT_ITEM);
                      item.first->str(false);
                      item.second->str(false);
                  });
    DataTree::actualOne()(pos, DICT_END);
    return "";
}

std::string BinaryOp::str(bool) const
{
    DataTree::actualOne()(pos, BINARY_OP, op);
    lhs->str(false);
    rhs->str(false);
    return "";
}

std::string PreUnaryOp::str(bool) const
{
    DataTree::actualOne()(pos, PRE_UNARY_OP, op);
    rhs->str(false);
    return "";
}

std::string Lambda::str(bool) const
{
    DataTree::actualOne()(pos, FUNC_DECL, param_names.size());
    std::for_each(param_names.begin()
                , param_names.end()
                , [&](std::string const& pn)
                  {
                      DataTree::actualOne()(PARAMETER, pn);
                  });
    body->write(dummyos());
    return "";
}

std::string ListPipeMapper::str(bool) const
{
    DataTree::actualOne()(pos, BINARY_OP, "|:");
    list->str(false);
    mapper->str(false);
    return "";
}

std::string ListPipeFilter::str(bool) const
{
    DataTree::actualOne()(pos, BINARY_OP, "|?");
    list->str(false);
    filter->str(false);
    return "";
}
