#ifndef __STEKIN_FLOWCHECK_NODE_BASE_H__
#define __STEKIN_FLOWCHECK_NODE_BASE_H__

#include <string>
#include <vector>
#include <gmpxx.h>

#include <proto/fwd-decl.h>
#include <util/pointer.h>
#include <misc/pos-type.h>

#include "fwd-decl.h"

namespace flchk {

    struct Statement {
        misc::position const pos;

        virtual util::sptr<proto::Statement const> compile(util::sref<SymbolTable> st) const = 0;

        virtual ~Statement() {}
    protected:
        explicit Statement(misc::position const& ps)
            : pos(ps)
        {}

        Statement(Statement const&) = delete;
    };

    struct Expression {
        misc::position const pos;
    public:
        virtual util::sptr<proto::Expression const> compile(util::sref<SymbolTable> st) const = 0;
        virtual std::string typeName() const = 0;
        virtual bool isLiteral() const;
        virtual bool boolValue() const;
        virtual util::sptr<Expression const> fold() const = 0;
    public:
        virtual util::sptr<Expression const> operate(misc::position const& op_pos
                                                   , std::string const& op_img
                                                   , mpz_class const& rhs) const;
        virtual util::sptr<Expression const> operate(misc::position const& op_pos
                                                   , std::string const& op_img
                                                   , mpf_class const& rhs) const;
        virtual util::sptr<Expression const> operate(misc::position const& op_pos
                                                   , std::string const& op_img
                                                   , bool rhs) const;

        virtual util::sptr<Expression const> asRhs(misc::position const& op_pos
                                                 , std::string const& op_img
                                                 , util::sptr<Expression const> lhs) const;
        virtual util::sptr<Expression const> asRhs(misc::position const& op_pos
                                                 , std::string const& op_img) const;
    public:
        virtual ~Expression() {}
    protected:
        Expression(misc::position const& ps)
            : pos(ps)
        {}

        Expression(Expression const&) = delete;
    };

}

#endif /* __STEKIN_FLOWCHECK_NODE_BASE_H__ */
