#ifndef __STEKIN_GRAMMAR_STATEMENT_NODES_H__
#define __STEKIN_GRAMMAR_STATEMENT_NODES_H__

#include <string>

#include <semantic/filter.h>
#include <util/pointer.h>

#include "node-base.h"
#include "block.h"

namespace grammar {

    struct Arithmetics
        : Statement
    {
        Arithmetics(misc::position const& pos, util::sptr<Expression const> e)
            : Statement(pos)
            , expr(std::move(e))
        {}

        void compile(util::sref<semantic::Filter> filter) const;

        util::sptr<Expression const> const expr;
    };

    struct Branch
        : Statement
    {
        Branch(misc::position const& pos, util::sptr<Expression const> p, Block c)
            : Statement(pos)
            , predicate(std::move(p))
            , consequence(std::move(c))
            , _alternative(nullptr)
        {}

        void compile(util::sref<semantic::Filter> filter) const;
        void acceptElse(misc::position const& else_pos, Block&& block);

        util::sptr<Expression const> const predicate;
        Block const consequence;
    private:
        util::sptr<Block const> _alternative;
        misc::position _else_pos;
    };

    struct BranchAlterOnly
        : Statement
    {
        BranchAlterOnly(misc::position const& pos, util::sptr<Expression const> p, Block a)
            : Statement(pos)
            , predicate(std::move(p))
            , alternative(std::move(a))
        {}

        void compile(util::sref<semantic::Filter> filter) const;

        util::sptr<Expression const> const predicate;
        Block const alternative;
    };

    struct Return
        : Statement
    {
        Return(misc::position const& pos, util::sptr<Expression const> r)
            : Statement(pos)
            , ret_val(std::move(r))
        {}

        void compile(util::sref<semantic::Filter> filter) const;

        util::sptr<Expression const> const ret_val;
    };

    struct ReturnNothing
        : Statement
    {
        explicit ReturnNothing(misc::position const& pos)
            : Statement(pos)
        {}

        void compile(util::sref<semantic::Filter> filter) const;
    };

    struct NameDef
        : Statement
    {
        NameDef(misc::position const& pos, std::string const& n, util::sptr<Expression const> i)
            : Statement(pos)
            , name(n)
            , init(std::move(i))
        {}

        void compile(util::sref<semantic::Filter> filter) const;

        std::string const name;
        util::sptr<Expression const> const init;
    };

    struct Import
        : Statement
    {
        Import(misc::position const& pos, std::vector<std::string> const& n)
            : Statement(pos)
            , names(n)
        {}

        void compile(util::sref<semantic::Filter> filter) const;

        std::vector<std::string> const names;
    };

    struct Export
        : Statement
    {
        Export(misc::position const& pos
             , std::vector<std::string> const e
             , util::sptr<Expression const> v)
                : Statement(pos)
                , export_point(std::move(e))
                , value(std::move(v))
        {}

        void compile(util::sref<semantic::Filter> filter) const;

        std::vector<std::string> const export_point;
        util::sptr<Expression const> const value;
    };

    struct AttrSet
        : Statement
    {
        AttrSet(misc::position const& pos
              , util::sptr<Expression const> s
              , util::sptr<Expression const> v)
            : Statement(pos)
            , set_point(std::move(s))
            , value(std::move(v))
        {}

        void compile(util::sref<semantic::Filter> filter) const;

        util::sptr<Expression const> const set_point;
        util::sptr<Expression const> const value;
    };

}

#endif /* __STEKIN_GRAMMAR_STATEMENT_NODES_H__ */
