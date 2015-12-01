#include <map>

#include <output/expr-nodes.h>
#include <output/stmt-nodes.h>
#include <output/function.h>
#include <util/string.h>
#include <report/errors.h>

#include "function.h"
#include "expr-nodes.h"
#include "scope-impl.h"
#include "const-fold.h"
#include "common.h"

using namespace semantic;

static util::ptrarr<output::Expression const> compileList(
                        util::ptrarr<Expression const> const& list
                      , util::sref<Scope> scope)
{
    return list.map([&](util::sptr<Expression const> const& value, int)
                    {
                        return value->compile(scope);
                    });
}

static bool isListAsync(util::ptrarr<Expression const> const& list)
{
    return list.any([&](util::sptr<Expression const> const& value, int)
                    {
                        return value->isAsync();
                    });
}

util::sptr<output::Expression const> PreUnaryOp::compile(util::sref<Scope> scope) const
{
    if (isLiteral(scope->sym())) {
        return compileLiteral(util::mkref(*this), scope->sym());
    }
    return util::mkptr(new output::PreUnaryOp(pos, op_img, rhs->compile(scope)));
}

bool PreUnaryOp::isAsync() const
{
    return rhs->isAsync();
}

bool PreUnaryOp::isLiteral(util::sref<SymbolTable const> st) const
{
    return rhs->isLiteral(st);
}

std::string PreUnaryOp::literalType(util::sref<SymbolTable const> st) const
{
    return foldPreUnaryType(op_img, rhs->literalType(st));
}

bool PreUnaryOp::boolValue(util::sref<SymbolTable const> st) const
{
    return foldPreUnaryBoolValue(pos, op_img, rhs, st);
}

mpz_class PreUnaryOp::intValue(util::sref<SymbolTable const> st) const
{
    return foldPreUnaryIntValue(pos, op_img, rhs, st);
}

mpf_class PreUnaryOp::floatValue(util::sref<SymbolTable const> st) const
{
    return foldPreUnaryFloatValue(pos, op_img, rhs, st);
}

std::string PreUnaryOp::stringValue(util::sref<SymbolTable const> st) const
{
    return foldPreUnaryStringValue(pos, op_img, rhs, st);
}

util::sptr<output::Expression const> BinaryOp::compile(util::sref<Scope> scope) const
{
    if (isLiteral(scope->sym())) {
        return compileLiteral(util::mkref(*this), scope->sym());
    }
    util::sptr<output::Expression const> clhs(lhs->compile(scope));
    return util::mkptr(new output::BinaryOp(pos, std::move(clhs), op_img, rhs->compile(scope)));
}

bool BinaryOp::isAsync() const
{
    return lhs->isAsync() || rhs->isAsync();
}

bool BinaryOp::isLiteral(util::sref<SymbolTable const> st) const
{
    return lhs->isLiteral(st) && rhs->isLiteral(st);
}

std::string BinaryOp::literalType(util::sref<SymbolTable const> st) const
{
    return foldBinaryType(op_img, lhs->literalType(st), rhs->literalType(st));
}

bool BinaryOp::boolValue(util::sref<SymbolTable const> st) const
{
    return foldBinaryBoolValue(pos, op_img, lhs, rhs, st);
}

mpz_class BinaryOp::intValue(util::sref<SymbolTable const> st) const
{
    return foldBinaryIntValue(pos, op_img, lhs, rhs, st);
}

mpf_class BinaryOp::floatValue(util::sref<SymbolTable const> st) const
{
    return foldBinaryFloatValue(pos, op_img, lhs, rhs, st);
}

std::string BinaryOp::stringValue(util::sref<SymbolTable const> st) const
{
    return foldBinaryStringValue(pos, op_img, lhs, rhs, st);
}

util::sptr<output::Expression const> TypeOf::compile(util::sref<Scope> scope) const
{
    if (isLiteral(scope->sym())) {
        return util::mkptr(new output::StringLiteral(pos, stringValue(scope->sym())));
    }
    return util::mkptr(new output::PreUnaryOp(pos, "typeof ", expr->compile(scope)));
}

bool TypeOf::isLiteral(util::sref<SymbolTable const> st) const
{
    return expr->isLiteral(st);
}

