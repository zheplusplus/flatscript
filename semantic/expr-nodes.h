#ifndef __STEKIN_SEMANTIC_EXPRESSION_NODES_H__
#define __STEKIN_SEMANTIC_EXPRESSION_NODES_H__

#include <string>

#include <util/arrays.h>

#include "node-base.h"
#include "fwd-decl.h"

namespace semantic {

    struct PreUnaryOp
        : Expression
    {
        PreUnaryOp(misc::position const& pos, std::string const& op, util::sptr<Expression const> r)
            : Expression(pos)
            , op_img(op)
            , rhs(std::move(r))
        {}

        util::sptr<output::Expression const> compile(CompilingSpace& space) const;
        bool isLiteral(util::sref<SymbolTable const>) const;
        std::string literalType(util::sref<SymbolTable const>) const;
        bool boolValue(util::sref<SymbolTable const>) const;
        mpz_class intValue(util::sref<SymbolTable const>) const;
        mpf_class floatValue(util::sref<SymbolTable const>) const;
        std::string stringValue(util::sref<SymbolTable const>) const;
        bool isAsync() const;

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

        util::sptr<output::Expression const> compile(CompilingSpace& space) const;
        bool isLiteral(util::sref<SymbolTable const>) const;
        std::string literalType(util::sref<SymbolTable const>) const;
        bool boolValue(util::sref<SymbolTable const>) const;
        mpz_class intValue(util::sref<SymbolTable const>) const;
        mpf_class floatValue(util::sref<SymbolTable const>) const;
        std::string stringValue(util::sref<SymbolTable const>) const;
        bool isAsync() const;

        util::sptr<Expression const> const lhs;
        std::string const op_img;
        util::sptr<Expression const> const rhs;
    };

    struct TypeOf
        : Expression
    {
        TypeOf(misc::position const& pos, util::sptr<Expression const> e)
            : Expression(pos)
            , expr(std::move(e))
        {}

        util::sptr<output::Expression const> compile(CompilingSpace& space) const;
        bool isLiteral(util::sref<SymbolTable const> st) const;
        std::string literalType(util::sref<SymbolTable const>) const { return "string"; }
        std::string stringValue(util::sref<SymbolTable const> st) const;

        util::sptr<Expression const> const expr;
    };

    struct Reference
        : Expression
    {
        Reference(misc::position const& pos, std::string const& n)
            : Expression(pos)
            , name(n)
        {}

        util::sptr<output::Expression const> compile(CompilingSpace& space) const;
        bool isLiteral(util::sref<SymbolTable const> st) const;
        std::string literalType(util::sref<SymbolTable const>) const;
        bool boolValue(util::sref<SymbolTable const>) const;
        mpz_class intValue(util::sref<SymbolTable const>) const;
        mpf_class floatValue(util::sref<SymbolTable const>) const;
        std::string stringValue(util::sref<SymbolTable const>) const;

        std::string const name;
    };

    struct BoolLiteral
        : Expression
    {
        BoolLiteral(misc::position const& pos, bool v)
            : Expression(pos)
            , value(v)
        {}

        util::sptr<output::Expression const> compile(CompilingSpace&) const;
        bool isLiteral(util::sref<SymbolTable const>) const { return true; }
        std::string literalType(util::sref<SymbolTable const>) const { return "bool"; }
        bool boolValue(util::sref<SymbolTable const>) const;

        bool const value;
    };

    struct IntLiteral
        : Expression
    {
        IntLiteral(misc::position const& pos, std::string const& image)
            : Expression(pos)
            , value(image)
        {}

        IntLiteral(misc::position const& pos, mpz_class const& v)
            : Expression(pos)
            , value(v)
        {}

        util::sptr<output::Expression const> compile(CompilingSpace&) const;
        bool isLiteral(util::sref<SymbolTable const>) const { return true; }
        std::string literalType(util::sref<SymbolTable const>) const { return "int"; }
        mpz_class intValue(util::sref<SymbolTable const>) const;

        mpz_class const value;
    };

