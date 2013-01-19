#include <semantic/function.h>
#include <report/errors.h>

#include "stmt-automations.h"
#include "expr-automations.h"
#include "stmt-nodes.h"
#include "function.h"

using namespace grammar;

void ExprStmtAutomation::activated(AutomationStack& stack)
{
    stack.push(util::mkptr(new PipelineAutomation));
}

void ExprStmtAutomation::pushColon(AutomationStack& stack, misc::position const& pos)
{
    if (_exprs.size() < 2) {
        stack.push(util::mkptr(new PipelineAutomation));
        return;
    }
    error::unexpectedToken(pos, ":");
}

void ExprStmtAutomation::accepted(AutomationStack&, util::sptr<Expression const> expr)
{
    _exprs.push_back(std::move(expr));
}

bool ExprStmtAutomation::finishOnBreak(bool) const
{
    return true;
}

void ExprStmtAutomation::finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&)
{
    _clause->acceptStmt(_reduceAsStmt());
    stack.pop();
}

util::sptr<Statement> ExprStmtAutomation::_reduceAsStmt()
{
    misc::position pos(_exprs[0]->pos);
    if (_exprs.size() == 1) {
        return util::mkptr(new Arithmetics(pos, std::move(_exprs[0])));
    }
    if (_exprs[0]->isName()) {
        return util::mkptr(new NameDef(pos, _exprs[0]->reduceAsName(), std::move(_exprs[1])));
    }
    return util::mkptr(new AttrSet(pos, std::move(_exprs[0]), std::move(_exprs[1])));
}

void ExprReceiver::activated(AutomationStack& stack)
{
    stack.push(util::mkptr(new PipelineAutomation));
}

void ExprReceiver::accepted(AutomationStack&, util::sptr<Expression const> expr)
{
    _expr = std::move(expr);
}

bool ExprReceiver::finishOnBreak(bool) const
{
    return true;
}

void ExprReceiver::finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&)
{
    if (_expr->empty()) {
        error::invalidEmptyExpr(_expr->pos);
    }
    _clause->acceptExpr(std::move(_expr));
    stack.pop();
}

void ReturnAutomation::finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&)
{
    if (_expr->empty()) {
        _clause->acceptStmt(util::mkptr(new ReturnNothing(_expr->pos)));
    } else {
        _clause->acceptStmt(util::mkptr(new Return(_expr->pos, std::move(_expr))));
    }
    stack.pop();
}

void ExportStmtAutomation::finish(
                        ClauseStackWrapper&, AutomationStack& stack, misc::position const&)
{
    if (_expr->empty()) {
        error::invalidEmptyExpr(_expr->pos);
    }
    _clause->acceptStmt(util::mkptr(new Export(_expr->pos, export_point, std::move(_expr))));
    stack.pop();
}
