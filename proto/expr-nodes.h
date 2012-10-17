#ifndef __STEKIN_PROTO_EXPRESSION_NODES_H__
#define __STEKIN_PROTO_EXPRESSION_NODES_H__

#include <string>
#include <vector>
#include <gmpxx.h>

#include "node-base.h"
#include "fwd-decl.h"

namespace proto {

    struct BoolLiteral
        : public Expression
    {
        BoolLiteral(misc::position const& pos, bool v)
            : Expression(pos)
            , value(v)
        {}

        void write() const;

        bool const value;
    };

    struct IntLiteral
        : public Expression
    {
        IntLiteral(misc::position const& pos, mpz_class const& v)
            : Expression(pos)
            , value(v)
        {}

        void write() const;

        mpz_class const value;
    };

    struct FloatLiteral
        : public Expression
    {
        FloatLiteral(misc::position const& pos, mpf_class const& v)
            : Expression(pos)
            , value(v)
        {}

        void write() const;

        mpf_class const value;
    };

    struct StringLiteral
        : public Expression
    {
        StringLiteral(misc::position const& pos, std::string const& v)
            : Expression(pos)
            , value(v)
        {}

        void write() const;

        std::string const value;
    };

    struct ListLiteral
        : public Expression
    {
        ListLiteral(misc::position const& pos, std::vector<util::sptr<Expression const>> v)
            : Expression(pos)
            , value(std::move(v))
        {}

        void write() const;
        void writeAsPipe() const;

        std::vector<util::sptr<Expression const>> const value;
    };

    struct ListElement
        : public Expression
    {
        explicit ListElement(misc::position const& pos)
            : Expression(pos)
        {}

        void write() const;
        void writeAsPipe() const;
    };

    struct ListIndex
        : public Expression
    {
        explicit ListIndex(misc::position const& pos)
            : Expression(pos)
        {}

        void write() const;
        void writeAsPipe() const;
    };

    struct Reference
        : public Expression
    {
        Reference(misc::position const& pos, std::string const& n)
            : Expression(pos)
            , name(n)
        {}

        void write() const;

        std::string const name;
    };

    struct Call
        : public Expression
    {
        Call(misc::position const& pos
           , std::string const& n
           , std::vector<util::sptr<Expression const>> a)
                : Expression(pos)
                , name(n)
                , args(std::move(a))
        {}

        void write() const;
        void writeAsPipe() const;
    public:
        std::string const name;
        std::vector<util::sptr<Expression const>> const args;
    };

    struct FuncReference
        : public Expression
    {
        FuncReference(misc::position const& pos, util::sref<Function> f)
            : Expression(pos)
            , func(f)
        {}

        void write() const;
        util::sref<Function> const func;
    };

    struct ListAppend
        : public Expression
    {
        ListAppend(misc::position const& pos
                 , util::sptr<Expression const> l
                 , util::sptr<Expression const> r)
            : Expression(pos)
            , lhs(std::move(l))
            , rhs(std::move(r))
        {}

        void write() const;
        void writeAsPipe() const;

        util::sptr<Expression const> const lhs;
        util::sptr<Expression const> const rhs;
    };

    struct BinaryOp
        : public Expression
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

        void write() const;
        void writeAsPipe() const;

        util::sptr<Expression const> const lhs;
        std::string const op;
        util::sptr<Expression const> const rhs;
    };

    struct PreUnaryOp
        : public Expression
    {
        PreUnaryOp(misc::position const& pos, std::string const& o, util::sptr<Expression const> r)
            : Expression(pos)
            , op(o)
            , rhs(std::move(r))
        {}

        void write() const;
        void writeAsPipe() const;

        std::string const op;
        util::sptr<Expression const> const rhs;
    };

    struct Conjunction
        : public Expression
    {
        Conjunction(misc::position const& pos
                  , util::sptr<Expression const> l
                  , util::sptr<Expression const> r)
            : Expression(pos)
            , lhs(std::move(l))
            , rhs(std::move(r))
        {}

        void write() const;
        void writeAsPipe() const;

        util::sptr<Expression const> const lhs;
        util::sptr<Expression const> const rhs;
    };

    struct Disjunction
        : public Expression
    {
        Disjunction(misc::position const& pos
                  , util::sptr<Expression const> l
                  , util::sptr<Expression const> r)
            : Expression(pos)
            , lhs(std::move(l))
            , rhs(std::move(r))
        {}

        void write() const;
        void writeAsPipe() const;

        util::sptr<Expression const> const lhs;
        util::sptr<Expression const> const rhs;
    };

    struct Negation
        : public Expression
    {
        Negation(misc::position const& pos, util::sptr<Expression const> r)
            : Expression(pos)
            , rhs(std::move(r))
        {}

        void write() const;
        void writeAsPipe() const;

        util::sptr<Expression const> const rhs;
    };

}

#endif /* __STEKIN_PROTO_EXPRESSION_NODES_H__ */
