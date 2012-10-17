#ifndef __STEKIN_GRAMMAR_EXPRESSION_NODES_H__
#define __STEKIN_GRAMMAR_EXPRESSION_NODES_H__

#include <vector>

#include "node-base.h"

namespace grammar {

    struct PreUnaryOp
        : public Expression
    {
        PreUnaryOp(misc::position const& pos, std::string const& op, util::sptr<Expression const> r)
            : Expression(pos)
            , op_img(op)
            , rhs(std::move(r))
        {}

        util::sptr<flchk::Expression const> compile() const;

        std::string const op_img;
        util::sptr<Expression const> const rhs;
    };

    struct BinaryOp
        : public Expression
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

        util::sptr<flchk::Expression const> compile() const;

        util::sptr<Expression const> const lhs;
        std::string const op_img;
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

        util::sptr<flchk::Expression const> compile() const;

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

        util::sptr<flchk::Expression const> compile() const;

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

        util::sptr<flchk::Expression const> compile() const;

        util::sptr<Expression const> const rhs;
    };

    struct Reference
        : public Expression
    {
        Reference(misc::position const& pos, std::string const& n)
            : Expression(pos)
            , name(n)
        {}

        util::sptr<flchk::Expression const> compile() const;

        std::string const name;
    };

    struct BoolLiteral
        : public Expression
    {
        BoolLiteral(misc::position const& pos, bool v)
            : Expression(pos)
            , value(v)
        {}

        util::sptr<flchk::Expression const> compile() const;

        bool const value;
    };

    struct IntLiteral
        : public Expression
    {
        IntLiteral(misc::position const& pos, std::string const& v)
            : Expression(pos)
            , value(v)
        {}

        util::sptr<flchk::Expression const> compile() const;

        std::string const value;
    };

    struct FloatLiteral
        : public Expression
    {
        FloatLiteral(misc::position const& pos, std::string const& v)
            : Expression(pos)
            , value(v)
        {}

        util::sptr<flchk::Expression const> compile() const;

        std::string const value;
    };

    struct StringLiteral
        : public Expression
    {
        StringLiteral(misc::position const& pos, std::string const& v)
            : Expression(pos)
            , value(v)
        {}

        util::sptr<flchk::Expression const> compile() const;

        std::string const value;
    };

    struct ListLiteral
        : public Expression
    {
        ListLiteral(misc::position const& pos, std::vector<util::sptr<Expression const>> v)
            : Expression(pos)
            , value(std::move(v))
        {}

        util::sptr<flchk::Expression const> compile() const;

        std::vector<util::sptr<Expression const>> const value;
    };

    struct ListElement
        : public Expression
    {
        explicit ListElement(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<flchk::Expression const> compile() const;
    };

    struct ListIndex
        : public Expression
    {
        explicit ListIndex(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<flchk::Expression const> compile() const;
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

        util::sptr<flchk::Expression const> compile() const;

        util::sptr<Expression const> const lhs;
        util::sptr<Expression const> const rhs;
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

        util::sptr<flchk::Expression const> compile() const;
        util::sptr<flchk::Call const> cplMemberCall() const;

        std::string const name;
        std::vector<util::sptr<Expression const>> const args;
    };

}

#endif /* __STEKIN_GRAMMAR_EXPRESSION_NODES_H__ */
