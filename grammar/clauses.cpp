#include <semantic/node-base.h>
#include <report/errors.h>

#include "clauses.h"
#include "stmt-nodes.h"
#include "function.h"
#include "stmt-automations.h"

using namespace grammar;

IfClause::IfClause(int indent_level, misc::position const& ps, util::sref<ClauseBase> parent)
    : ClauseBase(indent_level)
    , pos(ps)
    , _predicate(nullptr)
    , _last_else_pos_or_nul_if_not_matched(nullptr)
    , _current_branch(&_consequence)
    , _parent(parent)
{
    _stack.push(util::mkptr(new grammar::ExprReceiver(util::mkref(*this))));
}

void IfClause::acceptFunc(util::sptr<Function const> func)
{
    _current_branch->addFunc(std::move(func));
}

void IfClause::acceptStmt(util::sptr<Statement> stmt)
{
    _current_branch->addStmt(std::move(stmt));
}

void IfClause::deliver()
{
    if (_elseMatched()) {
        _parent->acceptStmt(util::mkptr(new Branch(pos
                                                 , std::move(_predicate)
                                                 , std::move(_consequence)
                                                 , std::move(_alternative))));
    } else {
        _parent->acceptStmt(util::mkptr(
                    new BranchConsqOnly(pos, std::move(_predicate), std::move(_consequence))));
    }
}

void IfClause::acceptElse(misc::position const& else_pos)
{
    if (_elseMatched()) {
        error::ifAlreadyMatchElse(_last_else_pos_or_nul_if_not_matched.cp(), else_pos);
    } else {
        _current_branch = &_alternative;
        _last_else_pos_or_nul_if_not_matched.reset(new misc::position(else_pos));
    }
}

void IfClause::acceptExpr(util::sptr<Expression const> expr)
{
    _predicate = std::move(expr);
}

bool IfClause::_elseMatched() const
{
    return _last_else_pos_or_nul_if_not_matched.not_nul();
}

IfnotClause::IfnotClause(int indent_level, misc::position const& ps, util::sref<ClauseBase> parent)
    : ClauseBase(indent_level)
    , pos(ps)
    , _predicate(nullptr)
    , _parent(parent)
{
    _stack.push(util::mkptr(new grammar::ExprReceiver(util::mkref(*this))));
}

void IfnotClause::acceptFunc(util::sptr<Function const> func)
{
    _alternative.addFunc(std::move(func));
}

void IfnotClause::acceptStmt(util::sptr<Statement> stmt)
{
    _alternative.addStmt(std::move(stmt));
}

void IfnotClause::deliver()
{
    _parent->acceptStmt(util::mkptr(
                new BranchAlterOnly(pos, std::move(_predicate), std::move(_alternative))));
}

void IfnotClause::acceptExpr(util::sptr<Expression const> expr)
{
    _predicate = std::move(expr);
}

void FunctionClause::acceptFunc(util::sptr<Function const> func)
{
    _body.addFunc(std::move(func));
}

void FunctionClause::acceptStmt(util::sptr<Statement> stmt)
{
    _body.addStmt(std::move(stmt));
}

void FunctionClause::deliver()
{
    _parent->acceptFunc(util::mkptr(new Function(pos, name, param_names, std::move(_body))));
}