std::string TypeOf::stringValue(util::sref<SymbolTable const> st) const
{
    static std::map<std::string, std::string> map{
        { "int", "number" },
        { "float", "number" },
        { "string", "string" },
        { "bool", "boolean" },
    };
    return map[expr->literalType(st)];
}

util::sptr<output::Expression const> Reference::compile(util::sref<Scope> scope) const
{
    return scope->sym()->compileRef(pos, name);
}

bool Reference::isLiteral(util::sref<SymbolTable const> st) const
{
    return st->literalOrNul(name).not_nul();
}

std::string Reference::literalType(util::sref<SymbolTable const> st) const
{
    return st->literalOrNul(name)->literalType(st);
}

bool Reference::boolValue(util::sref<SymbolTable const> st) const
{
    return st->literalOrNul(name)->boolValue(st);
}

mpz_class Reference::intValue(util::sref<SymbolTable const> st) const
{
    return st->literalOrNul(name)->intValue(st);
}

mpf_class Reference::floatValue(util::sref<SymbolTable const> st) const
{
    return st->literalOrNul(name)->floatValue(st);
}

std::string Reference::stringValue(util::sref<SymbolTable const> st) const
{
    return st->literalOrNul(name)->stringValue(st);
}

util::sptr<output::Expression const> BoolLiteral::compile(util::sref<Scope>) const
{
    return util::mkptr(new output::BoolLiteral(pos, value));
}

bool BoolLiteral::boolValue(util::sref<SymbolTable const>) const
{
    return value;
}

util::sptr<output::Expression const> IntLiteral::compile(util::sref<Scope>) const
{
    return util::mkptr(new output::IntLiteral(pos, value));
}

mpz_class IntLiteral::intValue(util::sref<SymbolTable const>) const
{
    return value;
}

util::sptr<output::Expression const> FloatLiteral::compile(util::sref<Scope>) const
{
    return util::mkptr(new output::FloatLiteral(pos, value));
}

mpf_class FloatLiteral::floatValue(util::sref<SymbolTable const>) const
{
    return value;
}

util::sptr<output::Expression const> StringLiteral::compile(util::sref<Scope>) const
{
    return util::mkptr(new output::StringLiteral(pos, value));
}

bool StringLiteral::boolValue(util::sref<SymbolTable const>) const
{
    return !value.empty();
}

std::string StringLiteral::stringValue(util::sref<SymbolTable const>) const
{
    return value;
}

util::sptr<output::Expression const> RegEx::compile(util::sref<Scope>) const
{
    return util::mkptr(new output::RegEx(this->pos, this->value));
}

util::sptr<output::Expression const> ListLiteral::compile(util::sref<Scope> scope) const
{
    return util::mkptr(new output::ListLiteral(pos, compileList(value, scope)));
}

bool ListLiteral::isAsync() const
{
    return isListAsync(value);
}

util::sptr<output::Expression const> ListAppend::compile(util::sref<Scope> scope) const
{
    util::sptr<output::Expression const> clhs(lhs->compile(scope));
    return util::mkptr(new output::ListAppend(pos, std::move(clhs), rhs->compile(scope)));
}

bool ListAppend::isAsync() const
{
    return lhs->isAsync() || rhs->isAsync();
}

util::sptr<output::Expression const> Call::compile(util::sref<Scope> scope) const
{
    util::sptr<output::Expression const> ccallee(callee->compile(scope));
    return util::mkptr(new output::Call(pos, std::move(ccallee), compileList(args, scope)));
}

bool Call::isAsync() const
{
    return callee->isAsync() || isListAsync(args);
}

util::sptr<output::Expression const> SuperConstructorCall::compile(util::sref<Scope> scope) const
{
    return util::mkptr(new output::SuperConstructorCall(
                this->pos, this->class_name, ::compileList(this->args, scope)));
}

bool SuperConstructorCall::isAsync() const
{
    return ::isListAsync(this->args);
}

util::sptr<output::Expression const> MemberAccess::compile(util::sref<Scope> scope) const
{
    return util::mkptr(new output::MemberAccess(pos, referee->compile(scope), member));
}

