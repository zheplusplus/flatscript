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

util::sptr<semantic::Expression const> EmptyExpr::reduceAsExpr() const
{
    return util::mkptr(new semantic::Undefined(pos));
}

util::sptr<semantic::Expression const> PreUnaryOp::reduceAsExpr() const
{
    if ("typeof" == op_img) {
        return util::mkptr(new semantic::TypeOf(pos, rhs->reduceAsExpr()));
    }
    return util::mkptr(new semantic::PreUnaryOp(pos, op_img, rhs->reduceAsExpr()));
}

util::sptr<semantic::Expression const> BinaryOp::reduceAsExpr() const
{
    if ("." == op_img) {
        return util::mkptr(new semantic::MemberAccess(
                            pos, lhs->reduceAsExpr(), rhs->reduceAsName()));
    }
    if ("++" == op_img) {
        return util::mkptr(new semantic::ListAppend(
                            pos, lhs->reduceAsExpr(), rhs->reduceAsExpr()));
    }
    if ("||" == op_img) {
        return util::mkptr(new semantic::Conditional(
                          pos
                        , lhs->reduceAsExpr()
                        , util::mkptr(new semantic::BoolLiteral(pos, true))
                        , rhs->reduceAsExpr()));
    }
    if ("&&" == op_img) {
        return util::mkptr(new semantic::Conditional(
                          pos
                        , lhs->reduceAsExpr()
                        , rhs->reduceAsExpr()
                        , util::mkptr(new semantic::BoolLiteral(pos, false))));
    }
    return util::mkptr(new semantic::BinaryOp(
                            pos, lhs->reduceAsExpr(), op_img, rhs->reduceAsExpr()));
}

util::sptr<semantic::Expression const> BinaryOp::reduceAsLeftValue() const
{
    if ("." != op_img) {
        return Expression::reduceAsLeftValue();
    }
    return util::mkptr(new semantic::MemberAccess(
                                pos, lhs->reduceAsExpr(), rhs->reduceAsName()));
}

bool Identifier::isName() const
{
    return true;
}

std::string Identifier::reduceAsName() const
{
    return name;
}

util::sptr<semantic::Expression const> Identifier::reduceAsExpr() const
{
    return util::mkptr(new semantic::Reference(pos, name));
}

std::string BoolLiteral::reduceAsProperty() const
{
    return util::str(value);
}

util::sptr<semantic::Expression const> BoolLiteral::reduceAsExpr() const
{
    return util::mkptr(new semantic::BoolLiteral(pos, value));
}

std::string IntLiteral::reduceAsProperty() const
{
    return util::str(value);
}

util::sptr<semantic::Expression const> IntLiteral::reduceAsExpr() const
{
    return util::mkptr(new semantic::IntLiteral(pos, value));
}

std::string FloatLiteral::reduceAsProperty() const
{
    return util::str(value);
}

util::sptr<semantic::Expression const> FloatLiteral::reduceAsExpr() const
{
    return util::mkptr(new semantic::FloatLiteral(pos, value));
}

std::string StringLiteral::reduceAsProperty() const
{
    return value;
}

util::sptr<semantic::Expression const> StringLiteral::reduceAsExpr() const
{
    return util::mkptr(new semantic::StringLiteral(pos, value));
}

util::sptr<semantic::Expression const> ListLiteral::reduceAsExpr() const
{
    return util::mkptr(new semantic::ListLiteral(
                pos, value.map([&](util::sptr<Expression const> const& e, int)
                               {
                                   return e->reduceAsExpr();
                               })));
}

util::sptr<semantic::Expression const> PipeElement::reduceAsExpr() const
{
    return util::mkptr(new semantic::PipeElement(pos));
}

util::sptr<semantic::Expression const> PipeIndex::reduceAsExpr() const
{
    return util::mkptr(new semantic::PipeIndex(pos));
}

util::sptr<semantic::Expression const> PipeKey::reduceAsExpr() const
{
    return util::mkptr(new semantic::PipeKey(pos));
}

util::sptr<semantic::Expression const> PipeResult::reduceAsExpr() const
{
    return util::mkptr(new semantic::PipeResult(pos));
}

