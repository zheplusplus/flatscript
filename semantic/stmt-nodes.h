#ifndef __STEKIN_SEMANTIC_STATEMENT_NODES_H__
#define __STEKIN_SEMANTIC_STATEMENT_NODES_H__

#include <string>

#include "node-base.h"
#include "block.h"
#include <util/pointer.h>

namespace semantic {

    struct Arithmetics
        : Statement
    {
        Arithmetics(misc::position const& pos, util::sptr<Expression const> e)
            : Statement(pos)
            , expr(std::move(e))
        {}

        void compile(CompilingSpace& space) const;

        util::sptr<Expression const> const expr;
    };

    struct Branch
        : Statement
    {
        Branch(misc::position const& pos, util::sptr<Expression const> p, Block c, Block a)
            : Statement(pos)
            , predicate(std::move(p))
            , consequence(std::move(c))
            , alternative(std::move(a))
        {}

        void compile(CompilingSpace& space) const;

        util::sptr<Expression const> const predicate;
        Block const consequence;
        Block const alternative;
    };

    struct Return
        : Statement
    {
        Return(misc::position const& pos, util::sptr<Expression const> retval)
            : Statement(pos)
            , ret_val(std::move(retval))
        {}

        void compile(CompilingSpace& space) const;

        util::sptr<Expression const> const ret_val;
    };

    struct ReturnNothing
        : Statement
    {
        explicit ReturnNothing(misc::position const& pos)
            : Statement(pos)
        {}

        void compile(CompilingSpace& space) const;
    };

    struct NameDef
        : Statement
    {
        NameDef(misc::position const& pos, std::string const& n, util::sptr<Expression const> i)
            : Statement(pos)
            , name(n)
            , init(std::move(i))
        {}

        void compile(CompilingSpace& space) const;

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

        void compile(CompilingSpace& space) const;

        std::vector<std::string> const names;
    };

    struct Export
        : Statement
    {
        Export(misc::position const& pos
             , std::vector<std::string> const& e
             , util::sptr<Expression const> v)
                : Statement(pos)
                , export_point(std::move(e))
                , value(std::move(v))
        {}

        void compile(CompilingSpace& space) const;

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

        void compile(CompilingSpace& space) const;

        util::sptr<Expression const> const set_point;
        util::sptr<Expression const> const value;
    };

}

#endif /* __STEKIN_SEMANTIC_STATEMENT_NODES_H__ */
