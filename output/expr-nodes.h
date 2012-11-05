#ifndef __STEKIN_OUTPUT_EXPRESSION_NODES_H__
#define __STEKIN_OUTPUT_EXPRESSION_NODES_H__

#include <string>
#include <vector>
#include <gmpxx.h>

#include "node-base.h"
#include "fwd-decl.h"

namespace output {

    struct BoolLiteral
        : Expression
    {
        BoolLiteral(misc::position const& pos, bool v)
            : Expression(pos)
            , value(v)
        {}

        std::string str(bool) const;

        bool const value;
    };

    struct IntLiteral
        : Expression
    {
        IntLiteral(misc::position const& pos, mpz_class const& v)
            : Expression(pos)
            , value(v)
        {}

        std::string str(bool) const;

        mpz_class const value;
    };

    struct FloatLiteral
        : Expression
    {
        FloatLiteral(misc::position const& pos, mpf_class const& v)
            : Expression(pos)
            , value(v)
        {}

        std::string str(bool) const;

        mpf_class const value;
    };

    struct StringLiteral
        : Expression
    {
        StringLiteral(misc::position const& pos, std::string const& v)
            : Expression(pos)
            , value(v)
        {}

        std::string str(bool) const;

        std::string const value;
    };

    struct ListLiteral
        : Expression
    {
        ListLiteral(misc::position const& pos, std::vector<util::sptr<Expression const>> v)
            : Expression(pos)
            , value(std::move(v))
        {}

        std::string str(bool in_pipe) const;

        std::vector<util::sptr<Expression const>> const value;
    };

    struct ListElement
        : Expression
    {
        explicit ListElement(misc::position const& pos)
            : Expression(pos)
        {}

        std::string str(bool in_pipe) const;
    };

    struct ListIndex
        : Expression
    {
        explicit ListIndex(misc::position const& pos)
            : Expression(pos)
        {}

        std::string str(bool in_pipe) const;
    };

    struct Reference
        : Expression
    {
        Reference(misc::position const& pos, std::string const& n)
            : Expression(pos)
            , name(n)
        {}

        std::string str(bool) const;

        std::string const name;
    };

    struct ImportedName
        : Expression
    {
        ImportedName(misc::position const& pos, std::string const& n)
            : Expression(pos)
            , name(n)
        {}

        std::string str(bool) const;

        std::string const name;
    };

    struct Call
        : Expression
    {
        Call(misc::position const& pos
           , util::sptr<Expression const> c
           , std::vector<util::sptr<Expression const>> a)
                : Expression(pos)
                , callee(std::move(c))
                , args(std::move(a))
        {}

        std::string str(bool in_pipe) const;

        util::sptr<Expression const> const callee;
        std::vector<util::sptr<Expression const>> const args;
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

        std::string str(bool in_pipe) const;

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

        std::string str(bool in_pipe) const;

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

            std::string str(bool) const;
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

        std::string str(bool in_pipe) const;

        util::sptr<Expression const> const list;
        util::sptr<Expression const> const begin;
        util::sptr<Expression const> const end;
        util::sptr<Expression const> const step;
    };

    struct Dictionary
        : Expression
    {
        typedef std::pair<util::sptr<Expression const>, util::sptr<Expression const>> ItemType;

        Dictionary(misc::position const& pos, std::vector<ItemType> i)
            : Expression(pos)
            , items(std::move(i))
        {}

        std::string str(bool in_pipe) const;

        std::vector<ItemType> const items;
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

        std::string str(bool in_pipe) const;

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

        std::string str(bool in_pipe) const;

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

        std::string str(bool in_pipe) const;

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

        std::string str(bool) const;

        std::vector<std::string> const param_names;
        util::sptr<Statement const> const body;
    };

}

#endif /* __STEKIN_OUTPUT_EXPRESSION_NODES_H__ */
