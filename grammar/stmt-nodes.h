#ifndef __STEKIN_GRAMMAR_STATEMENT_NODES_H__
#define __STEKIN_GRAMMAR_STATEMENT_NODES_H__

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

        util::sptr<semantic::Statement const> compile() const;

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

        util::sptr<semantic::Statement const> compile() const;
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

        util::sptr<semantic::Statement const> compile() const;

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

        util::sptr<semantic::Statement const> compile() const;

        util::sptr<Expression const> const ret_val;
    };

    struct ReturnNothing
        : Statement
    {
        explicit ReturnNothing(misc::position const& pos)
            : Statement(pos)
        {}

        util::sptr<semantic::Statement const> compile() const;
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
        Extern(misc::position const& pos, std::vector<std::string> const& n)
            : Statement(pos)
            , names(n)
        {}

        util::sptr<semantic::Statement const> compile() const;

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
        ExceptionStall(misc::position const& pos, Block f)
            : Statement(pos)
            , flow(std::move(f))
            , _catch(nullptr)
        {}

        util::sptr<semantic::Statement const> compile() const;
        void acceptCatch(misc::position const& catch_pos, Block&& block);

        Block const flow;
    private:
        util::sptr<Block const> _catch;
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

}

#endif /* __STEKIN_GRAMMAR_STATEMENT_NODES_H__ */
