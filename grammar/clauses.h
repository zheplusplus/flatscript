#ifndef __STEKIN_GRAMMAR_CLAUSE_H__
#define __STEKIN_GRAMMAR_CLAUSE_H__

#include <util/pointer.h>
#include <misc/pos-type.h>

#include "node-base.h"
#include "automation-base.h"
#include "function.h"
#include "tokens.h"

namespace grammar {

    struct ClauseBase {
        explicit ClauseBase(int ind)
            : indent(ind)
            , _member_indent(-1)
        {}

        ClauseBase(ClauseBase const&) = delete;
        virtual ~ClauseBase() {}

        int const indent;

        void acceptFunc(util::sptr<Function const> func);
        void acceptStmt(util::sptr<Statement> stmt);
        void acceptElse(misc::position const& else_pos, Block&& block);

        virtual void acceptExpr(util::sptr<Expression const>) {}
        virtual void deliver() = 0;
        virtual bool shrinkOn(int level) const;

        void nextToken(util::sptr<Token> const& token);
        bool tryFinish(misc::position const& pos, std::vector<util::sptr<ClauseBase>>& clauses);
        void prepareArith();
        void prepareReturn();
        void prepareExport(std::vector<std::string> const& names);

        void setMemberIndent(int level, misc::position const& pos);
    protected:
        AutomationStack _stack;
        int _member_indent;
        Block _block;
    };

    struct IfClause
        : ClauseBase
    {
        void deliver();

        IfClause(int indent_len, util::sptr<Expression const> pred, util::sref<ClauseBase> parent)
            : ClauseBase(indent_len)
            , _predicate(std::move(pred))
            , _parent(parent)
        {}
    private:
        util::sptr<Expression const> _predicate;
        util::sref<ClauseBase> const _parent;
    };

    struct ElseClause
        : ClauseBase
    {
        ElseClause(int indent_len, misc::position const& pos, util::sref<ClauseBase> parent)
            : ClauseBase(indent_len)
            , else_pos(pos)
            , _parent(parent)
        {}

        void deliver();

        misc::position const else_pos;
    private:
        util::sref<ClauseBase> const _parent;
    };

    struct IfnotClause
        : ClauseBase
    {
        void deliver();
        void acceptExpr(util::sptr<Expression const> expr);

        IfnotClause(int indent_level, misc::position const& pos, util::sref<ClauseBase> parent);

        misc::position const pos;
    private:
        util::sptr<Expression const> _predicate;
        util::sref<ClauseBase> const _parent;
    };

    struct FunctionClause
        : ClauseBase
    {
        void deliver();

        FunctionClause(int indent_level
                     , misc::position const& ps
                     , std::string const& func_name
                     , std::vector<std::string> const& params
                     , int async_param_idx
                     , util::sref<ClauseBase> parent)
            : ClauseBase(indent_level)
            , pos(ps)
            , name(func_name)
            , param_names(params)
            , async_param_index(async_param_idx)
            , _parent(parent)
        {}

        misc::position const pos;
        std::string const name;
        std::vector<std::string> const param_names;
        int const async_param_index;
    private:
        util::sref<ClauseBase> const _parent;
    };

    struct BlockReceiverClause
        : ClauseBase
    {
        BlockReceiverClause(int level
                          , AutomationStack& stack
                          , misc::position const& pos
                          , util::sref<AutomationBase> blockRecr)
            : ClauseBase(level)
            , _stack(stack)
            , _pos(pos)
            , _blockRecr(blockRecr)
        {}

        void deliver();
        bool shrinkOn(int level) const;
    private:
        AutomationStack& _stack;
        misc::position const _pos;
        util::sref<AutomationBase> _blockRecr;
    };

}

#endif /* __STEKIN_GRAMMAR_CLAUSE_H__ */
