#ifndef __STEKIN_OUTPUT_EXPRESSION_NODES_H__
#define __STEKIN_OUTPUT_EXPRESSION_NODES_H__

#include <string>
#include <gmpxx.h>

#include <util/arrays.h>

#include "node-base.h"
#include "fwd-decl.h"

namespace output {

    struct PropertyNameExpr
        : Expression
    {
        explicit PropertyNameExpr(misc::position const& pos)
            : Expression(pos)
        {}

        std::string strAsProp() const;
    };

    struct BoolLiteral
        : PropertyNameExpr
    {
        BoolLiteral(misc::position const& pos, bool v)
            : PropertyNameExpr(pos)
            , value(v)
        {}

        std::string str() const;

        bool const value;
    };

    struct IntLiteral
        : PropertyNameExpr
    {
        IntLiteral(misc::position const& pos, mpz_class const& v)
            : PropertyNameExpr(pos)
            , value(v)
        {}

        std::string str() const;

        mpz_class const value;
    };

    struct FloatLiteral
        : PropertyNameExpr
    {
        FloatLiteral(misc::position const& pos, mpf_class const& v)
            : PropertyNameExpr(pos)
            , value(v)
        {}

        std::string str() const;

        mpf_class const value;
    };

    struct StringLiteral
        : PropertyNameExpr
    {
        StringLiteral(misc::position const& pos, std::string const& v)
            : PropertyNameExpr(pos)
            , value(v)
        {}

        std::string str() const;

        std::string const value;
    };

    struct ListLiteral
        : Expression
    {
        ListLiteral(misc::position const& pos, util::ptrarr<Expression const> v)
            : Expression(pos)
            , value(std::move(v))
        {}

        std::string str() const;

        util::ptrarr<Expression const> const value;
    };

    struct PipeElement
        : Expression
    {
        explicit PipeElement(misc::position const& pos)
            : Expression(pos)
        {}

        std::string str() const;
    };

    struct PipeIndex
        : Expression
    {
        explicit PipeIndex(misc::position const& pos)
            : Expression(pos)
        {}

        std::string str() const;
    };

    struct PipeKey
        : Expression
    {
        explicit PipeKey(misc::position const& pos)
            : Expression(pos)
        {}

        std::string str() const;
    };

    struct Reference
        : PropertyNameExpr
    {
        Reference(misc::position const& pos, std::string const& n)
            : PropertyNameExpr(pos)
            , name(n)
        {}

        std::string str() const;

        std::string const name;
    };

    struct ImportedName
        : Expression
    {
        ImportedName(misc::position const& pos, std::string const& n)
            : Expression(pos)
            , name(n)
        {}

        std::string str() const;

        std::string const name;
    };

    struct Call
        : Expression
    {
        Call(misc::position const& pos
           , util::sptr<Expression const> c
           , util::ptrarr<Expression const> a)
                : Expression(pos)
                , callee(std::move(c))
                , args(std::move(a))
        {}

        std::string str() const;

        util::sptr<Expression const> const callee;
        util::ptrarr<Expression const> const args;
    };

    struct MemberAccess
        : Expression
    {
        MemberAccess(misc::position const& pos
                   , util::sptr<Expression const> ref
                   , std::string const& mem)
            : Expression(pos)
            , referee(std::move(ref))
            , member(mem)
        {}

        std::string str() const;

        util::sptr<Expression const> const referee;
        std::string const member;
    };

    struct Lookup
        : Expression
    {
        Lookup(misc::position const& pos
             , util::sptr<Expression const> c
             , util::sptr<Expression const> k)
                : Expression(pos)
                , collection(std::move(c))
                , key(std::move(k))
        {}

        std::string str() const;

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

            std::string str() const;
        };

        ListSlice(misc::position const& pos
                , util::sptr<Expression const> ls
                , util::sptr<Expression const> b
                , util::sptr<Expression const> e
                , util::sptr<Expression const> s)
            : Expression(pos)
            , list(std::move(ls))
            , begin(std::move(b))
            , end(std::move(e))
            , step(std::move(s))
        {}

        std::string str() const;

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

        std::string str() const;

        util::ptrkvarr<Expression const> const items;
    };

    struct ListAppend
        : Expression
    {
        ListAppend(misc::position const& pos
                 , util::sptr<Expression const> l
                 , util::sptr<Expression const> r)
            : Expression(pos)
            , lhs(std::move(l))
            , rhs(std::move(r))
        {}

        std::string str() const;

        util::sptr<Expression const> const lhs;
        util::sptr<Expression const> const rhs;
    };

    struct BinaryOp
        : Expression
    {
        BinaryOp(misc::position const& pos
                , util::sptr<Expression const> l
                , std::string const& o
                , util::sptr<Expression const> r)
            : Expression(pos)
            , lhs(std::move(l))
            , op(o)
            , rhs(std::move(r))
        {}

        std::string str() const;

        util::sptr<Expression const> const lhs;
        std::string const op;
        util::sptr<Expression const> const rhs;
    };

    struct PreUnaryOp
        : Expression
    {
        PreUnaryOp(misc::position const& pos, std::string const& o, util::sptr<Expression const> r)
            : Expression(pos)
            , op(o)
            , rhs(std::move(r))
        {}

        std::string str() const;

        std::string const op;
        util::sptr<Expression const> const rhs;
    };

    struct Lambda
        : Expression
    {
        Lambda(misc::position const& pos
             , std::vector<std::string> const& p
             , util::sptr<Statement const> b)
                : Expression(pos)
                , param_names(p)
                , body(std::move(b))
        {}

        std::string str() const;

        std::vector<std::string> const param_names;
        util::sptr<Statement const> const body;
    };

    struct AsyncReference
        : Expression
    {
        AsyncReference(misc::position const& pos, util::id const& id)
            : Expression(pos)
            , ref_id(id)
        {}

        std::string str() const;

        util::id const ref_id;
    };

}

#endif /* __STEKIN_OUTPUT_EXPRESSION_NODES_H__ */
