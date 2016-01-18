#ifndef __STEKIN_SEMANTIC_EXPRESSION_NODES_H__
#define __STEKIN_SEMANTIC_EXPRESSION_NODES_H__

#include <util/arrays.h>
#include <util/uid.h>

#include "node-base.h"

namespace semantic {

    struct Undefined
        : Expression
    {
        explicit Undefined(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<output::Expression const> compileAsRoot(util::sref<Scope>) const
        {
            return util::sptr<output::Expression const>(nullptr);
        }

        bool isAsync() const { return false; }

        util::sptr<output::Expression const> compile(util::sref<Scope>) const;
    };

    struct PreUnaryOp
        : Expression
    {
        PreUnaryOp(misc::position const& pos, std::string const& op, util::sptr<Expression const> r)
            : Expression(pos)
            , op_img(op)
            , rhs(std::move(r))
        {}

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;
        util::sptr<output::Expression const> compileAsRoot(util::sref<Scope> scope) const
        {
            return rhs->compileAsRoot(scope);
        }

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

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;
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

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;
        util::sptr<output::Expression const> compileAsRoot(util::sref<Scope> scope) const
        {
            return expr->compileAsRoot(scope);
        }

        bool isAsync() const { return this->expr->isAsync(); }

        bool boolValue(util::sref<SymbolTable const>) const { return true; }
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

        util::sptr<output::Expression const> compileAsRoot(util::sref<Scope> scope) const
        {
            this->compile(scope);
            return util::sptr<output::Expression const>(nullptr);
        }

        bool isAsync() const { return false; }

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;
        bool isLiteral(util::sref<SymbolTable const> st) const;
        std::string literalType(util::sref<SymbolTable const>) const;
        bool boolValue(util::sref<SymbolTable const>) const;
        mpz_class intValue(util::sref<SymbolTable const>) const;
        mpf_class floatValue(util::sref<SymbolTable const>) const;
        std::string stringValue(util::sref<SymbolTable const>) const;

        std::string const name;
    };

    struct LiteralFactor
        : Expression
    {
        explicit LiteralFactor(misc::position const& pos)
            : Expression(pos)
        {}

        bool isLiteral(util::sref<SymbolTable const>) const { return true; }
        bool isAsync() const { return false; }

        util::sptr<output::Expression const> compileAsRoot(util::sref<Scope>) const
        {
            return util::sptr<output::Expression const>(nullptr);
        }
    };

    struct BoolLiteral
        : LiteralFactor
    {
        BoolLiteral(misc::position const& pos, bool v)
            : LiteralFactor(pos)
            , value(v)
        {}

        util::sptr<output::Expression const> compile(util::sref<Scope>) const;
        std::string literalType(util::sref<SymbolTable const>) const { return "bool"; }
        bool boolValue(util::sref<SymbolTable const>) const;

        bool const value;
    };

    struct IntLiteral
        : LiteralFactor
    {
        IntLiteral(misc::position const& pos, std::string const& image)
            : LiteralFactor(pos)
            , value(image)
        {}

        IntLiteral(misc::position const& pos, mpz_class const& v)
            : LiteralFactor(pos)
            , value(v)
        {}

        util::sptr<output::Expression const> compile(util::sref<Scope>) const;
        bool boolValue(util::sref<SymbolTable const>) const { return this->value != 0; }
        std::string literalType(util::sref<SymbolTable const>) const { return "int"; }
        mpz_class intValue(util::sref<SymbolTable const>) const;

        mpz_class const value;
    };

    struct FloatLiteral
        : LiteralFactor
    {
        FloatLiteral(misc::position const& pos, std::string const& image)
            : LiteralFactor(pos)
            , value(image)
        {}

        FloatLiteral(misc::position const& pos, mpf_class const& v)
            : LiteralFactor(pos)
            , value(v)
        {}

        util::sptr<output::Expression const> compile(util::sref<Scope>) const;
        bool boolValue(util::sref<SymbolTable const>) const { return this->value != 0; }
        std::string literalType(util::sref<SymbolTable const>) const { return "float"; }
        mpf_class floatValue(util::sref<SymbolTable const>) const;

        mpf_class const value;
    };

    struct StringLiteral
        : LiteralFactor
    {
        StringLiteral(misc::position const& pos, std::string const& image)
            : LiteralFactor(pos)
            , value(image)
        {}

        util::sptr<output::Expression const> compile(util::sref<Scope>) const;
        std::string literalType(util::sref<SymbolTable const>) const { return "string"; }
        bool boolValue(util::sref<SymbolTable const>) const;
        std::string stringValue(util::sref<SymbolTable const>) const;

        std::string const value;
    };

    struct RegEx
        : Expression
    {
        RegEx(misc::position const& pos, std::string v)
            : Expression(pos)
            , value(std::move(v))
        {}

        util::sptr<output::Expression const> compile(util::sref<Scope>) const;

        util::sptr<output::Expression const> compileAsRoot(util::sref<Scope>) const
        {
            return util::sptr<output::Expression const>(nullptr);
        }
        bool isAsync() const { return false; }

        std::string const value;
    };

    struct ListLiteral
        : Expression
    {
        ListLiteral(misc::position const& pos, util::ptrarr<Expression const> v)
            : Expression(pos)
            , value(std::move(v))
        {}

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;
        bool boolValue(util::sref<SymbolTable const>) const { return true; }
        bool isAsync() const;

        util::ptrarr<Expression const> const value;
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

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;
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

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;
        bool isAsync() const;

        util::sptr<Expression const> const callee;
        util::ptrarr<Expression const> const args;
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

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;
        bool isAsync() const;

        std::string const class_name;
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

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;
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

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;
        bool isAsync() const;

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

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;
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

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;
        bool isAsync() const;

        util::ptrkvarr<Expression const> const items;
    };

    struct RegularAsyncCall
        : Expression
    {
        RegularAsyncCall(misc::position const& pos
                       , util::sptr<Expression const> c
                       , util::ptrarr<Expression const> fargs
                       , util::ptrarr<Expression const> largs)
            : Expression(pos)
            , callee(std::move(c))
            , former_args(std::move(fargs))
            , latter_args(std::move(largs))
        {}

        util::sptr<Expression const> const callee;
        util::ptrarr<Expression const> const former_args;
        util::ptrarr<Expression const> const latter_args;

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;
        util::sptr<output::Expression const> compileAsRoot(util::sref<Scope> scope) const;

        bool isAsync() const { return true; }
    private:
        virtual util::uid _compile(util::sref<Scope> scope, bool root) const;
    };

    struct AsyncCall
        : RegularAsyncCall
    {
        AsyncCall(misc::position const& pos
                , util::sptr<Expression const> callee
                , util::ptrarr<Expression const> fargs
                , std::vector<std::string> const ap
                , util::ptrarr<Expression const> largs)
            : RegularAsyncCall(pos, std::move(callee), std::move(fargs), std::move(largs))
            , async_params(ap)
        {}

        std::vector<std::string> const async_params;
    private:
        util::uid _compile(util::sref<Scope> scope, bool root) const;
    };

    struct This
        : Expression
    {
        explicit This(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<output::Expression const> compileAsRoot(util::sref<Scope>) const
        {
            return util::sptr<output::Expression const>(nullptr);
        }

        bool isAsync() const { return false; }

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;
    };

    struct SuperFunc
        : Expression
    {
        SuperFunc(misc::position const& pos, std::string prop)
            : Expression(pos)
            , property(prop)
        {}

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;

        bool isAsync() const { return false; }

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

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;
        bool isLiteral(util::sref<SymbolTable const> sym) const;
        std::string literalType(util::sref<SymbolTable const> sym) const;
        bool boolValue(util::sref<SymbolTable const> sym) const;
        mpz_class intValue(util::sref<SymbolTable const> sym) const;
        mpf_class floatValue(util::sref<SymbolTable const> sym) const;
        std::string stringValue(util::sref<SymbolTable const> sym) const;
        bool isAsync() const;

        util::sptr<Expression const> const predicate;
        util::sptr<Expression const> const consequence;
        util::sptr<Expression const> const alternative;
    private:
        util::sref<Expression const> _equivVal(util::sref<SymbolTable const> sym) const;
        util::sptr<output::Expression const> _compileSync(util::sref<Scope> scope) const;
        util::sptr<output::Expression const> _compileAsync(util::sref<Scope> scope) const;
    };

    struct ExceptionObjDeprecated
        : Expression
    {
        explicit ExceptionObjDeprecated(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<output::Expression const> compileAsRoot(util::sref<Scope> scope) const
        {
            this->compile(scope);
            return util::sptr<output::Expression const>(nullptr);
        }

        bool isAsync() const { return false; }

        util::sptr<output::Expression const> compile(util::sref<Scope> scope) const;
    };

}

#endif /* __STEKIN_SEMANTIC_EXPRESSION_NODES_H__ */
