#ifndef __STEKIN_GRAMMAR_STATEMENT_NODES_H__
#define __STEKIN_GRAMMAR_STATEMENT_NODES_H__

#include <string>
#include <list>

#include <util/pointer.h>

#include "node-base.h"
#include "block.h"

namespace grammar {

    struct Arithmetics
        : public Statement
    {
        Arithmetics(misc::position const& pos, util::sptr<Expression const> e)
            : Statement(pos)
            , expr(std::move(e))
        {}

        void compile(util::sref<flchk::Filter> filter) const;

        util::sptr<Expression const> const expr;
    };

    struct Branch
        : public Statement
    {
        Branch(misc::position const& pos, util::sptr<Expression const> p, Block c, Block a)
            : Statement(pos)
            , predicate(std::move(p))
            , consequence(std::move(c))
            , alternative(std::move(a))
        {}

        void compile(util::sref<flchk::Filter> filter) const;

        util::sptr<Expression const> const predicate;
        Block const consequence;
        Block const alternative;
    };

    struct BranchConsqOnly
        : public Statement
    {
        BranchConsqOnly(misc::position const& pos, util::sptr<Expression const> p, Block c)
            : Statement(pos)
            , predicate(std::move(p))
            , consequence(std::move(c))
        {}

        void compile(util::sref<flchk::Filter> filter) const;

        util::sptr<Expression const> const predicate;
        Block const consequence;
    };

    struct BranchAlterOnly
        : public Statement
    {
        BranchAlterOnly(misc::position const& pos, util::sptr<Expression const> p, Block a)
            : Statement(pos)
            , predicate(std::move(p))
            , alternative(std::move(a))
        {}

        void compile(util::sref<flchk::Filter> filter) const;

        util::sptr<Expression const> const predicate;
        Block const alternative;
    };

    struct Return
        : public Statement
    {
        Return(misc::position const& pos, util::sptr<Expression const> r)
            : Statement(pos)
            , ret_val(std::move(r))
        {}

        void compile(util::sref<flchk::Filter> filter) const;

        util::sptr<Expression const> const ret_val;
    };

    struct ReturnNothing
        : public Statement
    {
        explicit ReturnNothing(misc::position const& pos)
            : Statement(pos)
        {}

        void compile(util::sref<flchk::Filter> filter) const;
    };

    struct VarDef
        : public Statement
    {
        VarDef(misc::position const& pos, std::string const& n, util::sptr<Expression const> i)
            : Statement(pos)
            , name(n)
            , init(std::move(i))
        {}

        void compile(util::sref<flchk::Filter> filter) const;

        std::string const name;
        util::sptr<Expression const> const init;
    };

}

#endif /* __STEKIN_GRAMMAR_STATEMENT_NODES_H__ */