util::sptr<semantic::Expression const> Call::reduceAsExpr() const
{
    ArgReducingEnv args_env;
    util::ptrarr<semantic::Expression const> reduced_args(
            args.map([&](util::sptr<Expression const> const& e, int index)
                     {
                         return e->reduceAsArg(args_env, index);
                     }));
    if (!args_env.isAsync()) {
        return util::mkptr(new semantic::Call(
                            pos, callee->reduceAsExpr(), std::move(reduced_args)));
    }
    if (args_env.isRegularAsync()) {
        return util::mkptr(new semantic::RegularAsyncCall(
                    pos
                  , callee->reduceAsExpr()
                  , reduced_args.deliverRange(0, args_env.asyncIndex())
                  , reduced_args.deliverRange(args_env.asyncIndex() + 1, reduced_args.size())));
    }
    return util::mkptr(new semantic::AsyncCall(
                pos
              , callee->reduceAsExpr()
              , reduced_args.deliverRange(0, args_env.asyncIndex())
              , args_env.asyncParams()
              , reduced_args.deliverRange(args_env.asyncIndex() + 1, reduced_args.size())));
}

util::sptr<semantic::Expression const> Lookup::reduceAsExpr() const
{
    return util::mkptr(new semantic::Lookup(pos, collection->reduceAsExpr(), key->reduceAsExpr()));
}

util::sptr<semantic::Expression const> Lookup::reduceAsLeftValue() const
{
    return reduceAsExpr();
}

util::sptr<semantic::Expression const> ListSlice::reduceAsExpr() const
{
    return util::mkptr(new semantic::ListSlice(pos
                                             , list->reduceAsExpr()
                                             , begin->reduceAsExpr()
                                             , end->reduceAsExpr()
                                             , step->reduceAsExpr()));
}

util::sptr<semantic::Expression const> Dictionary::reduceAsExpr() const
{
    return util::mkptr(new semantic::Dictionary(pos, items.map(
              [&](util::ptrkv<Expression const> const& kv, int)
              {
                  return util::mkkv(kv.key->reduceAsExpr(), kv.value->reduceAsExpr());
              })));
}

util::sptr<semantic::Expression const> Lambda::reduceAsExpr() const
{
    return util::mkptr(new semantic::Lambda(pos, param_names, body.compile()->deliver()));
}

util::sptr<semantic::Expression const> RegularAsyncLambda::reduceAsExpr() const
{
    return util::mkptr(new semantic::RegularAsyncLambda(
                            pos, param_names, async_param_index, body.compile()->deliver()));
}

util::sptr<semantic::Expression const> AsyncPlaceholder::reduceAsExpr() const
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

util::sptr<semantic::Expression const> This::reduceAsExpr() const
{
    return util::mkptr(new semantic::This(pos));
}

util::sptr<semantic::Expression const> Pipeline::reduceAsExpr() const
{
    if ("|:" == op_img) {
        return semantic::Pipeline::createMapper(pos, lhs->reduceAsExpr(), rhs->reduceAsExpr());
    }
    return semantic::Pipeline::createFilter(pos, lhs->reduceAsExpr(), rhs->reduceAsExpr());
}

util::sptr<semantic::Expression const> BlockPipeline::reduceAsExpr() const
{
    return util::mkptr(new semantic::Pipeline(
                    pos, list->reduceAsExpr(), section.compile()->deliver()));
}

util::sptr<semantic::Expression const> Conditional::reduceAsExpr() const
{
    return util::mkptr(new semantic::Conditional(pos
                                               , predicate->reduceAsExpr()
                                               , consequence->reduceAsExpr()
                                               , alternative->reduceAsExpr()));
}

util::sptr<semantic::Expression const> RegularAsyncParam::reduceAsExpr() const
{
    error::asyncParamNotExpr(pos);
    return util::mkptr(new semantic::Undefined(pos));
}

void RegularAsyncParam::reduceAsParam(ParamReducingEnv& env, int index) const
{
    env.setAsync(pos, index);
}

util::sptr<semantic::Expression const> RegularAsyncParam::reduceAsArg(
                                        ArgReducingEnv& env, int index) const
{
    env.setRegularAsync(pos, index);
    return util::sptr<semantic::Expression const>(nullptr);
}
