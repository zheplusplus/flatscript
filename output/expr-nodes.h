#ifndef __STEKIN_OUTPUT_EXPRESSION_NODES_H__
#define __STEKIN_OUTPUT_EXPRESSION_NODES_H__

#include <string>
#include <gmpxx.h>

#include <util/arrays.h>
#include <util/uid.h>

#include "node-base.h"
#include "methods.h"

namespace output {

    struct PrimeFactor
        : Expression
    {
        bool mayThrow() const { return false; }
    };

    struct Undefined
        : PrimeFactor
    {
        std::string str() const;
    };

    struct BoolLiteral
        : PrimeFactor
    {
        explicit BoolLiteral(bool v)
            : value(v)
        {}

        std::string str() const;

        bool const value;
    };

    struct IntLiteral
        : PrimeFactor
    {
        explicit IntLiteral(mpz_class const& v)
            : value(v)
        {}

        std::string str() const;

        mpz_class const value;
    };

    struct FloatLiteral
        : PrimeFactor
    {
        explicit FloatLiteral(mpf_class const& v)
            : value(v)
        {}

        std::string str() const;

        mpf_class const value;
    };

    struct StringLiteral
        : PrimeFactor
    {
        explicit StringLiteral(std::string const& v)
            : value(v)
        {}

        std::string str() const;

        std::string const value;
    };

    struct RegEx
        : PrimeFactor
    {
        explicit RegEx(std::string v)
            : value(std::move(v))
        {}

        std::string str() const;

        std::string const value;
    };

    struct ListLiteral
        : Expression
    {
        ListLiteral(util::ptrarr<Expression const> v)
            : value(std::move(v))
        {}

        std::string str() const;
        bool mayThrow() const;

        util::ptrarr<Expression const> const value;
    };

    struct Reference
        : PrimeFactor
    {
        explicit Reference(std::string const& n)
            : name(n)
        {}

        std::string str() const;

        std::string const name;
    };

    struct SubReference
        : Reference
    {
        SubReference(std::string const& name, util::uid sid)
            : Reference(name)
            , space_id(sid)
        {}

        std::string str() const;

        util::uid const space_id;
    };

    struct TransientParamReference
        : Reference
    {
        explicit TransientParamReference(std::string const& name)
            : Reference(name)
        {}

        std::string str() const;
    };

    struct ImportedName
        : Expression
    {
        explicit ImportedName(std::string n)
            : name(std::move(n))
        {}

        std::string str() const;
        bool mayThrow() const { return true; }

        std::string const name;
    };

    struct Call
        : Expression
    {
        Call(util::sptr<Expression const> c, util::ptrarr<Expression const> a)
            : callee(std::move(c))
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
        MemberAccess(util::sptr<Expression const> ref, std::string const& mem)
            : referee(std::move(ref))
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
        Lookup(util::sptr<Expression const> c, util::sptr<Expression const> k)
            : collection(std::move(c))
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
        ListSlice(util::sptr<Expression const> ls
                , util::sptr<Expression const> b
                , util::sptr<Expression const> e
                , util::sptr<Expression const> s)
            : list(std::move(ls))
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
        Dictionary()
            : items(util::ptrkvarr<Expression const>())
        {}

        explicit Dictionary(util::ptrkvarr<Expression const> i)
            : items(std::move(i))
        {}

        std::string str() const;
        bool mayThrow() const { return true; }

        util::ptrkvarr<Expression const> const items;
    };

    struct ListAppend
        : Expression
    {
        ListAppend(util::sptr<Expression const> l, util::sptr<Expression const> r)
            : lhs(std::move(l))
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
        Assignment(util::sptr<Expression const> l, util::sptr<Expression const> r)
            : lhs(std::move(l))
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
        BinaryOp(util::sptr<Expression const> l
               , std::string const& o
               , util::sptr<Expression const> r)
            : lhs(std::move(l))
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
        PreUnaryOp(std::string const& o, util::sptr<Expression const> r)
            : op(o)
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
        Lambda(std::vector<std::string> p, util::sptr<Statement const> b, bool mp)
            : param_names(std::move(p))
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
        RegularAsyncLambda(std::vector<std::string> const& p
                         , int async_param_idx
                         , util::sptr<Statement const> b)
            : param_names(p)
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
        explicit AsyncReference(util::uid const& id)
            : ref_id(id)
        {}

        std::string str() const;

        util::uid const ref_id;
    };

    struct RegularAsyncCallbackArg
        : Expression
    {
        RegularAsyncCallbackArg(util::sptr<Statement const> b, Method t)
            : body(std::move(b))
            , thrower(std::move(t))
            , id(util::uid::next_id())
        {}

        std::string str() const;
        bool mayThrow() const { return true; }

        util::sptr<Statement const> const body;
        Method const thrower;
        util::uid const id;
    };

    struct This
        : PrimeFactor
    {
        std::string str() const;
    };

    struct SuperFunc
        : Expression
    {
        explicit SuperFunc(std::string prop)
            : property(std::move(prop))
        {}

        std::string str() const;
        bool mayThrow() const { return true; }

        std::string const property;
    };

    struct Conditional
        : Expression
    {
        Conditional(util::sptr<Expression const> p
                  , util::sptr<Expression const> c
                  , util::sptr<Expression const> a)
            : predicate(std::move(p))
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
        std::string str() const;
    };

    struct ConditionalCallbackParameter
        : PrimeFactor
    {
        std::string str() const;
    };

    struct SuperConstructorCall
        : Expression
    {
        SuperConstructorCall(std::string cn, util::ptrarr<Expression const> a)
            : class_name(std::move(cn))
            , args(std::move(a))
        {}

        std::string str() const;
        bool mayThrow() const { return true; }

        std::string const class_name;
        util::ptrarr<Expression const> const args;
    };

}

#endif /* __STEKIN_OUTPUT_EXPRESSION_NODES_H__ */
