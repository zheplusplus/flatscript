#include <map>

#include <output/expr-nodes.h>
#include <output/stmt-nodes.h>
#include <output/function.h>
#include <util/string.h>
#include <report/errors.h>

#include "function.h"
#include "expr-nodes.h"
#include "compiling-space.h"
#include "const-fold.h"

using namespace semantic;

static util::ptrarr<output::Expression const> compileList(
                        util::ptrarr<Expression const> const& list
                      , BaseCompilingSpace& space)
{
    return list.map([&](util::sptr<Expression const> const& value, int)
                    {
                        return value->compile(space);
                    });
}

static bool isListAsync(util::ptrarr<Expression const> const& list)
{
    return list.any([&](util::sptr<Expression const> const& value, int)
                    {
                        return value->isAsync();
                    });
}

util::sptr<output::Expression const> PreUnaryOp::compile(BaseCompilingSpace& space) const
{
    if (isLiteral(space.sym())) {
        return compileLiteral(util::mkref(*this), space.sym());
    }
    return util::mkptr(new output::PreUnaryOp(pos, op_img, rhs->compile(space)));
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

util::sptr<output::Expression const> BinaryOp::compile(BaseCompilingSpace& space) const
{
    if (isLiteral(space.sym())) {
        return compileLiteral(util::mkref(*this), space.sym());
    }
    util::sptr<output::Expression const> clhs(lhs->compile(space));
    return util::mkptr(new output::BinaryOp(pos, std::move(clhs), op_img, rhs->compile(space)));
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

util::sptr<output::Expression const> TypeOf::compile(BaseCompilingSpace& space) const
{
    if (isLiteral(space.sym())) {
        return util::mkptr(new output::StringLiteral(pos, stringValue(space.sym())));
    }
    return util::mkptr(new output::PreUnaryOp(pos, "typeof ", expr->compile(space)));
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

util::sptr<output::Expression const> Reference::compile(BaseCompilingSpace& space) const
{
    return space.sym()->compileRef(pos, name);
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

util::sptr<output::Expression const> BoolLiteral::compile(BaseCompilingSpace&) const
{
    return util::mkptr(new output::BoolLiteral(pos, value));
}

bool BoolLiteral::boolValue(util::sref<SymbolTable const>) const
{
    return value;
}

util::sptr<output::Expression const> IntLiteral::compile(BaseCompilingSpace&) const
{
    return util::mkptr(new output::IntLiteral(pos, value));
}

mpz_class IntLiteral::intValue(util::sref<SymbolTable const>) const
{
    return value;
}

util::sptr<output::Expression const> FloatLiteral::compile(BaseCompilingSpace&) const
{
    return util::mkptr(new output::FloatLiteral(pos, value));
}

mpf_class FloatLiteral::floatValue(util::sref<SymbolTable const>) const
{
    return value;
}

util::sptr<output::Expression const> StringLiteral::compile(BaseCompilingSpace&) const
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

util::sptr<output::Expression const> ListLiteral::compile(BaseCompilingSpace& space) const
{
    return util::mkptr(new output::ListLiteral(pos, compileList(value, space)));
}

bool ListLiteral::isAsync() const
{
    return isListAsync(value);
}

util::sptr<output::Expression const> ListAppend::compile(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> clhs(lhs->compile(space));
    return util::mkptr(new output::ListAppend(pos, std::move(clhs), rhs->compile(space)));
}

bool ListAppend::isAsync() const
{
    return lhs->isAsync() || rhs->isAsync();
}

util::sptr<output::Expression const> Call::compile(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> ccallee(callee->compile(space));
    return util::mkptr(new output::Call(pos, std::move(ccallee), compileList(args, space)));
}

bool Call::isAsync() const
{
    return callee->isAsync() || isListAsync(args);
}

util::sptr<output::Expression const> MemberAccess::compile(BaseCompilingSpace& space) const
{
    return util::mkptr(new output::MemberAccess(pos, referee->compile(space), member));
}

bool MemberAccess::isAsync() const
{
    return referee->isAsync();
}

util::sptr<output::Expression const> Lookup::compile(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> ccollection(collection->compile(space));
    return util::mkptr(new output::Lookup(pos, std::move(ccollection), key->compile(space)));
}

bool Lookup::isAsync() const
{
    return collection->isAsync() || key->isAsync();
}

util::sptr<output::Expression const> ListSlice::compile(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> clist(list->compile(space));
    util::sptr<output::Expression const> cbegin(begin->compile(space));
    util::sptr<output::Expression const> cend(end->compile(space));
    return util::mkptr(new output::ListSlice(
                pos, std::move(clist), std::move(cbegin), std::move(cend), step->compile(space)));
}

bool ListSlice::isAsync() const
{
    return list->isAsync() || begin->isAsync() || end->isAsync() || step->isAsync();
}

util::sptr<output::Expression const> Undefined::compile(BaseCompilingSpace&) const
{
    return util::mkptr(new output::Undefined(pos));
}

util::sptr<output::Expression const> Dictionary::compile(BaseCompilingSpace& space) const
{
    return util::mkptr(new output::Dictionary(pos, items.map(
                    [&](util::ptrkv<Expression const> const& kv, int)
                    {
                        util::sptr<output::Expression const> ckey(kv.key->compile(space));
                        return util::mkkv(std::move(ckey), kv.value->compile(space));
                    })));
}

bool Dictionary::isAsync() const
{
    return items.any([&](util::ptrkv<Expression const> const& kv, int)
                     {
                         return kv.key->isAsync() || kv.value->isAsync();
                     });
}

util::sptr<output::Expression const> Lambda::compile(BaseCompilingSpace& space) const
{
    return util::mkptr(new output::Lambda(
            pos, param_names, body.compile(CompilingSpace(pos, space.sym(), param_names)), false));
}

util::sptr<output::Expression const> RegularAsyncLambda::compile(BaseCompilingSpace& space) const
{
    return util::mkptr(new output::RegularAsyncLambda(
            pos, param_names, async_param_index, body.compile(
                                    RegularAsyncCompilingSpace(pos, space.sym(), param_names))));
}

util::sptr<output::Expression const> RegularAsyncCall::compile(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> compl_callee(callee->compile(space));
    util::ptrarr<output::Expression const> compl_fargs(compileList(former_args, space));
    util::ptrarr<output::Expression const> compl_largs(compileList(latter_args, space));

    util::sref<output::Block> current_flow(space.block());
    util::sptr<output::Block> async_flow(new output::Block);
    space.setAsyncSpace(pos, std::vector<std::string>(), *async_flow);

    util::sptr<output::Expression const> callback(new output::RegularAsyncCallbackArg(
                                            pos, std::move(async_flow), space.raiseMethod()));
    util::id compl_call_id(callback.id());
    compl_fargs.append(std::move(callback)).append(std::move(compl_largs));

    current_flow->addStmt(util::mkptr(new output::Arithmetics(util::mkptr(
                        new output::Call(pos, std::move(compl_callee), std::move(compl_fargs))))));
    return util::mkptr(new output::AsyncReference(pos, compl_call_id));
}

util::sptr<output::Expression const> AsyncCall::compile(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> compl_callee(callee->compile(space));
    util::ptrarr<output::Expression const> compl_fargs(compileList(former_args, space));
    util::ptrarr<output::Expression const> compl_largs(compileList(latter_args, space));

    util::sref<output::Block> current_flow(space.block());
    util::sptr<output::Block> async_flow(new output::Block);
    space.setAsyncSpace(pos, async_params, *async_flow);

    compl_fargs.append(util::mkptr(new output::Lambda(
                  pos, async_params, std::move(async_flow), true))).append(std::move(compl_largs));

    util::sptr<output::Expression const> compl_call(util::mkptr(
                        new output::Call(pos, std::move(compl_callee), std::move(compl_fargs))));
    util::id compl_call_id(compl_call.id());
    current_flow->addStmt(util::mkptr(new output::AsyncCallResultDef(std::move(compl_call))));
    return util::mkptr(new output::AsyncReference(pos, compl_call_id));
}

util::sptr<output::Expression const> This::compile(BaseCompilingSpace& space) const
{
    space.referenceThis();
    return util::mkptr(new output::This(pos));
}

util::sptr<output::Expression const> Conditional::compile(BaseCompilingSpace& space) const
{
    if (predicate->isLiteral(space.sym())) {
        return _equivVal(space.sym())->compile(space);
    }
    if (consequence->isAsync() || alternative->isAsync()) {
        return _compileAsync(space);
    }
    return _compileSync(space);
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

util::sptr<output::Expression const> Conditional::_compileSync(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> compl_pred(predicate->compile(space));
    SubCompilingSpace consq_space(space);
    SubCompilingSpace alter_space(space);
    return util::mkptr(new output::Conditional(pos
                                             , std::move(compl_pred)
                                             , consequence->compile(consq_space)
                                             , alternative->compile(alter_space)));
}

static util::sptr<output::Block const> compileConditionalBranch(
            BaseCompilingSpace& space
          , util::sref<Expression const> expr
          , util::sref<output::ConditionalCallback const> cb)
{
    util::ptrarr<output::Expression const> args;
    SubCompilingSpace sub_space(space);
    args.append(expr->compile(sub_space));
    sub_space.block()->addStmt(util::mkptr(new output::Arithmetics(util::mkptr(
                        new output::FunctionInvocation(expr->pos, cb, std::move(args))))));
    return sub_space.deliver();
}

util::sptr<output::Expression const> Conditional::_compileAsync(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> compl_pred(predicate->compile(space));

    util::sptr<output::ConditionalCallback> cb(new output::ConditionalCallback);
    std::string param_name(cb->parameters()[0]);

    util::sptr<output::Block const> consq_flow(compileConditionalBranch(space, *consequence, *cb));
    util::sptr<output::Block const> alter_flow(compileConditionalBranch(space, *alternative, *cb));

    util::sref<output::Block> cb_body_flow(cb->bodyFlow());
    space.block()->addFunc(std::move(cb));
    space.block()->addStmt(util::mkptr(new output::Branch(
                        std::move(compl_pred), std::move(consq_flow), std::move(alter_flow))));
    space.setAsyncSpace(cb_body_flow);
    return util::mkptr(new output::Reference(pos, param_name));
}
