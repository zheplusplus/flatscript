#ifndef __STEKIN_GRAMMAR_STATEMENT_AUTOMATIONS_H__
#define __STEKIN_GRAMMAR_STATEMENT_AUTOMATIONS_H__

#include "automation-base.h"

namespace grammar {

    struct ExprStmtAutomation
        : AutomationBase
    {
        void activated(AutomationStack& stack);
        void pushColon(AutomationStack& stack, misc::position const&);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        bool finishOnBreak(bool) const;
        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);

        explicit ExprStmtAutomation(util::sref<ClauseBase> acc)
            : _clause(acc)
        {}
    private:
        util::sptr<Statement> _reduceAsStmt();

        std::vector<util::sptr<Expression const>> _exprs;
        util::sref<ClauseBase> const _clause;
    };

    struct ExprReceiver
        : AutomationBase
    {
        void activated(AutomationStack& stack);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        bool finishOnBreak(bool) const;
        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);

        explicit ExprReceiver(util::sref<ClauseBase> acc)
            : _clause(acc)
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

        explicit ReturnAutomation(util::sref<ClauseBase> acc)
            : ExprReceiver(acc)
        {}
    };

    struct ExportStmtAutomation
        : ExprReceiver
    {
        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);

        ExportStmtAutomation(util::sref<ClauseBase> acc, std::vector<std::string> const& ep)
            : ExprReceiver(acc)
            , export_point(ep)
        {}

        std::vector<std::string> const export_point;
    };

}

#endif /* __STEKIN_GRAMMAR_STATEMENT_AUTOMATIONS_H__ */
