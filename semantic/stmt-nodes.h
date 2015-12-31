#ifndef __STEKIN_SEMANTIC_STATEMENT_NODES_H__
#define __STEKIN_SEMANTIC_STATEMENT_NODES_H__

#include "node-base.h"

namespace semantic {

    struct Arithmetics
        : Statement
    {
        Arithmetics(misc::position const& pos, util::sptr<Expression const> e)
            : Statement(pos)
            , expr(std::move(e))
        {}

        void compile(util::sref<Scope> scope) const;
        bool isAsync() const;

        util::sptr<Expression const> const expr;
    };

    struct Branch
        : Statement
    {
        Branch(misc::position const& pos
             , util::sptr<Expression const> p
             , util::sptr<Statement const> c
             , util::sptr<Statement const> a)
                : Statement(pos)
                , predicate(std::move(p))
                , consequence(std::move(c))
                , alternative(std::move(a))
        {}

        void compile(util::sref<Scope> scope) const;
        bool isAsync() const;

        util::sptr<Expression const> const predicate;
        util::sptr<Statement const> const consequence;
        util::sptr<Statement const> const alternative;
    };

    struct Return
        : Statement
    {
        Return(misc::position const& pos, util::sptr<Expression const> retval)
            : Statement(pos)
            , ret_val(std::move(retval))
        {}

        void compile(util::sref<Scope> scope) const;
        bool isAsync() const;

        util::sptr<Expression const> const ret_val;
    };

    struct NameDef
        : Statement
    {
        NameDef(misc::position const& pos, std::string const& n, util::sptr<Expression const> i)
            : Statement(pos)
            , name(n)
            , init(std::move(i))
        {}

        void compile(util::sref<Scope> scope) const;
        bool isAsync() const;

        std::string const name;
        util::sptr<Expression const> const init;
    };

    struct Extern
        : Statement
    {
        Extern(misc::position const& pos, std::vector<std::string> const& n)
            : Statement(pos)
            , names(n)
        {}

        void compile(util::sref<Scope> scope) const;

        bool isAsync() const { return false; }

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

        void compile(util::sref<Scope> scope) const;
        bool isAsync() const;

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

        void compile(util::sref<Scope> scope) const;
        bool isAsync() const;

        util::sptr<Expression const> const set_point;
        util::sptr<Expression const> const value;
    };

    struct ExceptionStall
        : Statement
    {
        ExceptionStall(misc::position const& pos
                     , util::sptr<Statement const> fl
                     , util::sptr<Statement const> c)
            : Statement(pos)
            , try_block(std::move(fl))
            , catch_block(std::move(c))
        {}

        void compile(util::sref<Scope> scope) const;
        bool isAsync() const;

        util::sptr<Statement const> const try_block;
        util::sptr<Statement const> const catch_block;
    };

    struct Throw
        : Statement
    {
        Throw(misc::position const& pos, util::sptr<Expression const> e)
            : Statement(pos)
            , exception(std::move(e))
        {}

        void compile(util::sref<Scope> scope) const;

        bool isAsync() const { return false; }

        util::sptr<Expression const> const exception;
    };

    struct Break
        : Statement
    {
        explicit Break(misc::position const& pos)
            : Statement(pos)
        {}

        void compile(util::sref<Scope> scope) const;

        bool isAsync() const { return false; }
    };

    struct Continue
        : Statement
    {
        explicit Continue(misc::position const& pos)
            : Statement(pos)
        {}

        void compile(util::sref<Scope> scope) const;

        bool isAsync() const { return false; }
    };

}

#endif /* __STEKIN_SEMANTIC_STATEMENT_NODES_H__ */
