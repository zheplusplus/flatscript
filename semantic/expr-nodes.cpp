#include <algorithm>
#include <map>

#include <output/expr-nodes.h>
#include <output/stmt-nodes.h>
#include <util/string.h>
#include <report/errors.h>

#include "expr-nodes.h"
#include "function.h"
#include "compiling-space.h"
#include "filter.h"
#include "const-fold.h"

using namespace semantic;

static util::ptrarr<output::Expression const> compileList(
                        util::ptrarr<Expression const> const& list
                      , CompilingSpace& space)
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

util::sptr<output::Expression const> PreUnaryOp::compile(CompilingSpace& space) const
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

util::sptr<output::Expression const> BinaryOp::compile(CompilingSpace& space) const
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

util::sptr<output::Expression const> TypeOf::compile(CompilingSpace& space) const
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

util::sptr<output::Expression const> Reference::compile(CompilingSpace& space) const
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

util::sptr<output::Expression const> BoolLiteral::compile(CompilingSpace&) const
{
    return util::mkptr(new output::BoolLiteral(pos, value));
}

bool BoolLiteral::boolValue(util::sref<SymbolTable const>) const
{
    return value;
}

util::sptr<output::Expression const> IntLiteral::compile(CompilingSpace&) const
{
    return util::mkptr(new output::IntLiteral(pos, value));
}

mpz_class IntLiteral::intValue(util::sref<SymbolTable const>) const
{
    return value;
}

util::sptr<output::Expression const> FloatLiteral::compile(CompilingSpace&) const
{
    return util::mkptr(new output::FloatLiteral(pos, value));
}

mpf_class FloatLiteral::floatValue(util::sref<SymbolTable const>) const
{
    return value;
}

util::sptr<output::Expression const> StringLiteral::compile(CompilingSpace&) const
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

util::sptr<output::Expression const> ListLiteral::compile(CompilingSpace& space) const
{
    return util::mkptr(new output::ListLiteral(pos, compileList(value, space)));
}

bool ListLiteral::isAsync() const
{
    return isListAsync(value);
}

util::sptr<output::Expression const> PipeElement::compile(CompilingSpace&) const
{
    return util::mkptr(new output::PipeElement(pos));
}

util::sptr<output::Expression const> PipeIndex::compile(CompilingSpace&) const
{
    return util::mkptr(new output::PipeIndex(pos));
}

util::sptr<output::Expression const> PipeKey::compile(CompilingSpace&) const
{
    return util::mkptr(new output::PipeKey(pos));
}

util::sptr<output::Expression const> ListAppend::compile(CompilingSpace& space) const
{
    util::sptr<output::Expression const> clhs(lhs->compile(space));
    return util::mkptr(new output::ListAppend(pos, std::move(clhs), rhs->compile(space)));
}

bool ListAppend::isAsync() const
{
    return lhs->isAsync() || rhs->isAsync();
}

util::sptr<output::Expression const> Call::compile(CompilingSpace& space) const
{
    util::sptr<output::Expression const> ccallee(callee->compile(space));
    return util::mkptr(new output::Call(pos, std::move(ccallee), compileList(args, space)));
}

bool Call::isAsync() const
{
    return callee->isAsync() || isListAsync(args);
}

util::sptr<output::Expression const> MemberAccess::compile(CompilingSpace& space) const
{
    return util::mkptr(new output::MemberAccess(pos, referee->compile(space), member));
}

bool MemberAccess::isAsync() const
{
    return referee->isAsync();
}

util::sptr<output::Expression const> Lookup::compile(CompilingSpace& space) const
{
    util::sptr<output::Expression const> ccollection(collection->compile(space));
    return util::mkptr(new output::Lookup(pos, std::move(ccollection), key->compile(space)));
}

bool Lookup::isAsync() const
{
    return collection->isAsync() || key->isAsync();
}

util::sptr<output::Expression const> ListSlice::compile(CompilingSpace& space) const
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

util::sptr<output::Expression const> ListSlice::Default::compile(CompilingSpace&) const
{
    return util::mkptr(new output::ListSlice::Default(pos));
}

util::sptr<output::Expression const> Dictionary::compile(CompilingSpace& space) const
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

util::sptr<output::Expression const> Lambda::compile(CompilingSpace& space) const
{
    return util::mkptr(new output::Lambda(
                  pos, param_names, body->compile(CompilingSpace(pos, space.sym(), param_names))));
}

util::sptr<output::Expression const> AsyncCall::compile(CompilingSpace& space) const
{
    util::sptr<output::Expression const> compl_callee(callee->compile(space));
    util::ptrarr<output::Expression const> compl_fargs(compileList(former_args, space));
    util::ptrarr<output::Expression const> compl_largs(compileList(latter_args, space));

    util::sref<output::Block> current_flow(space.block());
    util::sptr<output::Block> async_flow(new output::Block);
    space.setAsyncSpace(pos, async_params, *async_flow);

    compl_fargs.append(util::mkptr(new output::Lambda(pos, async_params, std::move(async_flow))))
               .append(std::move(compl_largs));

    util::sptr<output::Expression const> compl_call(util::mkptr(
                        new output::Call(pos, std::move(compl_callee), std::move(compl_fargs))));
    util::id compl_call_id(compl_call.id());
    current_flow->addStmt(util::mkptr(new output::AsyncCallResultDef(std::move(compl_call))));
    return util::mkptr(new output::AsyncReference(pos, compl_call_id));
}
