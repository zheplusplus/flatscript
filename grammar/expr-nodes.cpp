#include <semantic/expr-nodes.h>
#include <semantic/list-pipe.h>
#include <semantic/function.h>
#include <semantic/filter.h>
#include <util/string.h>
#include <report/errors.h>

#include "expr-nodes.h"
#include "function.h"

using namespace grammar;

bool EmptyExpr::empty() const
{
    return true;
}

util::sptr<semantic::Expression const> EmptyExpr::reduceAsExpr(BaseReducingEnv const&) const
{
    return util::mkptr(new semantic::ListSlice::Default(pos));
}

util::sptr<semantic::Expression const> PreUnaryOp::reduceAsExpr(BaseReducingEnv const& env) const
{
    if ("typeof" == op_img) {
        return util::mkptr(new semantic::TypeOf(pos, rhs->reduceAsExpr(env)));
    }
    return util::mkptr(new semantic::PreUnaryOp(pos, op_img, rhs->reduceAsExpr(env)));
}

util::sptr<semantic::Expression const> BinaryOp::reduceAsExpr(BaseReducingEnv const& env) const
{
    if ("." == op_img) {
        return util::mkptr(new semantic::MemberAccess(
                            pos, lhs->reduceAsExpr(env), rhs->reduceAsName()));
    }
    if ("++" == op_img) {
        return util::mkptr(new semantic::ListAppend(
                            pos, lhs->reduceAsExpr(env), rhs->reduceAsExpr(env)));
    }
    return util::mkptr(new semantic::BinaryOp(
                            pos, lhs->reduceAsExpr(env), op_img, rhs->reduceAsExpr(env)));
}

util::sptr<semantic::Expression const> BinaryOp::reduceAsLeftValue(BaseReducingEnv const& env) const
{
    if ("." != op_img) {
        return Expression::reduceAsLeftValue(env);
    }
    return util::mkptr(new semantic::MemberAccess(
                                pos, lhs->reduceAsExpr(env), rhs->reduceAsName()));
}

bool Identifier::isName() const
{
    return true;
}

std::string Identifier::reduceAsName() const
{
    return name;
}

util::sptr<semantic::Expression const> Identifier::reduceAsExpr(BaseReducingEnv const&) const
{
    return util::mkptr(new semantic::Reference(pos, name));
}

std::string BoolLiteral::reduceAsProperty() const
{
    return util::str(value);
}

util::sptr<semantic::Expression const> BoolLiteral::reduceAsExpr(BaseReducingEnv const&) const
{
    return util::mkptr(new semantic::BoolLiteral(pos, value));
}

std::string IntLiteral::reduceAsProperty() const
{
    return util::str(value);
}

util::sptr<semantic::Expression const> IntLiteral::reduceAsExpr(BaseReducingEnv const&) const
{
    return util::mkptr(new semantic::IntLiteral(pos, value));
}

std::string FloatLiteral::reduceAsProperty() const
{
    return util::str(value);
}

util::sptr<semantic::Expression const> FloatLiteral::reduceAsExpr(BaseReducingEnv const&) const
{
    return util::mkptr(new semantic::FloatLiteral(pos, value));
}

std::string StringLiteral::reduceAsProperty() const
{
    return value;
}

util::sptr<semantic::Expression const> StringLiteral::reduceAsExpr(BaseReducingEnv const&) const
{
    return util::mkptr(new semantic::StringLiteral(pos, value));
}

util::sptr<semantic::Expression const> ListLiteral::reduceAsExpr(BaseReducingEnv const& env) const
{
    return util::mkptr(new semantic::ListLiteral(
                pos, value.map([&](util::sptr<Expression const> const& e, int) {
                         return e->reduceAsExpr(env);
                     })));
}

util::sptr<semantic::Expression const> PipeElement::reduceAsExpr(BaseReducingEnv const& env) const
{
    if (!env.inPipe()) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new semantic::PipeElement(pos));
}

util::sptr<semantic::Expression const> PipeIndex::reduceAsExpr(BaseReducingEnv const& env) const
{
    if (!env.inPipe()) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new semantic::PipeIndex(pos));
}

