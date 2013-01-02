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
    _funcs.iter([&](util::sptr<Function const> const& func, int)
                {
                    func->write(dummyos());
                });
    _stmts.iter([&](util::sptr<Statement const> const& stmt, int)
                {
                    stmt->write(dummyos());
                });
    DataTree::actualOne()(SCOPE_END);
}

void Block::addStmt(util::sptr<Statement const> stmt)
{
    _stmts.append(std::move(stmt));
}

void Block::addFunc(util::sptr<Function const> func)
{
    _funcs.append(std::move(func));
}

void Block::append(util::sptr<Block> b)
{
    _stmts.append(std::move(b->_stmts));
    _funcs.append(std::move(b->_funcs));
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

void AsyncCallResultDef::write(std::ostream&) const
{
    DataTree::actualOne()(ASYNC_RESULT_DEF);
    async_result->str();
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

void AsyncPipeBody::write(std::ostream&) const
{
    DataTree::actualOne()(ASYNC_PIPE_BODY, int(pipe_type));
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

static void writeList(util::ptrarr<Expression const> const& list)
{
    list.iter([&](util::sptr<Expression const> const& member, int)
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
    items.iter([&](util::ptrkv<Expression const> const& kv, int)
               {
                   DataTree::actualOne()(pos, DICT_ITEM);
                   kv.key->str();
                   kv.value->str();
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

std::string AsyncReference::str() const
{
    DataTree::actualOne()(pos, ASYNC_REFERENCE);
    return "";
}

std::string Pipeline::str() const
{
    DataTree::actualOne()(pos, BINARY_OP, int(pipe_type));
    list->str();
    section->str();
    return "";
}

std::string AsyncPipeResult::str() const
{
    DataTree::actualOne()(pos, ASYNC_PIPE_RESULT);
    return "";
}

std::string AsyncPipe::str() const
{
    DataTree::actualOne()(pos, ASYNC_PIPELINE);
    list->str();
    recursion->write(dummyos());
    succession->write(dummyos());
    return "";
}
