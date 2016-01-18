#include <output/stmt-nodes.h>
#include <output/expr-nodes.h>
#include <output/list-pipe.h>
#include <output/function-impl.h>
#include <output/class.h>
#include <output/methods.h>
#include <output/name-mangler.h>
#include <util/string.h>
#include <including.h>

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
    for (auto const& s: this->_local_decls) {
        DataTree::actualOne()(FWD_DECL, s);
    }
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

void Function::write(std::ostream&) const
{
    std::vector<std::string> params(parameters());
    DataTree::actualOne()(FUNCTION, mangledName(), params.size());
    for (auto const& pn: params) {
        DataTree::actualOne()(PARAMETER, pn);
    }
    body()->write(dummyos());
}

void ClassInitFunc::write(std::ostream&) const
{
    DataTree::actualOne()(CLASS_INIT_FN, name, member_funcs.size());
    DataTree::actualOne()(CLASS_INIT_FN, inherit ? 1 : 0);
    for (auto const& fn: member_funcs) {
        DataTree::actualOne()(MEMBER_FUNC, fn.first);
        fn.second->str();
    }
}

void ClassInitCall::write(std::ostream&) const
{
    DataTree::actualOne()(CLASS_INIT_ST, name, base_class_or_nul.nul() ? 0 : 1);
    if (base_class_or_nul.not_nul()) {
        base_class_or_nul->str();
    }
}

