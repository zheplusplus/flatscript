#include <semantic/function.h>
#include <report/errors.h>

#include "clauses.h"
#include "stmt-nodes.h"
#include "expr-nodes.h"
#include "stmt-automations.h"

using namespace grammar;

void ClauseBase::acceptFunc(util::sptr<Function const> func)
{
    _block.addFunc(std::move(func));
}

void ClauseBase::acceptClass(util::sptr<Class> cls)
{
    _block.addStmt(std::move(cls));
}

void ClauseBase::acceptCtor(misc::position const& ct_pos, std::vector<std::string>
                          , Block, bool, std::vector<util::sptr<Expression const>>)
{
    error::ctorNotAllowed(ct_pos);
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

void IfnotClause::deliver()
{
    misc::position pos(_predicate->pos);
    _parent->acceptStmt(util::mkptr(
                new BranchAlterOnly(pos, std::move(_predicate), std::move(_block))));
}

void FunctionClause::deliver()
{
    this->_parent->acceptFunc(util::mkptr(
                    new Function(pos, name, param_names, async_param_index, std::move(_block))));
}

void ClassClause::deliver()
{
    this->_parent->acceptClass(util::mkptr(new Class(
          pos, std::move(this->_class_name), std::move(this->_base_class)
        , std::move(this->_block))));
}

void ClassClause::acceptClass(util::sptr<Class> cls)
{
    error::nestedClassNotAllowed(cls->pos);
}

void ClassClause::acceptStmt(util::sptr<Statement> stmt)
{
    error::stmtNotAllowedInClass(stmt->pos);
}

void ClassClause::acceptCtor(
        misc::position const& ct_pos, std::vector<std::string> params
      , Block body, bool super_init, std::vector<util::sptr<Expression const>> super_ctor_args)
{
    this->_block.setCtor(ct_pos, std::move(params), std::move(body), super_init
                       , std::move(super_ctor_args));
}

void CtorClause::deliver()
{
    this->_parent->acceptCtor(this->pos, std::move(this->_params), std::move(this->_block)
                            , this->_super_init, std::move(this->_super_ctor_args));
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
