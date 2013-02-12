#include <semantic/function.h>
#include <report/errors.h>

#include "clauses.h"
#include "stmt-nodes.h"
#include "function.h"
#include "stmt-automations.h"

using namespace grammar;

void ClauseBase::acceptFunc(util::sptr<Function const> func)
{
    _block.addFunc(std::move(func));
}

void ClauseBase::acceptStmt(util::sptr<Statement> stmt)
{
    _block.addStmt(std::move(stmt));
}

void ClauseBase::acceptElse(misc::position const& else_pos, Block&& block)
{
    _block.acceptElse(else_pos, std::move(block));
}

bool ClauseBase::shrinkOn(int level) const
{
    return level <= indent;
}

void ClauseBase::nextToken(util::sptr<Token> const& token)
{
    token->act(_stack);
}

bool ClauseBase::tryFinish(misc::position const& pos, std::vector<util::sptr<ClauseBase>>& clauses)
{
    if ((!_stack.empty()) && _stack.top()->finishOnBreak(true)) {
        ClauseStackWrapper wrapper(_member_indent, _stack, pos, clauses);
        _stack.top()->finish(wrapper, _stack, pos);
        return true;
    }
    return _stack.empty();
}

void ClauseBase::prepareArith()
{
    _stack.push(util::mkptr(new ExprStmtAutomation(util::mkref(*this))));
}

void ClauseBase::prepareReturn()
{
    _stack.push(util::mkptr(new ReturnAutomation(util::mkref(*this))));
}

void ClauseBase::prepareExport(std::vector<std::string> const& names)
{
    _stack.push(util::mkptr(new ExportStmtAutomation(util::mkref(*this), names)));
}

void ClauseBase::setMemberIndent(int level, misc::position const& pos)
{
    if (-1 == _member_indent) {
        _member_indent = level;
        return;
    }
    if (level != _member_indent) {
        error::invalidIndent(pos);
    }
}

void IfClause::deliver()
{
    misc::position pos(_predicate->pos);
    _parent->acceptStmt(util::mkptr(new Branch(pos, std::move(_predicate), std::move(_block))));
}

void ElseClause::deliver()
{
    _parent->acceptElse(else_pos, std::move(_block));
}

IfnotClause::IfnotClause(int indent_level, misc::position const& ps, util::sref<ClauseBase> parent)
    : ClauseBase(indent_level)
    , pos(ps)
    , _predicate(nullptr)
    , _parent(parent)
{
    _stack.push(util::mkptr(new ExprReceiver(util::mkref(*this))));
}

void IfnotClause::deliver()
{
    _parent->acceptStmt(util::mkptr(
                new BranchAlterOnly(pos, std::move(_predicate), std::move(_block))));
}

void IfnotClause::acceptExpr(util::sptr<Expression const> expr)
{
    _predicate = std::move(expr);
}

void FunctionClause::deliver()
{
    return _parent->acceptFunc(util::mkptr(
                    new Function(pos, name, param_names, async_param_index, std::move(_block))));
}

void BlockReceiverClause::deliver()
{
    _blockRecr->accepted(_stack, _pos, std::move(_block));
}

bool BlockReceiverClause::shrinkOn(int level) const
{
    if (_member_indent == -1) {
        return ClauseBase::shrinkOn(level);
    }
    return level < _member_indent;
}
