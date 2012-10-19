#ifndef __STEKIN_FLOWCHECK_EXPRESSION_NODES_H__
#define __STEKIN_FLOWCHECK_EXPRESSION_NODES_H__

#include <string>
#include <vector>

#include <util/pointer.h>

#include "node-base.h"
#include "fwd-decl.h"

namespace flchk {

    struct PreUnaryOp
        : Expression
    {
        PreUnaryOp(misc::position const& pos, std::string const& op, util::sptr<Expression const> r)
            : Expression(pos)
            , op_img(op)
            , rhs(std::move(r))
        {}

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable> st) const;
        bool isLiteral() const;
        bool boolValue() const;
        util::sptr<Expression const> fold() const;
        std::string typeName() const;

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

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable> st) const;
        bool isLiteral() const;
        bool boolValue() const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

        util::sptr<Expression const> const lhs;
        std::string const op_img;
        util::sptr<Expression const> const rhs;
    };

    struct Conjunction
        : Expression
    {
        Conjunction(misc::position const& pos
                  , util::sptr<Expression const> l
                  , util::sptr<Expression const> r)
            : Expression(pos)
            , lhs(std::move(l))
            , rhs(std::move(r))
        {}

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable> st) const;
        bool isLiteral() const;
        bool boolValue() const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

        util::sptr<Expression const> const lhs;
        util::sptr<Expression const> const rhs;
    };

    struct Disjunction
        : Expression
    {
        Disjunction(misc::position const& pos
                  , util::sptr<Expression const> l
                  , util::sptr<Expression const> r)
            : Expression(pos)
            , lhs(std::move(l))
            , rhs(std::move(r))
        {}

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable> st) const;
        bool isLiteral() const;
        bool boolValue() const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

        util::sptr<Expression const> const lhs;
        util::sptr<Expression const> const rhs;
    };

    struct Negation
        : Expression
    {
        Negation(misc::position const& pos, util::sptr<Expression const> r)
            : Expression(pos)
            , rhs(std::move(r))
        {}

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable> st) const;
        bool isLiteral() const;
        bool boolValue() const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

        util::sptr<Expression const> const rhs;
    };

    struct Reference
        : Expression
    {
        Reference(misc::position const& pos, std::string const& n)
            : Expression(pos)
            , name(n)
        {}

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable> st) const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

        std::string const name;
    };

    struct BoolLiteral
        : Expression
    {
        BoolLiteral(misc::position const& pos, bool v)
            : Expression(pos)
            , value(v)
        {}

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable>) const;
        bool isLiteral() const;
        bool boolValue() const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

        util::sptr<Expression const> operate(misc::position const& op_pos
                                           , std::string const& op_img
                                           , mpz_class const&) const;
        util::sptr<Expression const> operate(misc::position const& op_pos
                                           , std::string const& op_img
                                           , mpf_class const&) const;
        util::sptr<Expression const> operate(misc::position const& op_pos
                                           , std::string const& op_img
                                           , bool rhs) const;

        util::sptr<Expression const> asRhs(misc::position const& op_pos
                                         , std::string const& op_img
                                         , util::sptr<Expression const> lhs) const;
        util::sptr<Expression const> asRhs(misc::position const& op_pos
                                         , std::string const& op_img) const;

        bool const value;

        static util::sptr<Expression const> mkbool(bool value);
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

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable>) const;
        bool isLiteral() const;
        bool boolValue() const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

        util::sptr<Expression const> operate(misc::position const& op_pos
                                           , std::string const& op_img
                                           , mpz_class const& rhs) const;
        util::sptr<Expression const> operate(misc::position const& op_pos
                                           , std::string const& op_img
                                           , mpf_class const& rhs) const;
        util::sptr<Expression const> operate(misc::position const& op_pos
                                           , std::string const& op_img
                                           , bool) const;

        util::sptr<Expression const> asRhs(misc::position const& op_pos
                                         , std::string const& op_img
                                         , util::sptr<Expression const> lhs) const;
        util::sptr<Expression const> asRhs(misc::position const& op_pos
                                         , std::string const& op_img) const;

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

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable>) const;
        bool isLiteral() const;
        bool boolValue() const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

        util::sptr<Expression const> operate(misc::position const& op_pos
                                           , std::string const& op_img
                                           , mpz_class const& rhs) const;
        util::sptr<Expression const> operate(misc::position const& op_pos
                                           , std::string const& op_img
                                           , mpf_class const& rhs) const;
        util::sptr<Expression const> operate(misc::position const& op_pos
                                           , std::string const& op_img
                                           , bool) const;

        util::sptr<Expression const> asRhs(misc::position const& op_pos
                                         , std::string const& op_img
                                         , util::sptr<Expression const> lhs) const;
        util::sptr<Expression const> asRhs(misc::position const& op_pos
                                         , std::string const& op_img) const;

        mpf_class const value;
    };

    struct StringLiteral
        : Expression
    {
        StringLiteral(misc::position const& pos, std::string const& image)
            : Expression(pos)
            , value(image)
        {}

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable>) const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

        std::string const value;
    };

    struct ListLiteral
        : Expression
    {
        ListLiteral(misc::position const& pos, std::vector<util::sptr<Expression const>> v)
            : Expression(pos)
            , value(std::move(v))
        {}

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable> st) const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

        std::vector<util::sptr<Expression const>> const value;
    };

    struct ListElement
        : Expression
    {
        explicit ListElement(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable>) const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;
    };

    struct ListIndex
        : Expression
    {
        explicit ListIndex(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable>) const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;
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

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable> st) const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

        util::sptr<Expression const> const lhs;
        util::sptr<Expression const> const rhs;
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

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable> st) const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

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

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable> st) const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

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

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable> st) const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

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

            util::sptr<proto::Expression const> compile(util::sref<SymbolTable>) const;
            std::string typeName() const;
            util::sptr<Expression const> fold() const;
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

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable> st) const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

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

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable> st) const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

        std::vector<ItemType> const items;
    };

}

#endif /* __STEKIN_FLOWCHECK_EXPRESSION_NODES_H__ */
