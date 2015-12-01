#ifndef __STEKIN_OUTPUT_EXPRESSION_NODES_H__
#define __STEKIN_OUTPUT_EXPRESSION_NODES_H__

#include <string>
#include <gmpxx.h>

#include <util/arrays.h>

#include "node-base.h"
#include "methods.h"

namespace output {

    struct PrimeFactor
        : Expression
    {
        explicit PrimeFactor(misc::position const& pos)
            : Expression(pos)
        {}

        bool mayThrow() const { return false; }
    };

    struct Undefined
        : PrimeFactor
    {
        explicit Undefined(misc::position const& pos)
            : PrimeFactor(pos)
        {}

        std::string str() const;
    };

    struct BoolLiteral
        : PrimeFactor
    {
        BoolLiteral(misc::position const& pos, bool v)
            : PrimeFactor(pos)
            , value(v)
        {}

        std::string str() const;

        bool const value;
    };

    struct IntLiteral
        : PrimeFactor
    {
        IntLiteral(misc::position const& pos, mpz_class const& v)
            : PrimeFactor(pos)
            , value(v)
        {}

        std::string str() const;

        mpz_class const value;
    };

    struct FloatLiteral
        : PrimeFactor
    {
        FloatLiteral(misc::position const& pos, mpf_class const& v)
            : PrimeFactor(pos)
            , value(v)
        {}

        std::string str() const;

        mpf_class const value;
    };

    struct StringLiteral
        : PrimeFactor
    {
        StringLiteral(misc::position const& pos, std::string const& v)
            : PrimeFactor(pos)
            , value(v)
        {}

        std::string str() const;

        std::string const value;
    };

    struct RegEx
        : PrimeFactor
    {
        RegEx(misc::position const& pos, std::string v)
            : PrimeFactor(pos)
            , value(std::move(v))
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
        bool mayThrow() const;

        util::ptrarr<Expression const> const value;
    };

    struct Reference
        : PrimeFactor
    {
        Reference(misc::position const& pos, std::string const& n)
            : PrimeFactor(pos)
            , name(n)
        {}

        std::string str() const;

        std::string const name;
    };

    struct SubReference
        : Reference
    {
        SubReference(misc::position const& pos, std::string const& name, util::id sid)
            : Reference(pos, name)
            , space_id(sid)
        {}

        std::string str() const;

        util::id const space_id;
    };

    struct TransientParamReference
        : Reference
    {
        TransientParamReference(misc::position const& pos, std::string const& name)
            : Reference(pos, name)
        {}

        std::string str() const;
    };

    struct ImportedName
        : Expression
    {
        ImportedName(misc::position const& pos, std::string const& n)
            : Expression(pos)
            , name(n)
        {}

        std::string str() const;
        bool mayThrow() const { return true; }

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
        bool mayThrow() const { return true; }

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
        bool mayThrow() const { return true; }

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
        bool mayThrow() const { return true; }

        util::sptr<Expression const> const collection;
        util::sptr<Expression const> const key;
    };

    struct ListSlice
        : Expression
    {
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
        bool mayThrow() const { return true; }

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
        bool mayThrow() const { return true; }

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
        bool mayThrow() const { return true; }

        util::sptr<Expression const> const lhs;
        util::sptr<Expression const> const rhs;
    };

    struct Assignment
        : Expression
    {
        Assignment(misc::position const& pos
                 , util::sptr<Expression const> l
                 , util::sptr<Expression const> r)
            : Expression(pos)
            , lhs(std::move(l))
            , rhs(std::move(r))
        {}

        std::string str() const;
        bool mayThrow() const { return lhs->mayThrow() || rhs->mayThrow(); }

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
        bool mayThrow() const { return lhs->mayThrow() || rhs->mayThrow(); }

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
        bool mayThrow() const { return rhs->mayThrow(); }

        std::string const op;
        util::sptr<Expression const> const rhs;
    };

    struct Lambda
        : Expression
    {
        Lambda(misc::position const& pos
             , std::vector<std::string> const& p
             , util::sptr<Statement const> b
             , bool mp)
                : Expression(pos)
                , param_names(p)
                , body(std::move(b))
                , mangle_as_param(mp)
        {}

        std::string str() const;
        bool mayThrow() const { return false; }

        std::vector<std::string> const param_names;
        util::sptr<Statement const> const body;
        bool const mangle_as_param;
    };

    struct RegularAsyncLambda
        : Expression
    {
        RegularAsyncLambda(misc::position const& pos
                         , std::vector<std::string> const& p
                         , int async_param_idx
                         , util::sptr<Statement const> b)
            : Expression(pos)
            , param_names(p)
            , async_param_index(async_param_idx)
            , body(std::move(b))
        {}

        std::string str() const;
        bool mayThrow() const { return false; }

        std::vector<std::string> const param_names;
        int const async_param_index;
        util::sptr<Statement const> const body;
    };

    struct AsyncReference
        : PrimeFactor
    {
        AsyncReference(misc::position const& pos, util::id const& id)
            : PrimeFactor(pos)
            , ref_id(id)
        {}

        std::string str() const;

        util::id const ref_id;
    };

    struct RegularAsyncCallbackArg
        : Expression
    {
        RegularAsyncCallbackArg(misc::position const& pos
                              , util::sptr<Statement const> b
                              , Method t)
            : Expression(pos)
            , body(std::move(b))
            , thrower(std::move(t))
        {}

        std::string str() const;
        bool mayThrow() const { return true; }

        util::sptr<Statement const> const body;
        Method const thrower;
    };

    struct This
        : PrimeFactor
    {
        explicit This(misc::position const& pos)
            : PrimeFactor(pos)
        {}

        std::string str() const;
    };

    struct SuperFunc
        : Expression
    {
        SuperFunc(misc::position const& pos, std::string prop)
            : Expression(pos)
            , property(prop)
        {}

        std::string str() const;
        bool mayThrow() const { return true; }

        std::string const property;
    };

    struct Conditional
        : Expression
    {
        Conditional(misc::position const& pos
                  , util::sptr<Expression const> p
                  , util::sptr<Expression const> c
                  , util::sptr<Expression const> a)
            : Expression(pos)
            , predicate(std::move(p))
            , consequence(std::move(c))
            , alternative(std::move(a))
        {}

        std::string str() const;
        bool mayThrow() const { return true; }

        util::sptr<Expression const> const predicate;
        util::sptr<Expression const> const consequence;
        util::sptr<Expression const> const alternative;
    };

    struct ExceptionObj
        : PrimeFactor
    {
        explicit ExceptionObj(misc::position const& pos)
            : PrimeFactor(pos)
        {}

        std::string str() const;
    };

    struct ConditionalCallbackParameter
        : PrimeFactor
    {
        explicit ConditionalCallbackParameter(misc::position const& pos)
            : PrimeFactor(pos)
        {}

        std::string str() const;
    };

    struct SuperConstructorCall
        : Expression
    {
        SuperConstructorCall(misc::position const& pos, std::string cn
                           , util::ptrarr<Expression const> a)
            : Expression(pos)
            , class_name(std::move(cn))
            , args(std::move(a))
        {}

        std::string str() const;
        bool mayThrow() const { return true; }

        std::string const class_name;
        util::ptrarr<Expression const> const args;
    };

}

#endif /* __STEKIN_OUTPUT_EXPRESSION_NODES_H__ */
