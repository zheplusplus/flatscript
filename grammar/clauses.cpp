#include <semantic/function.h>
#include <report/errors.h>

#include "clauses.h"
#include "stmt-nodes.h"
#include "stmt-automations.h"

using namespace grammar;

void ClauseBase::acceptFunc(util::sptr<Function const> func)
{
    _block.addFunc(std::move(func));
}

void ClauseBase::acceptClass(util::sptr<Class const> cls)
{
    _block.addClass(std::move(cls));
}

void ClauseBase::acceptCtor(misc::position const& ct_pos, std::vector<std::string> params, Block body)
{
    _block.setCtor(ct_pos, std::move(params), std::move(body));
}

void ClauseBase::acceptStmt(util::sptr<Statement> stmt)
{
    _block.addStmt(std::move(stmt));
}

void ClauseBase::acceptElse(misc::position const& else_pos, Block block)
{
    _block.acceptElse(else_pos, std::move(block));
}

void ClauseBase::acceptCatch(misc::position const& catch_pos, Block block)
{
    _block.acceptCatch(catch_pos, std::move(block));
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

void ClauseBase::prepareThrow()
{
    _stack.push(util::mkptr(new ThrowAutomation(util::mkref(*this))));
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

void IfnotClause::deliver()
{
    misc::position pos(_predicate->pos);
    _parent->acceptStmt(util::mkptr(
                new BranchAlterOnly(pos, std::move(_predicate), std::move(_block))));
}

void FunctionClause::deliver()
{
    return _parent->acceptFunc(util::mkptr(
                    new Function(pos, name, param_names, async_param_index, std::move(_block))));
}

void ClassClause::deliver()
{
    _parent->acceptClass(util::mkptr(
            new Class(pos, _class_name, _base_class_name, std::move(_block))));
}

void ClassClause::acceptClass(util::sptr<Class const> cls)
{
    error::nestedClassNotAllowed(cls->pos);
}

void ClassClause::acceptStmt(util::sptr<Statement> stmt)
{
    error::stmtNotAllowedInClass(stmt->pos);
}

void TryClause::deliver()
{
    _parent->acceptStmt(util::mkptr(new ExceptionStall(_try_pos, std::move(_block))));
}

void CatchClause::deliver()
{
    _parent->acceptCatch(catch_pos, std::move(_block));
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
