#ifndef __STEKIN_GRAMMAR_EXPRESSION_NODES_H__
#define __STEKIN_GRAMMAR_EXPRESSION_NODES_H__

#include <utility>
#include <gmpxx.h>

#include <util/arrays.h>

#include "node-base.h"
#include "block.h"

namespace grammar {

    struct EmptyExpr
        : Expression
    {
        explicit EmptyExpr(misc::position const& pos)
            : Expression(pos)
        {}

        bool empty() const;
        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const&) const;
    };

    struct PreUnaryOp
        : Expression
    {
        PreUnaryOp(misc::position const& pos, std::string const& op, util::sptr<Expression const> r)
            : Expression(pos)
            , op_img(op)
            , rhs(std::move(r))
        {}

        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const& env) const;

        std::string const op_img;
        util::sptr<Expression const> const rhs;
    };

    struct BinaryOp
        : Expression
    {
        BinaryOp(misc::position const& pos
               , util::sptr<Expression const> l
               , std::string const& op
               , util::sptr<Expression const> r)
            : Expression(pos)
            , lhs(std::move(l))
            , op_img(op)
            , rhs(std::move(r))
        {}

        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const& env) const;
        util::sptr<semantic::Expression const> reduceAsLeftValue(BaseReducingEnv const& env) const;

        util::sptr<Expression const> const lhs;
        std::string const op_img;
        util::sptr<Expression const> const rhs;
    };

    struct Identifier
        : Expression
    {
        Identifier(misc::position const& pos, std::string const& n)
            : Expression(pos)
            , name(n)
        {}

        bool isName() const;
        std::string reduceAsName() const;
        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const&) const;

        std::string const name;
    };

    struct BoolLiteral
        : Expression
    {
        BoolLiteral(misc::position const& pos, bool v)
            : Expression(pos)
            , value(v)
        {}

        std::string reduceAsProperty() const;
        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const&) const;

        bool const value;
    };

    struct IntLiteral
        : Expression
    {
        IntLiteral(misc::position const& pos, std::string const& v)
            : Expression(pos)
            , value(v)
        {}

        std::string reduceAsProperty() const;
        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const&) const;

        mpz_class const value;
    };

    struct FloatLiteral
        : Expression
    {
        FloatLiteral(misc::position const& pos, std::string const& v)
            : Expression(pos)
            , value(v)
        {}

        std::string reduceAsProperty() const;
        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const&) const;

        mpf_class const value;
    };

    struct StringLiteral
        : Expression
    {
        StringLiteral(misc::position const& pos, std::string const& v)
            : Expression(pos)
            , value(v)
        {}

        std::string reduceAsProperty() const;
        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const&) const;

        std::string const value;
    };

    struct ListLiteral
        : Expression
    {
        ListLiteral(misc::position const& pos, util::ptrarr<Expression const> v)
            : Expression(pos)
            , value(std::move(v))
        {}

        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const& env) const;

        util::ptrarr<Expression const> const value;
    };

    struct PipeElement
        : Expression
    {
        explicit PipeElement(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const& env) const;
    };

    struct PipeIndex
        : Expression
    {
        explicit PipeIndex(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const& env) const;
    };

    struct PipeKey
        : Expression
    {
        explicit PipeKey(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const& env) const;
    };

    struct Call
        : Expression
    {
        Call(util::sptr<Expression const> c, util::ptrarr<Expression const> a)
            : Expression(c->pos)
            , callee(std::move(c))
            , args(std::move(a))
        {}

        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const& env) const;

        util::sptr<Expression const> const callee;
        util::ptrarr<Expression const> const args;
    };

    struct Lookup
        : Expression
    {
        Lookup(util::sptr<Expression const> c, util::sptr<Expression const> k)
            : Expression(c->pos)
            , collection(std::move(c))
            , key(std::move(k))
        {}

        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const& env) const;
        util::sptr<semantic::Expression const> reduceAsLeftValue(BaseReducingEnv const& env) const;

        util::sptr<Expression const> const collection;
        util::sptr<Expression const> const key;
    };

    struct ListSlice
        : Expression
    {
        struct Default
            : Expression
        {
            explicit Default(misc::position const& pos)
                : Expression(pos)
            {}

            util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const&) const;

            static util::sptr<Expression const> create(misc::position const& pos)
            {
                return util::mkptr(new Default(pos));
            }
        };

        ListSlice(util::sptr<Expression const> ls
                , util::sptr<Expression const> b
                , util::sptr<Expression const> e
                , util::sptr<Expression const> s)
            : Expression(ls->pos)
            , list(std::move(ls))
            , begin(b->empty() ? Default::create(pos) : std::move(b))
            , end(e->empty() ? Default::create(pos) : std::move(e))
            , step(s->empty() ? Default::create(pos) : std::move(s))
        {}

        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const& env) const;

        util::sptr<Expression const> const list;
        util::sptr<Expression const> const begin;
        util::sptr<Expression const> const end;
        util::sptr<Expression const> const step;
    };

    struct Dictionary
        : Expression
    {
        Dictionary(misc::position const& pos, util::ptrkvarr<Expression const> i)
            : Expression(pos)
            , items(std::move(i))
        {}

        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const& env) const;

        util::ptrkvarr<Expression const> const items;
    };

    struct Lambda
        : Expression
    {
        Lambda(misc::position const& pos, std::vector<std::string> const& p, Block b)
            : Expression(pos)
            , param_names(p)
            , body(std::move(b))
        {}

        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const& env) const;

        std::vector<std::string> const param_names;
        Block const body;
    };

    struct AsyncPlaceholder
        : Expression
    {
        AsyncPlaceholder(misc::position const& pos, std::vector<std::string> const& p)
            : Expression(pos)
            , param_names(p)
        {}

        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const&) const;
        util::sptr<semantic::Expression const> reduceAsArg(ArgReducingEnv& env, int index) const;

        std::vector<std::string> const param_names;
    };

    struct This
        : Expression
    {
        explicit This(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const&) const;
    };

    struct Pipeline
        : BinaryOp
    {
        Pipeline(misc::position const& pos
               , util::sptr<Expression const> lhs
               , std::string const& op
               , util::sptr<Expression const> rhs)
            : BinaryOp(pos, std::move(lhs), op, std::move(rhs))
        {}

        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const& env) const;
    };

    struct BlockPipeline
        : Expression
    {
        BlockPipeline(misc::position const& pos, util::sptr<Expression const> ls, Block sec)
            : Expression(pos)
            , list(std::move(ls))
            , section(std::move(sec))
        {}

        util::sptr<semantic::Expression const> reduceAsExpr(BaseReducingEnv const& env) const;

        util::sptr<Expression const> const list;
        Block const section;
    };

}

#endif /* __STEKIN_GRAMMAR_EXPRESSION_NODES_H__ */