bool MemberAccess::isAsync() const
{
    return referee->isAsync();
}

util::sptr<output::Expression const> Lookup::compile(util::sref<Scope> scope) const
{
    util::sptr<output::Expression const> ccollection(collection->compile(scope));
    return util::mkptr(new output::Lookup(pos, std::move(ccollection), key->compile(scope)));
}

bool Lookup::isAsync() const
{
    return collection->isAsync() || key->isAsync();
}

util::sptr<output::Expression const> ListSlice::compile(util::sref<Scope> scope) const
{
    util::sptr<output::Expression const> clist(list->compile(scope));
    util::sptr<output::Expression const> cbegin(begin->compile(scope));
    util::sptr<output::Expression const> cend(end->compile(scope));
    return util::mkptr(new output::ListSlice(
                pos, std::move(clist), std::move(cbegin), std::move(cend), step->compile(scope)));
}

bool ListSlice::isAsync() const
{
    return list->isAsync() || begin->isAsync() || end->isAsync() || step->isAsync();
}

util::sptr<output::Expression const> Undefined::compile(util::sref<Scope>) const
{
    return util::mkptr(new output::Undefined(pos));
}

util::sptr<output::Expression const> Dictionary::compile(util::sref<Scope> scope) const
{
    return util::mkptr(new output::Dictionary(pos, items.map(
                    [&](util::ptrkv<Expression const> const& kv, int)
                    {
                        util::sptr<output::Expression const> ckey(kv.key->compile(scope));
                        return util::mkkv(std::move(ckey), kv.value->compile(scope));
                    })));
}

bool Dictionary::isAsync() const
{
    return items.any([&](util::ptrkv<Expression const> const& kv, int)
                     {
                         return kv.key->isAsync() || kv.value->isAsync();
                     });
}

util::sptr<output::Expression const> RegularAsyncCall::compile(util::sref<Scope> scope) const
{
    return util::mkptr(new output::AsyncReference(pos, this->_compile(scope, false)));
}

util::sptr<output::Expression const> RegularAsyncCall::compileAsRoot(util::sref<Scope> scope) const
{
    this->_compile(scope, true);
    return util::sptr<output::Expression const>(nullptr);
}

util::id RegularAsyncCall::_compile(util::sref<Scope> scope, bool) const
{
    util::sptr<output::Expression const> compl_callee(callee->compile(scope));
    util::ptrarr<output::Expression const> compl_fargs(compileList(former_args, scope));
    util::ptrarr<output::Expression const> compl_largs(compileList(latter_args, scope));

    util::sref<output::Block> current_flow(scope->block());
    util::sptr<output::Block> async_flow(new output::Block);
    scope->setAsyncSpace(pos, std::vector<std::string>(), *async_flow);

    util::sptr<output::Expression const> callback(new output::RegularAsyncCallbackArg(
                                            pos, std::move(async_flow), scope->throwMethod()));
    util::id compl_call_id(callback.id());
    compl_fargs.append(std::move(callback)).append(std::move(compl_largs));

    current_flow->addStmt(makeArith(util::mkptr(
                        new output::Call(pos, std::move(compl_callee), std::move(compl_fargs)))));
    return compl_call_id;
}

util::id AsyncCall::_compile(util::sref<Scope> scope, bool root) const
{
    util::sptr<output::Expression const> compl_callee(callee->compile(scope));
    util::ptrarr<output::Expression const> compl_fargs(compileList(former_args, scope));
    util::ptrarr<output::Expression const> compl_largs(compileList(latter_args, scope));

    util::sref<output::Block> current_flow(scope->block());
    util::sptr<output::Block> async_flow(new output::Block);
    scope->setAsyncSpace(pos, async_params, *async_flow);

    compl_fargs.append(util::mkptr(new output::Lambda(
                  pos, async_params, std::move(async_flow), true))).append(std::move(compl_largs));

    util::sptr<output::Expression const> compl_call(util::mkptr(
                        new output::Call(pos, std::move(compl_callee), std::move(compl_fargs))));
    util::id compl_call_id(compl_call.id());
    current_flow->addStmt(util::mkptr(new output::AsyncCallResultDef(std::move(compl_call), !root)));
    return compl_call_id;
}

