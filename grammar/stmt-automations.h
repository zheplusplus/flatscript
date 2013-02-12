#ifndef __STEKIN_GRAMMAR_STATEMENT_AUTOMATIONS_H__
#define __STEKIN_GRAMMAR_STATEMENT_AUTOMATIONS_H__

#include "automation-base.h"

namespace grammar {

    struct ExprStmtAutomation
        : AutomationBase
    {
        explicit ExprStmtAutomation(util::sref<ClauseBase> clause);

        void pushFactor(AutomationStack& stack
                      , util::sptr<Expression const> factor
                      , std::string const& image);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        bool finishOnBreak(bool sub_empty) const;
        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);
    private:
        void _pushColon(AutomationStack& stack, misc::position const& pos);
        util::sptr<Statement> _reduceAsStmt();

        std::vector<util::sptr<Expression const>> _exprs;
        util::sref<ClauseBase> const _clause;
        bool _before_colon;
    };

    struct IfAutomation
        : AutomationBase
    {
        void activated(AutomationStack& stack);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        bool finishOnBreak(bool sub_empty) const;
        void finish(ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&);

        IfAutomation()
            : _pred_cache(nullptr)
        {}
    private:
        util::sptr<Expression const> _pred_cache;
    };

    struct ElseAutomation
        : AutomationBase
    {
        explicit ElseAutomation(misc::position const& pos)
            : else_pos(pos)
        {}

        void accepted(AutomationStack&, util::sptr<Expression const>) {}
        bool finishOnBreak(bool) const { return true; }
        void finish(ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&);

        misc::position const else_pos;
    };

    struct FunctionAutomation
        : AutomationBase
    {
        FunctionAutomation()
            : _before_open_paren(true)
            , _finished(false)
            , _async_param_index(-1)
        {}

        void pushFactor(AutomationStack&
                      , util::sptr<Expression const> factor
                      , std::string const& image);
        void accepted(AutomationStack&, util::sptr<Expression const>) {};
        void accepted(AutomationStack&, std::vector<util::sptr<Expression const>> list);
        bool finishOnBreak(bool) const;
        void finish(ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&);
    private:
        bool _before_open_paren;
        bool _finished;
        misc::position _pos;
        std::string _func_name;
        std::vector<std::string> _params;
        int _async_param_index;
    };

    struct ExprReceiver
        : AutomationBase
    {
        void activated(AutomationStack& stack);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        bool finishOnBreak(bool) const { return true; }
        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);

        explicit ExprReceiver(util::sref<ClauseBase> clause)
            : _clause(clause)
            , _expr(nullptr)
        {}
    protected:
        util::sref<ClauseBase> const _clause;
        util::sptr<Expression const> _expr;
    };

    struct ReturnAutomation
        : ExprReceiver
    {
        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);

        explicit ReturnAutomation(util::sref<ClauseBase> clause)
            : ExprReceiver(clause)
        {}
    };

    struct ExportStmtAutomation
        : ExprReceiver
    {
        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);

        ExportStmtAutomation(util::sref<ClauseBase> clause, std::vector<std::string> const& ep)
            : ExprReceiver(clause)
            , export_point(ep)
        {}

        std::vector<std::string> const export_point;
    };

}

#endif /* __STEKIN_GRAMMAR_STATEMENT_AUTOMATIONS_H__ */