util::sptr<Expression const> Function::callMe(util::ptrarr<Expression const> args) const
{
    return util::mkptr(new output::Call(
                util::mkptr(new Reference(mangledName())), std::move(args)));
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

util::sref<Statement const> AnonymousCallback::body() const
{
    return *_body;
}

std::string AnonymousCallback::mangledName() const
{
    return "# AnonymousCallback";
}

util::sref<Block> AnonymousCallback::bodyFlow()
{
    return *_body;
}

std::vector<std::string> ConditionalCallback::parameters() const
{
    return std::vector<std::string>({ "ConditionalCallback # Parameter" });
}

std::vector<std::string> NoParamCallback::parameters() const
{
    return std::vector<std::string>();
}

std::vector<std::string> AsyncCatchFunc::parameters() const
{
    return std::vector<std::string>({ "AsyncCatchFunc # Parameter" });
}

std::vector<std::string> AsyncCatcherDeprecated::parameters() const
{
    return std::vector<std::string>({ "AsyncCatcher # Parameter" });
}

std::string ModuleInitFunc::mangledName() const
{
    return "# ModuleInitFunc";
}

std::vector<std::string> ModuleInitFunc::parameters() const
{
    return std::vector<std::string>({ "ModuleInitFunc # Parameter" });
}

util::sptr<Expression const> ModuleInitFunc::exportArg() const
{
    return util::mkptr(new ModuleInitFunc::InitTarget(this->module_id));
}

std::string ModuleInitFunc::InitTarget::str() const
{
    return "# ModuleInitFunc::InitTarget";
}

util::sptr<semantic::Statement const> flats::compileFile(
                std::string const& file, misc::position const& p)
{
    DataTree::actualOne()(INCLUDE, file);
    return util::mkptr(new semantic::Block(p));
}

std::string MemberAccess::str() const
{
    DataTree::actualOne()(BINARY_OP, "[.]");
    referee->str();
    DataTree::actualOne()(REFERENCE, member);
    return "";
}

Export::Export(std::vector<std::string> e, util::sptr<Expression const> v)
    : export_point(std::move(e))
    , value(std::move(v))
{}

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

void ThisDeclaration::write(std::ostream&) const
{
    DataTree::actualOne()(DEC_THIS);
}

void ExceptionStall::write(std::ostream&) const
{
    DataTree::actualOne()(TRY);
    try_block->write(dummyos());
    DataTree::actualOne()(CATCH, except_name);
    catch_block->write(dummyos());
}

void ExceptionStallDeprecated::write(std::ostream&) const
{
    DataTree::actualOne()(TRY);
    try_block->write(dummyos());
    DataTree::actualOne()(CATCH);
    catch_block->write(dummyos());
}

void SyncRangeIteration::write(std::ostream&) const
{
    DataTree::actualOne()(SYNC_FOR_RANGE, util::str(this->step)
                                                + ':' + (this->has_ret ? 'R' : '-')
                                                + ':' + (this->has_break ? 'B' : '-'));
    this->reference->str();
    this->begin->str();
    this->end->str();
    this->section->write(dummyos());
}

void AsyncRangeIteration::write(std::ostream&) const
{
    DataTree::actualOne()(ASYNC_FOR_RANGE, this->step);
    this->reference->str();
    this->begin->str();
    this->end->str();
    this->section->write(dummyos());
    this->succession->write(dummyos());
}

void ExprScheme::write(std::ostream&) const
{
    method->scheme("");
    expr->str();
}

std::string Undefined::str() const
{
    DataTree::actualOne()(UNDEFINED);
    return "";
}

std::string BoolLiteral::str() const
{
    DataTree::actualOne()(BOOLEAN, util::str(value));
    return "";
}

std::string IntLiteral::str() const
{
    DataTree::actualOne()(INTEGER, util::str(value));
    return "";
}

std::string FloatLiteral::str() const
{
    DataTree::actualOne()(FLOATING, util::str(value));
    return "";
}

std::string StringLiteral::str() const
{
    DataTree::actualOne()(STRING, value);
    return "";
}

std::string RegEx::str() const
{
    DataTree::actualOne()(REGEXP, value);
    return "";
}

std::string ListLiteral::str() const
{
    DataTree::actualOne()(LIST, value.size());
    writeList(value);
    return "";
}

std::string PipeElement::str() const
{
    DataTree::actualOne()(PIPE_ELEMENT);
    return "";
}

std::string PipeIndex::str() const
{
    DataTree::actualOne()(PIPE_INDEX);
    return "";
}

std::string PipeKey::str() const
{
    DataTree::actualOne()(PIPE_KEY);
    return "";
}

std::string PipeResult::str() const
{
    DataTree::actualOne()(PIPE_RESULT);
    return "";
}

std::string PipeBreak::str() const
{
    DataTree::actualOne()(BREAK);
    return "";
}

std::string PipeContinue::str() const
{
    DataTree::actualOne()(CONTINUE);
    return "";
}

std::string ListAppend::str() const
{
    DataTree::actualOne()(BINARY_OP, "++");
    lhs->str();
    rhs->str();
    return "";
}

std::string Reference::str() const
{
    DataTree::actualOne()(REFERENCE, name);
    return "";
}

std::string SubReference::str() const
{
    DataTree::actualOne()(SUB_REFERENCE, name);
    return "";
}

std::string TransientParamReference::str() const
{
    DataTree::actualOne()(TRANSIENT_PARAMETER, name);
    return "";
}

std::string ImportedName::str() const
{
    DataTree::actualOne()(IMPORTED_NAME, name);
    return "";
}

std::string Call::str() const
{
    DataTree::actualOne()(CALL, args.size());
    callee->str();
    writeList(args);
    return "";
}

std::string SuperConstructorCall::str() const
{
    DataTree::actualOne()(SUPER_CONSTRUCTOR_CALL, args.size());
    writeList(args);
    return "";
}

std::string Lookup::str() const
{
    DataTree::actualOne()(BINARY_OP, "[]");
    collection->str();
    key->str();
    return "";
}

std::string ListSlice::str() const
{
    DataTree::actualOne()(LIST_SLICE);
    list->str();
    begin->str();
    end->str();
    step->str();
    return "";
}

std::string Dictionary::str() const
{
    DataTree::actualOne()(DICT_BEGIN);
    items.iter([&](util::ptrkv<Expression const> const& kv, int)
               {
                   DataTree::actualOne()(DICT_ITEM);
                   kv.key->str();
                   kv.value->str();
               });
    DataTree::actualOne()(DICT_END);
    return "";
}

std::string Assignment::str() const
{
    DataTree::actualOne()(BINARY_OP, "[=]");
    lhs->str();
    rhs->str();
    return "";
}

std::string BinaryOp::str() const
{
    DataTree::actualOne()(BINARY_OP, op);
    lhs->str();
    rhs->str();
    return "";
}

std::string PreUnaryOp::str() const
{
    DataTree::actualOne()(PRE_UNARY_OP, op);
    rhs->str();
    return "";
}

std::string Lambda::str() const
{
    DataTree::actualOne()(FUNCTION, param_names.size());
    std::for_each(param_names.begin()
                , param_names.end()
                , [&](std::string const& pn)
                  {
                      DataTree::actualOne()(PARAMETER, pn);
                  });
    if (mangle_as_param) {
        DataTree::actualOne()(MANGLE_AS_PARAM);
    }
    body->write(dummyos());
    return "";
}

std::string RegularAsyncLambda::str() const
{
    std::vector<std::string> p(param_names);
    p.insert(p.begin() + async_param_index, "# RegularAsyncParam");
    DataTree::actualOne()(FUNCTION, p.size());
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
    DataTree::actualOne()(THIS);
    return "";
}

std::string SuperFunc::str() const
{
    DataTree::actualOne()(SUPER_FUNC, property);
    return "";
}

std::string Conditional::str() const
{
    DataTree::actualOne()(CONDITIONAL);
    predicate->str();
    consequence->str();
    alternative->str();
    return "";
}

std::string ExceptionObj::str() const
{
    DataTree::actualOne()(EXCEPTION_OBJ);
    return "";
}

std::string ConditionalCallbackParameter::str() const
{
    DataTree::actualOne()(COND_CALLBACK_PARAM);
    return "";
}

std::string AsyncReference::str() const
{
    DataTree::actualOne()(ASYNC_REFERENCE);
    return "";
}

std::string RegularAsyncCallbackArg::str() const
{
    DataTree::actualOne()(FUNCTION);
    DataTree::actualOne()(PARAMETER, "# RegularAsyncCallbackParameters");
    thrower->scheme("");
    body->write(dummyos());
    return "";
}

std::string AsyncPipeline::str() const
{
    DataTree::actualOne()(ASYNC_PIPELINE);
    list->str();
    recursion->write(dummyos());
    succession->write(dummyos());
    thrower->scheme("");
    return "";
}

std::string SyncPipeline::str() const
{
    DataTree::actualOne()(SYNC_PIPELINE);
    list->str();
    section->write(dummyos());
    return "";
}

std::string RootSyncPipeline::str() const
{
    DataTree::actualOne()(ROOT_SYNC_PIPELINE);
    list->str();
    section->write(dummyos());
    return "";
}

struct MethodImpl
    : output::method::_Method
{
    explicit MethodImpl(NodeType t)
        : type(std::move(t))
    {}

    NodeType const type;

    std::string scheme(std::string const&) const
    {
        DataTree::actualOne()(type);
        return "";
    }

    bool mayThrow(util::sptr<Expression const> const&) const { return false; }
};

Method make_method(NodeType n)
{
    return util::mkptr(new MethodImpl(n));
}

Method method::place()
{
    return make_method(ARITHMETICS);
}

Method method::throwExc()
{
    return make_method(EXC_THROW);
}

Method method::callbackExc()
{
    return make_method(EXC_CALLBACK);
}

Method method::asyncCatcher(std::string)
{
    return make_method(ASYNC_CATCH_FUNC);
}

Method method::ret()
{
    return make_method(RETURN);
}

Method method::asyncRet()
{
    return make_method(REGULAR_ASYNC_RETURN);
}

Method method::syncPipeRet(util::uid)
{
    return make_method(SYNC_PIPELINE_RETURN);
}

Method method::syncBreak()
{
    return make_method(SYNC_BREAK);
}

Method method::callNext()
{
    return make_method(CALL_NEXT);
}

int Block::count() const { return 0; }
int Export::count() const { return 0; }
int ExceptionStallDeprecated::count() const { return 0; }
std::string output::formName(std::string const& name) { return name; }
std::string output::formSubName(std::string const& name, util::uid) { return name + '$'; }
std::string output::formModuleExportName(util::uid) { return "ME$"; }
bool ListLiteral::mayThrow() const { return false; }
bool Block::mayThrow() const { return false; }