util::sptr<output::Expression const> This::compile(util::sref<Scope> scope) const
{
    scope->referenceThis(this->pos);
    return util::mkptr(new output::This(pos));
}

util::sptr<output::Expression const> SuperFunc::compile(util::sref<Scope> scope) const
{
    if (!scope->allowSuper()) {
        error::superNotInMember(pos);
    }
    return util::mkptr(new output::SuperFunc(pos, property));
}

util::sptr<output::Expression const> Conditional::compile(util::sref<Scope> scope) const
{
    if (predicate->isLiteral(scope->sym())) {
        return _equivVal(scope->sym())->compile(scope);
    }
    if (consequence->isAsync() || alternative->isAsync()) {
        return _compileAsync(scope);
    }
    return _compileSync(scope);
}

bool Conditional::isLiteral(util::sref<SymbolTable const> sym) const
{
    return predicate->isLiteral(sym) && _equivVal(sym)->isLiteral(sym);
}

std::string Conditional::literalType(util::sref<SymbolTable const> sym) const
{
    return _equivVal(sym)->literalType(sym);
}

bool Conditional::boolValue(util::sref<SymbolTable const> sym) const
{
    return _equivVal(sym)->boolValue(sym);
}

mpz_class Conditional::intValue(util::sref<SymbolTable const> sym) const
{
    return _equivVal(sym)->intValue(sym);
}

mpf_class Conditional::floatValue(util::sref<SymbolTable const> sym) const
{
    return _equivVal(sym)->floatValue(sym);
}

std::string Conditional::stringValue(util::sref<SymbolTable const> sym) const
{
    return _equivVal(sym)->stringValue(sym);
}

bool Conditional::isAsync() const
{
    return predicate->isAsync() || consequence->isAsync() || alternative->isAsync();
}

util::sref<Expression const> Conditional::_equivVal(util::sref<SymbolTable const> sym) const
{
    if (predicate->isLiteral(sym)) {
        return *(predicate->boolValue(sym) ? consequence : alternative);
    }
    return util::mkref(*this);
}

util::sptr<output::Expression const> Conditional::_compileSync(util::sref<Scope> scope) const
{
    util::sptr<output::Expression const> compl_pred(predicate->compile(scope));
    return util::mkptr(new output::Conditional(
          pos, std::move(compl_pred), consequence->compile(scope), alternative->compile(scope)));
}

static util::sptr<output::Block const> compileConditionalBranch(
            util::sref<Scope> scope
          , util::sref<Expression const> expr
          , util::sref<output::ConditionalCallback const> cb)
{
    util::ptrarr<output::Expression const> args;
    BranchingSubScope sub_scope(scope);
    args.append(expr->compile(util::mkref(sub_scope)));
    sub_scope.addStmt(expr->pos, makeArith(cb->callMe(expr->pos, std::move(args))));
    return sub_scope.deliver();
}

util::sptr<output::Expression const> Conditional::_compileAsync(util::sref<Scope> scope) const
{
    util::sptr<output::Expression const> compl_pred(predicate->compile(scope));

    util::sptr<output::ConditionalCallback> cb(new output::ConditionalCallback);
    util::sptr<output::Block const> consq_flow(compileConditionalBranch(scope, *consequence, *cb));
    util::sptr<output::Block const> alter_flow(compileConditionalBranch(scope, *alternative, *cb));

    util::sref<output::Block> cb_body_flow(cb->bodyFlow());
    scope->block()->addFunc(std::move(cb));
    scope->addStmt(pos, util::mkptr(new output::Branch(
                        std::move(compl_pred), std::move(consq_flow), std::move(alter_flow))));
    scope->setAsyncSpace(pos, std::vector<std::string>(), cb_body_flow);
    return util::mkptr(new output::ConditionalCallbackParameter(pos));
}

util::sptr<output::Expression const> ExceptionObj::compile(util::sref<Scope> scope) const
{
    if (!scope->inCatch()) {
        error::exceptionNotInCatchContext(pos);
    }
    return util::mkptr(new output::ExceptionObj(pos));
}