util::sptr<semantic::Expression const> PipeKey::reduceAsExpr(BaseReducingEnv const& env) const
{
    if (!env.inPipe()) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new semantic::PipeKey(pos));
}

util::sptr<semantic::Expression const> Call::reduceAsExpr(BaseReducingEnv const& env) const
{
    ArgReducingEnv args_env(env);
    util::ptrarr<semantic::Expression const> reduced_args(
            args.map([&](util::sptr<Expression const> const& e, int index) {
                return e->reduceAsArg(args_env, index);
            }));
    if (args_env.isAsync()) {
        return util::mkptr(new semantic::AsyncCall(
                    pos
                  , callee->reduceAsExpr(env)
                  , reduced_args.deliverRange(0, args_env.asyncIndex())
                  , args_env.asyncParams()
                  , reduced_args.deliverRange(args_env.asyncIndex() + 1, reduced_args.size())));
    }
    return util::mkptr(new semantic::Call(pos, callee->reduceAsExpr(env), std::move(reduced_args)));
}

util::sptr<semantic::Expression const> Lookup::reduceAsExpr(BaseReducingEnv const& env) const
{
    return util::mkptr(
          new semantic::Lookup(pos, collection->reduceAsExpr(env), key->reduceAsExpr(env)));
}

util::sptr<semantic::Expression const> Lookup::reduceAsLeftValue(BaseReducingEnv const& env) const
{
    return reduceAsExpr(env);
}

util::sptr<semantic::Expression const> ListSlice::Default::reduceAsExpr(BaseReducingEnv const&) const
{
    return util::mkptr(new semantic::ListSlice::Default(pos));
}

util::sptr<semantic::Expression const> ListSlice::reduceAsExpr(BaseReducingEnv const& env) const
{
    return util::mkptr(new semantic::ListSlice(pos
                                             , list->reduceAsExpr(env)
                                             , begin->reduceAsExpr(env)
                                             , end->reduceAsExpr(env)
                                             , step->reduceAsExpr(env)));
}

util::sptr<semantic::Expression const> Dictionary::reduceAsExpr(BaseReducingEnv const& env) const
{
    return util::mkptr(new semantic::Dictionary(pos, items.map(
              [&](util::ptrkv<Expression const> const& kv, int)
              {
                  return util::mkkv(kv.key->reduceAsExpr(env), kv.value->reduceAsExpr(env));
              })));
}

util::sptr<semantic::Expression const> Lambda::reduceAsExpr(BaseReducingEnv const& env) const
{
    return util::mkptr(new semantic::Lambda(pos, param_names, body.compile(env)->deliver()));
}

util::sptr<semantic::Expression const> AsyncPlaceholder::reduceAsExpr(BaseReducingEnv const&) const
{
    error::asyncPlaceholderNotArgument(pos);
    return util::sptr<semantic::Expression const>(nullptr);
}

util::sptr<semantic::Expression const> AsyncPlaceholder::reduceAsArg(
                                                ArgReducingEnv& env, int index) const
{
    env.setAsync(pos, index, param_names);
    return util::sptr<semantic::Expression const>(nullptr);
}

util::sptr<semantic::Expression const> This::reduceAsExpr(BaseReducingEnv const&) const
{
    return util::mkptr(new semantic::This(pos));
}

util::sptr<semantic::Expression const> Pipeline::reduceAsExpr(BaseReducingEnv const& env) const
{
    if ("|:" == op_img) {
        return semantic::Pipeline::createMapper(
                        pos, lhs->reduceAsExpr(env), rhs->reduceAsExpr(PipeReducingEnv()));
    }
    return semantic::Pipeline::createFilter(
                    pos, lhs->reduceAsExpr(env), rhs->reduceAsExpr(PipeReducingEnv()));
}

util::sptr<semantic::Expression const> BlockPipeline::reduceAsExpr(BaseReducingEnv const& env) const
{
    return util::mkptr(new semantic::Pipeline(
                    pos, list->reduceAsExpr(env), section.compile(PipeReducingEnv())->deliver()));
}
