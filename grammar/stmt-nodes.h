#ifndef __STEKIN_GRAMMAR_STATEMENT_NODES_H__
#define __STEKIN_GRAMMAR_STATEMENT_NODES_H__

#include <string>
#include <list>

#include <flowcheck/fwd-decl.h>
#include <util/pointer.h>

#include "node-base.h"
#include "block.h"

namespace grammar {

    struct Arithmetics
        : Statement
    {
        Arithmetics(misc::position const& pos, util::sptr<flchk::Expression const> e)
            : Statement(pos)
            , expr(std::move(e))
        {}

        void compile(util::sref<flchk::Filter> filter);

        util::sptr<flchk::Expression const> expr;
    };

    struct Branch
        : Statement
    {
        Branch(misc::position const& pos, util::sptr<flchk::Expression const> p, Block c, Block a)
            : Statement(pos)
            , predicate(std::move(p))
            , consequence(std::move(c))
            , alternative(std::move(a))
        {}

        void compile(util::sref<flchk::Filter> filter);

        util::sptr<flchk::Expression const> predicate;
        Block const consequence;
        Block const alternative;
    };

    struct BranchConsqOnly
        : Statement
    {
        BranchConsqOnly(misc::position const& pos, util::sptr<flchk::Expression const> p, Block c)
            : Statement(pos)
            , predicate(std::move(p))
            , consequence(std::move(c))
        {}

        void compile(util::sref<flchk::Filter> filter);

        util::sptr<flchk::Expression const> predicate;
        Block const consequence;
    };

    struct BranchAlterOnly
        : Statement
    {
        BranchAlterOnly(misc::position const& pos, util::sptr<flchk::Expression const> p, Block a)
            : Statement(pos)
            , predicate(std::move(p))
            , alternative(std::move(a))
        {}

        void compile(util::sref<flchk::Filter> filter);

        util::sptr<flchk::Expression const> predicate;
        Block const alternative;
    };

    struct Return
        : Statement
    {
        Return(misc::position const& pos, util::sptr<flchk::Expression const> r)
            : Statement(pos)
            , ret_val(std::move(r))
        {}

        void compile(util::sref<flchk::Filter> filter);

        util::sptr<flchk::Expression const> ret_val;
    };

    struct ReturnNothing
        : Statement
    {
        explicit ReturnNothing(misc::position const& pos)
            : Statement(pos)
        {}

        void compile(util::sref<flchk::Filter> filter);
    };

    struct NameDef
        : Statement
    {
        NameDef(misc::position const& pos
              , std::string const& n
              , util::sptr<flchk::Expression const> i)
            : Statement(pos)
            , name(n)
            , init(std::move(i))
        {}

        void compile(util::sref<flchk::Filter> filter);

        std::string const name;
        util::sptr<flchk::Expression const> init;
    };

    struct Import
        : Statement
    {
        Import(misc::position const& pos, std::vector<std::string> const& n)
            : Statement(pos)
            , names(n)
        {}

        void compile(util::sref<flchk::Filter> filter);

        std::vector<std::string> const names;
    };

    struct AttrSet
        : Statement
    {
        AttrSet(misc::position const& pos
              , util::sptr<flchk::Expression const> s
              , util::sptr<flchk::Expression const> v)
            : Statement(pos)
            , set_point(std::move(s))
            , value(std::move(v))
        {}

        void compile(util::sref<flchk::Filter> filter);

        util::sptr<flchk::Expression const> set_point;
        util::sptr<flchk::Expression const> value;
    };

}

#endif /* __STEKIN_GRAMMAR_STATEMENT_NODES_H__ */
