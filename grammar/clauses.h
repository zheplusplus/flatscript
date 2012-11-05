#ifndef __STEKIN_GRAMMAR_CLAUSE_H__
#define __STEKIN_GRAMMAR_CLAUSE_H__

#include <util/pointer.h>
#include <misc/pos-type.h>

#include "node-base.h"
#include "automation-base.h"
#include "function.h"
#include "expr-tokens.h"

namespace grammar {

    struct IfClause
        : ClauseBase
    {
        void acceptFunc(util::sptr<Function const> func);
        void acceptStmt(util::sptr<Statement> stmt);
        void deliverTo(util::sref<ClauseBase> acc);
        void acceptElse(misc::position const& else_pos);
        void acceptExpr(util::sptr<Expression const> expr);

        IfClause(int indent_level, misc::position const& pos);

        misc::position const pos;
    private:
        bool _elseMatched() const;

        util::sptr<Expression const> _predicate;

        util::sptr<misc::position> _last_else_pos_or_nul_if_not_matched;
        Block* _current_branch;

        Block _consequence;
        Block _alternative;
    };

    struct IfnotClause
        : ClauseBase
    {
        void acceptFunc(util::sptr<Function const> func);
        void acceptStmt(util::sptr<Statement> stmt);
        void deliverTo(util::sref<ClauseBase> acc);
        void acceptExpr(util::sptr<Expression const> expr);

        IfnotClause(int indent_level, misc::position const& pos);

        misc::position const pos;
    private:
        util::sptr<Expression const> _predicate;
        Block _alternative;
    };

    struct FunctionClause
        : ClauseBase
    {
        void acceptFunc(util::sptr<Function const> func);
        void acceptStmt(util::sptr<Statement> stmt);
        void deliverTo(util::sref<ClauseBase> acc);

        FunctionClause(int indent_level
                     , misc::position const& ps
                     , std::string const& func_name
                     , std::vector<std::string> const& params)
            : ClauseBase(indent_level)
            , pos(ps)
            , name(func_name)
            , param_names(params)
        {}

        misc::position const pos;
        std::string const name;
        std::vector<std::string> const param_names;
    private:
        Block _body;
    };

}

#endif /* __STEKIN_GRAMMAR_CLAUSE_H__ */
