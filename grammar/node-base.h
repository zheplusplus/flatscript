#ifndef __STEKIN_GRAMMAR_NODE_BASE_H__
#define __STEKIN_GRAMMAR_NODE_BASE_H__

#include <semantic/fwd-decl.h>
#include <util/pointer.h>
#include <misc/pos-type.h>

namespace grammar {

    struct Statement {
        misc::position const pos;

        virtual void compile(util::sref<semantic::Filter> filter) const = 0;

        virtual ~Statement() {}

        explicit Statement(misc::position const& ps)
            : pos(ps)
        {}

        Statement(Statement const&) = delete;
    };

    struct Expression {
        misc::position const pos;

        virtual ~Expression() {}

        virtual bool empty() const;
        virtual bool isName() const;
        virtual std::string reduceAsName() const;
        virtual util::sptr<semantic::Expression const> reduceAsExpr() const = 0;
        virtual util::sptr<semantic::Expression const> reduceAsLeftValue() const;

        explicit Expression(misc::position const& ps)
            : pos(ps)
        {}

        Expression(Expression const&) = delete;
    };

}

#endif /* __STEKIN_GRAMMAR_NODE_BASE_H__ */