    struct FloatLiteral
        : Expression
    {
        FloatLiteral(misc::position const& pos, std::string const& image)
            : Expression(pos)
            , value(image)
        {}

        FloatLiteral(misc::position const& pos, mpf_class const& v)
            : Expression(pos)
            , value(v)
        {}

        util::sptr<output::Expression const> compile(CompilingSpace&) const;
        bool isLiteral(util::sref<SymbolTable const>) const { return true; }
        std::string literalType(util::sref<SymbolTable const>) const { return "float"; }
        mpf_class floatValue(util::sref<SymbolTable const>) const;

        mpf_class const value;
    };

    struct StringLiteral
        : Expression
    {
        StringLiteral(misc::position const& pos, std::string const& image)
            : Expression(pos)
            , value(image)
        {}

        util::sptr<output::Expression const> compile(CompilingSpace&) const;
        bool isLiteral(util::sref<SymbolTable const>) const { return true; }
        std::string literalType(util::sref<SymbolTable const>) const { return "string"; }
        bool boolValue(util::sref<SymbolTable const>) const;
        std::string stringValue(util::sref<SymbolTable const>) const;

        std::string const value;
    };

    struct ListLiteral
        : Expression
    {
        ListLiteral(misc::position const& pos, util::ptrarr<Expression const> v)
            : Expression(pos)
            , value(std::move(v))
        {}

        util::sptr<output::Expression const> compile(CompilingSpace& space) const;
        bool isAsync() const;

        util::ptrarr<Expression const> const value;
    };

    struct PipeElement
        : Expression
    {
        explicit PipeElement(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<output::Expression const> compile(CompilingSpace&) const;
    };

    struct PipeIndex
        : Expression
    {
        explicit PipeIndex(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<output::Expression const> compile(CompilingSpace&) const;
    };

    struct PipeKey
        : Expression
    {
        explicit PipeKey(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<output::Expression const> compile(CompilingSpace&) const;
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

        util::sptr<output::Expression const> compile(CompilingSpace& space) const;
        bool isAsync() const;

        util::sptr<Expression const> const lhs;
        util::sptr<Expression const> const rhs;
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

        util::sptr<output::Expression const> compile(CompilingSpace& space) const;
        bool isAsync() const;

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

        util::sptr<output::Expression const> compile(CompilingSpace& space) const;
        bool isAsync() const;

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

        util::sptr<output::Expression const> compile(CompilingSpace& space) const;
        bool isAsync() const;

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

            util::sptr<output::Expression const> compile(CompilingSpace&) const;
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

        util::sptr<output::Expression const> compile(CompilingSpace& space) const;
        bool isAsync() const;

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

        util::sptr<output::Expression const> compile(CompilingSpace& space) const;
        bool isAsync() const;

        util::ptrkvarr<Expression const> const items;
    };

    struct Lambda
        : Expression
    {
        Lambda(misc::position const& pos
             , std::vector<std::string> const& p
             , util::sptr<Filter const> b)
                : Expression(pos)
                , param_names(p)
                , body(std::move(b))
        {}

        util::sptr<output::Expression const> compile(CompilingSpace& space) const;

        std::vector<std::string> const param_names;
        util::sptr<Filter const> const body;
    };

    struct AsyncCall
        : Expression
    {
        AsyncCall(misc::position const& pos
                , util::sptr<Expression const> c
                , util::ptrarr<Expression const> fargs
                , std::vector<std::string> const ap
                , util::ptrarr<Expression const> largs)
            : Expression(pos)
            , callee(std::move(c))
            , former_args(std::move(fargs))
            , async_params(ap)
            , latter_args(std::move(largs))
        {}

        util::sptr<Expression const> const callee;
        util::ptrarr<Expression const> const former_args;
        std::vector<std::string> const async_params;
        util::ptrarr<Expression const> const latter_args;

        util::sptr<output::Expression const> compile(CompilingSpace& space) const;
        bool isAsync() const { return true; }
    };

}

#endif /* __STEKIN_SEMANTIC_EXPRESSION_NODES_H__ */
