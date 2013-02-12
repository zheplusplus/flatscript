#ifndef __STEKIN_GRAMMAR_NODE_BASE_H__
#define __STEKIN_GRAMMAR_NODE_BASE_H__

#include <semantic/fwd-decl.h>
#include <util/pointer.h>
#include <misc/pos-type.h>

#include "reducing-env.h"
#include "fwd-decl.h"

namespace grammar {

    struct Statement {
        misc::position const pos;

        virtual void compile(util::sref<semantic::Filter> filter) const = 0;
        virtual void acceptElse(misc::position const& else_pos, Block&& block);

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
        virtual void reduceAsParam(ParamReducingEnv& env, int index) const;
        virtual std::string reduceAsName() const;
        virtual std::string reduceAsProperty() const;
        virtual util::sptr<semantic::Expression const> reduceAsExpr() const = 0;
        virtual util::sptr<semantic::Expression const> reduceAsLeftValue() const;
        virtual util::sptr<semantic::Expression const> reduceAsArg(
                                                    ArgReducingEnv& env, int index) const;

        explicit Expression(misc::position const& ps)
            : pos(ps)
        {}

        Expression(Expression const&) = delete;
    };

}

#endif /* __STEKIN_GRAMMAR_NODE_BASE_H__ */
