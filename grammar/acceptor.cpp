#include <flowcheck/node-base.h>
#include <report/errors.h>

#include "acceptor.h"
#include "stmt-nodes.h"
#include "function.h"

using namespace grammar;

void Acceptor::acceptElse(misc::position const& else_pos)
{
    error::elseNotMatchIf(else_pos);
}

void IfAcceptor::acceptFunc(util::sptr<Function const> func)
{
    _current_branch->addFunc(std::move(func));
}

void IfAcceptor::acceptStmt(util::sptr<Statement const> stmt)
{
    _current_branch->addStmt(std::move(stmt));
}

void IfAcceptor::deliverTo(util::sref<Acceptor> acc)
{
    if (_elseMatched()) {
        acc->acceptStmt(util::mkptr(new Branch(pos
                                             , std::move(_predicate)
                                             , std::move(_consequence)
                                             , std::move(_alternative))));
    } else {
        acc->acceptStmt(util::mkptr(new BranchConsqOnly(pos
                                                      , std::move(_predicate)
                                                      , std::move(_consequence))));
    }
}

void IfAcceptor::acceptElse(misc::position const& else_pos)
{
    if (_elseMatched()) {
        error::ifAlreadyMatchElse(_last_else_pos_or_nul_if_not_matched.cp(), else_pos);
    } else {
        _current_branch = &_alternative;
        _last_else_pos_or_nul_if_not_matched.reset(new misc::position(else_pos));
    }
}

bool IfAcceptor::_elseMatched() const
{
    return _last_else_pos_or_nul_if_not_matched.not_nul();
}

void IfnotAcceptor::acceptFunc(util::sptr<Function const> func)
{
    _alternative.addFunc(std::move(func));
}

void IfnotAcceptor::acceptStmt(util::sptr<Statement const> stmt)
{
    _alternative.addStmt(std::move(stmt));
}

void IfnotAcceptor::deliverTo(util::sref<Acceptor> acc)
{
    acc->acceptStmt(util::mkptr(new BranchAlterOnly(pos
                                                  , std::move(_predicate)
                                                  , std::move(_alternative))));
}

void FunctionAcceptor::acceptFunc(util::sptr<Function const> func)
{
    _body.addFunc(std::move(func));
}

void FunctionAcceptor::acceptStmt(util::sptr<Statement const> stmt)
{
    _body.addStmt(std::move(stmt));
}

void FunctionAcceptor::deliverTo(util::sref<Acceptor> acc)
{
    acc->acceptFunc(util::mkptr(new Function(pos, name, param_names, std::move(_body))));
}
