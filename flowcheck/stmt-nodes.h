#ifndef __STEKIN_FLOWCHECK_STATEMENT_NODES_H__
#define __STEKIN_FLOWCHECK_STATEMENT_NODES_H__

#include <string>
#include <list>

#include "node-base.h"
#include "block.h"
#include <util/pointer.h>

namespace flchk {

    struct Arithmetics
        : public Statement
    {
        Arithmetics(misc::position const& pos, util::sptr<Expression const> e)
            : Statement(pos)
            , expr(std::move(e))
        {}

        util::sptr<proto::Statement const> compile(util::sref<SymbolTable> st) const;

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

        util::sptr<proto::Statement const> compile(util::sref<SymbolTable> st) const;

        util::sptr<Expression const> const predicate;
        Block const consequence;
        Block const alternative;
    };

    struct Return
        : public Statement
    {
        Return(misc::position const& pos, util::sptr<Expression const> retval)
            : Statement(pos)
            , ret_val(std::move(retval))
        {}

        util::sptr<proto::Statement const> compile(util::sref<SymbolTable> st) const;

        util::sptr<Expression const> const ret_val;
    };

    struct ReturnNothing
        : public Statement
    {
        explicit ReturnNothing(misc::position const& pos)
            : Statement(pos)
        {}

        util::sptr<proto::Statement const> compile(util::sref<SymbolTable> st) const;
    };

    struct VarDef
        : public Statement
    {
        VarDef(misc::position const& pos, std::string const& n, util::sptr<Expression const> i)
                : Statement(pos)
                , name(n)
                , init(std::move(i))
        {}

        util::sptr<proto::Statement const> compile(util::sref<SymbolTable> st) const;

        std::string const name;
        util::sptr<Expression const> const init;
    };

}

#endif /* __STEKIN_FLOWCHECK_STATEMENT_NODES_H__ */
