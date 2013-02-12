#include <algorithm>
#include <map>

#include <output/stmt-nodes.h>
#include <output/expr-nodes.h>
#include <output/list-pipe.h>
#include <output/function.h>
#include <output/methods.h>
#include <util/string.h>

#include "test-common.h"

using namespace test;
using namespace output;

static void writeList(util::ptrarr<Expression const> const& list)
{
    list.iter([&](util::sptr<Expression const> const& member, int)
              {
                  member->str();
              });
}

void Block::write(std::ostream&) const
{
    DataTree::actualOne()(SCOPE_BEGIN);
    std::for_each(_local_decls.begin()
                , _local_decls.end()
                , [&](std::string const& s)
                  {
                      DataTree::actualOne()(FWD_DECL, s);
                  });
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

void Block::setLocalDecls(std::set<std::string> const& decls)
{
    _local_decls = decls;
}

void Function::write(std::ostream&) const
{
    std::vector<std::string> params(parameters());
    DataTree::actualOne()(FUNCTION, mangledName(), params.size());
    std::for_each(params.begin()
                , params.end()
                , [&](std::string const& pn)
                  {
                      DataTree::actualOne()(PARAMETER, pn);
                  });
    body()->write(dummyos());
}

util::sref<Statement const> RegularFunction::body() const
{
    return *body_stmt;
}

std::string RegularFunction::mangledName() const
{
    return name;
}

std::vector<std::string> RegularFunction::parameters() const
{
    return params;
}

std::vector<std::string> RegularAsyncFunction::parameters() const
{
    std::vector<std::string> p(params);
    p.insert(p.begin() + async_param_index, "# RegularAsyncParam");
    return p;
}

std::string RegularAsyncReturnCall::str() const
{
    DataTree::actualOne()(pos, REGULAR_ASYNC_RETURN);
    val->str();
    return "";
}

util::sref<Statement const> ConditionalCallback::body() const
{
    return *_body;
}

std::string ConditionalCallback::mangledName() const
{
    return "# ConditionalCallback";
}

std::vector<std::string> ConditionalCallback::parameters() const
{
    return std::vector<std::string>({ "ConditionalCallback # Parameter" });
}

util::sref<Block> ConditionalCallback::bodyFlow()
{
    return *_body;
}

std::string FunctionInvocation::str() const
{
    DataTree::actualOne()(pos, FUNC_INVOKE, args.size());
    writeList(args);
    return "";
}

std::string MemberAccess::str() const
{
    DataTree::actualOne()(pos, BINARY_OP, "[.]");
    referee->str();
    DataTree::actualOne()(pos, REFERENCE, member);
    return "";
}

void Return::write(std::ostream&) const
{
    DataTree::actualOne()(RETURN);
    ret_val->str();
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

void ThisDeclaration::write(std::ostream&) const
{
    DataTree::actualOne()(DEC_THIS);
}

void PipelineContinue::write(std::ostream&) const
{
    DataTree::actualOne()(PIPELINE_CONTINUE);
}

std::string Undefined::str() const
{
    DataTree::actualOne()(pos, UNDEFINED);
    return "";
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

std::string PipeResult::str() const
{
    DataTree::actualOne()(pos, PIPE_RESULT);
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

std::string Assignment::str() const
{
    DataTree::actualOne()(pos, BINARY_OP, "[=]");
    lhs->str();
    rhs->str();
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
    DataTree::actualOne()(pos, FUNCTION, param_names.size());
    std::for_each(param_names.begin()
                , param_names.end()
                , [&](std::string const& pn)
                  {
                      DataTree::actualOne()(PARAMETER, pn);
                  });
    if (copy_decls) {
        DataTree::actualOne()(COPY_PARAM_DECL);
    }
    body->write(dummyos());
    return "";
}

std::string RegularAsyncLambda::str() const
{
    std::vector<std::string> p(param_names);
    p.insert(p.begin() + async_param_index, "# RegularAsyncParam");
    DataTree::actualOne()(pos, FUNCTION, p.size());
    std::for_each(p.begin()
                , p.end()
                , [&](std::string const& pn)
                  {
                      DataTree::actualOne()(PARAMETER, pn);
                  });
    body->write(dummyos());
    return "";
}

std::string This::str() const
{
    DataTree::actualOne()(pos, THIS);
    return "";
}

std::string Conditional::str() const
{
    DataTree::actualOne()(pos, CONDITIONAL);
    predicate->str();
    consequence->str();
    alternative->str();
    return "";
}

std::string AsyncReference::str() const
{
    DataTree::actualOne()(pos, ASYNC_REFERENCE);
    return "";
}

std::string RegularAsyncCallbackArg::str() const
{
    DataTree::actualOne()(pos, FUNCTION);
    DataTree::actualOne()(PARAMETER, "# RegularAsyncCallbackParameters");
    raiser("");
    body->write(dummyos());
    return "";
}

std::string AsyncPipeline::str() const
{
    DataTree::actualOne()(pos, ASYNC_PIPELINE);
    list->str();
    recursion->write(dummyos());
    succession->write(dummyos());
    raiser("");
    return "";
}

std::string SyncPipeline::str() const
{
    DataTree::actualOne()(pos, SYNC_PIPELINE);
    list->str();
    section->write(dummyos());
    return "";
}

Method method::throwExc()
{
    return [](std::string const&)
           {
               DataTree::actualOne()(EXC_THROW);
               return "";
           };
}

Method method::callbackExc()
{
    return [](std::string const&)
           {
               DataTree::actualOne()(EXC_CALLBACK);
               return "";
           };
}

int Block::count() const { return 0; }
int Export::count() const { return 0; }
