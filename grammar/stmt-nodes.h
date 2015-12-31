#ifndef __STEKIN_GRAMMAR_STATEMENT_NODES_H__
#define __STEKIN_GRAMMAR_STATEMENT_NODES_H__

#include "node-base.h"

namespace grammar {

    struct Arithmetics
        : Statement
    {
        Arithmetics(misc::position const& pos, util::sptr<Expression const> e)
            : Statement(pos)
            , expr(std::move(e))
        {}

        util::sptr<semantic::Statement const> compile() const;

        util::sptr<Expression const> const expr;
    };

    struct Branch
        : Statement
    {
        Branch(misc::position const& pos, util::sptr<Expression const> p, util::sptr<Statement const> c)
            : Statement(pos)
            , predicate(std::move(p))
            , consequence(std::move(c))
            , _alternative(nullptr)
        {}

        util::sptr<semantic::Statement const> compile() const;
        void acceptElse(misc::position const& else_pos, util::sptr<Statement const> block);

        util::sptr<Expression const> const predicate;
        util::sptr<Statement const> const consequence;
    private:
        util::sptr<Statement const> _alternative;
        misc::position _else_pos;
    };

    struct BranchAlterOnly
        : Statement
    {
        BranchAlterOnly(misc::position const& pos, util::sptr<Expression const> p, util::sptr<Statement const> a)
            : Statement(pos)
            , predicate(std::move(p))
            , alternative(std::move(a))
        {}

        util::sptr<semantic::Statement const> compile() const;

        util::sptr<Expression const> const predicate;
        util::sptr<Statement const> const alternative;
    };

    struct Return
        : Statement
    {
        Return(misc::position const& pos, util::sptr<Expression const> r)
            : Statement(pos)
            , ret_val(std::move(r))
        {}

        util::sptr<semantic::Statement const> compile() const;

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

        util::sptr<semantic::Statement const> compile() const;

        std::string const name;
        util::sptr<Expression const> const init;
    };

    struct Extern
        : Statement
    {
        Extern(misc::position const& pos, std::vector<std::string> n)
            : Statement(pos)
            , names(std::move(n))
        {}

        util::sptr<semantic::Statement const> compile() const;

        std::vector<std::string> const names;
    };

    struct Export
        : Statement
    {
        Export(misc::position const& pos
             , std::vector<std::string> e
             , util::sptr<Expression const> v)
                : Statement(pos)
                , export_point(std::move(e))
                , value(std::move(v))
        {}

        util::sptr<semantic::Statement const> compile() const;

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

        util::sptr<semantic::Statement const> compile() const;

        util::sptr<Expression const> const set_point;
        util::sptr<Expression const> const value;
    };

    struct ExceptionStall
        : Statement
    {
        ExceptionStall(misc::position const& pos, util::sptr<Statement const> f)
            : Statement(pos)
            , flow(std::move(f))
            , _catch(nullptr)
        {}

        util::sptr<semantic::Statement const> compile() const;
        void acceptCatch(misc::position const& catch_pos, util::sptr<Statement const> block);

        util::sptr<Statement const> const flow;
    private:
        util::sptr<Statement const> _catch;
        misc::position _catch_pos;
    };

    struct Throw
        : Statement
    {
        Throw(misc::position const& pos, util::sptr<Expression const> e)
            : Statement(pos)
            , exception(std::move(e))
        {}

        util::sptr<semantic::Statement const> compile() const;

        util::sptr<Expression const> const exception;
    };

    struct Break
        : Statement
    {
        explicit Break(misc::position const& pos)
            : Statement(pos)
        {}

        util::sptr<semantic::Statement const> compile() const;
    };

    struct Continue
        : Statement
    {
        explicit Continue(misc::position const& pos)
            : Statement(pos)
        {}

        util::sptr<semantic::Statement const> compile() const;
    };

    struct RangeIteration
        : Statement
    {
        RangeIteration(misc::position const& pos
                     , std::string r
                     , util::sptr<Expression const> b
                     , util::sptr<Expression const> e
                     , util::sptr<Expression const> s
                     , util::sptr<Statement const> lp)
            : Statement(pos)
            , reference(std::move(r))
            , begin(std::move(b))
            , end(std::move(e))
            , step(std::move(s))
            , loop(std::move(lp))
        {}

        util::sptr<semantic::Statement const> compile() const;

        std::string const reference;
        util::sptr<Expression const> const begin;
        util::sptr<Expression const> const end;
        util::sptr<Expression const> const step;
        util::sptr<Statement const> loop;
    };

}

#endif /* __STEKIN_GRAMMAR_STATEMENT_NODES_H__ */
