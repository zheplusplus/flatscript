#ifndef __STEKIN_GRAMMAR_NODE_BASE_H__
#define __STEKIN_GRAMMAR_NODE_BASE_H__

#include <util/pointer.h>
#include <misc/pos-type.h>
#include <semantic/node-base.h>

#include "reducing-env.h"

namespace grammar {

    struct Statement {
        misc::position const pos;

        virtual util::sptr<semantic::Statement const> compile() const = 0;
        virtual void acceptElse(misc::position const& else_pos, util::sptr<Statement const> block);
        virtual void acceptCatch(misc::position const& catch_pos, util::sptr<Statement const> block);

        virtual ~Statement() {}

        explicit Statement(misc::position const& ps)
            : pos(ps)
        {}

        Statement(Statement const&) = delete;
    };

    struct Expression {
        misc::position const pos;

        virtual ~Expression() {}

        virtual bool empty() const { return false; }
        virtual bool isName() const { return false; }
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
