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
    ret_val->str();
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
    value->str();
}

void AttrSet::write(std::ostream&) const
{
    DataTree::actualOne()(ATTR_SET);
    set_point->str();
    value->str();
}

void NameDef::write(std::ostream&) const
{
    DataTree::actualOne()(NAME_DEF, name);
    init->str();
}

void Branch::write(std::ostream&) const
{
    DataTree::actualOne()(BRANCH);
    predicate->str();
    consequence->write(dummyos());
    alternative->write(dummyos());
}

void Arithmetics::write(std::ostream&) const
{
    DataTree::actualOne()(ARITHMETICS);
    expr->str();
}

std::string Expression::strAsProp() const
{
    return str();
}

std::string PropertyNameExpr::strAsProp() const
{
    return str();
}

std::string BoolLiteral::str() const
{
    DataTree::actualOne()(pos, BOOLEAN, util::str(value));
    return "";
}

std::string IntLiteral::str() const
{
    DataTree::actualOne()(pos, INTEGER, util::str(value));
    return "";
}

std::string FloatLiteral::str() const
{
    DataTree::actualOne()(pos, FLOATING, util::str(value));
    return "";
}

std::string StringLiteral::str() const
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
                      member->str();
                  });
}

std::string ListLiteral::str() const
{
    DataTree::actualOne()(pos, LIST, value.size());
    writeList(value);
    return "";
}

std::string PipeElement::str() const
{
    DataTree::actualOne()(pos, PIPE_ELEMENT);
    return "";
}

std::string PipeIndex::str() const
{
    DataTree::actualOne()(pos, PIPE_INDEX);
    return "";
}

std::string PipeKey::str() const
{
    DataTree::actualOne()(pos, PIPE_KEY);
    return "";
}

std::string ListAppend::str() const
{
    DataTree::actualOne()(pos, BINARY_OP, "++");
    lhs->str();
    rhs->str();
    return "";
}

std::string Reference::str() const
{
    DataTree::actualOne()(pos, REFERENCE, name);
    return "";
}

std::string ImportedName::str() const
{
    DataTree::actualOne()(pos, IMPORTED_NAME, name);
    return "";
}

std::string Call::str() const
{
    DataTree::actualOne()(pos, CALL, args.size());
    callee->str();
    writeList(args);
    return "";
}

std::string MemberAccess::str() const
{
    DataTree::actualOne()(pos, BINARY_OP, ".");
    referee->str();
    DataTree::actualOne()(pos, REFERENCE, member);
    return "";
}

std::string Lookup::str() const
{
    DataTree::actualOne()(pos, BINARY_OP, "[]");
    collection->str();
    key->str();
    return "";
}

std::string ListSlice::str() const
{
    DataTree::actualOne()(pos, LIST_SLICE);
    list->str();
    begin->str();
    end->str();
    step->str();
    return "";
}

std::string ListSlice::Default::str() const
{
    DataTree::actualOne()(pos, LIST_SLICE_DEFAULT);
    return "";
}

std::string Dictionary::str() const
{
    DataTree::actualOne()(pos, DICT_BEGIN);
    std::for_each(items.begin()
                , items.end()
                , [&](ItemType const& item)
                  {
                      DataTree::actualOne()(pos, DICT_ITEM);
                      item.first->str();
                      item.second->str();
                  });
    DataTree::actualOne()(pos, DICT_END);
    return "";
}

std::string BinaryOp::str() const
{
    DataTree::actualOne()(pos, BINARY_OP, op);
    lhs->str();
    rhs->str();
    return "";
}

std::string PreUnaryOp::str() const
{
    DataTree::actualOne()(pos, PRE_UNARY_OP, op);
    rhs->str();
    return "";
}

std::string Lambda::str() const
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

std::string ListPipeMapper::str() const
{
    DataTree::actualOne()(pos, BINARY_OP, "|:");
    list->str();
    mapper->str();
    return "";
}

std::string ListPipeFilter::str() const
{
    DataTree::actualOne()(pos, BINARY_OP, "|?");
    list->str();
    filter->str();
    return "";
}
