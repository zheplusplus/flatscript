#include <output/block.h>
#include <output/function.h>
#include <report/errors.h>
#include <report/warnings.h>

#include "filter.h"
#include "stmt-nodes.h"
#include "expr-nodes.h"
#include "function.h"
#include "compiling-space.h"

using namespace semantic;

void Filter::addReturn(misc::position const& pos, util::sptr<Expression const> ret_val)
{
    _checkNotTerminated(pos);
    _setTerminated(pos);
    _block.addStmt(util::mkptr(new Return(pos, std::move(ret_val))));
}

void Filter::addReturnNothing(misc::position const& pos)
{
    _checkNotTerminated(pos);
    _setTerminated(pos);
    _block.addStmt(util::mkptr(new Return(pos, util::mkptr(new Undefined(pos)))));
}

void Filter::addArith(misc::position const& pos, util::sptr<Expression const> expr)
{
    _checkNotTerminated(pos);
    _block.addStmt(util::mkptr(new Arithmetics(pos, std::move(expr))));
}

void Filter::addImport(misc::position const& pos, std::vector<std::string> const& names)
{
    _checkNotTerminated(pos);
    _block.addStmt(util::mkptr(new Import(pos, names)));
}

void Filter::addExport(misc::position const& pos
                     , std::vector<std::string> const& export_point
                     , util::sptr<Expression const> value)
{
    _checkNotTerminated(pos);
    _block.addStmt(util::mkptr(new Export(pos, export_point, std::move(value))));
}

void Filter::addAttrSet(misc::position const& pos
                      , util::sptr<Expression const> set_point
                      , util::sptr<Expression const> value)
{
    _checkNotTerminated(pos);
    _block.addStmt(util::mkptr(new AttrSet(pos, std::move(set_point), std::move(value))));
}

void Filter::addBranch(misc::position const& pos
                     , util::sptr<Expression const> predicate
                     , util::sptr<Filter> consequence
                     , util::sptr<Filter> alternative)
{
    _checkNotTerminated(pos);
    _checkBranchesTermination(pos, consequence, alternative);
    _block.addStmt(util::mkptr(new Branch(pos
                                        , std::move(predicate)
                                        , std::move(consequence->_block)
                                        , std::move(alternative->_block))));
}

void Filter::addBranch(misc::position const& pos
                     , util::sptr<Expression const> predicate
                     , util::sptr<Filter> consequence)
{
    _checkNotTerminated(pos);
    _block.addStmt(util::mkptr(
                new Branch(pos, std::move(predicate), std::move(consequence->_block), Block())));
}

void Filter::addBranchAlterOnly(misc::position const& pos
                              , util::sptr<Expression const> predicate
                              , util::sptr<Filter> alternative)
{
    _checkNotTerminated(pos);
    _block.addStmt(util::mkptr(
                new Branch(pos, std::move(predicate), Block(), std::move(alternative->_block))));
}

void Filter::defName(misc::position const& pos
                   , std::string const& name
                   , util::sptr<Expression const> init)
{
    _checkNotTerminated(pos);
    _block.addStmt(util::mkptr(new NameDef(pos, name, std::move(init))));
}

void Filter::defFunc(util::sptr<Function const> func)
{
    _block.addFunc(std::move(func));
}

Block Filter::deliver()
{
    return std::move(_block);
}

void Filter::_checkBranchesTermination(misc::position const& pos
                                     , util::sptr<Filter> const& consequence
                                     , util::sptr<Filter> const& alternative)
{
    if (consequence->_terminated() || alternative->_terminated()) {
        warning::oneOrTwoBranchesTerminated(*consequence->_term_pos_or_nul_if_not_term
                                          , *alternative->_term_pos_or_nul_if_not_term);
    }
    if (consequence->_terminated() && alternative->_terminated()) {
        _setTerminated(pos);
    }
}

void Filter::_checkNotTerminated(misc::position const& pos)
{
    if (_terminated()) {
        _reportTerminated(pos);
    }
}

void Filter::_reportTerminated(misc::position const& pos)
{
    if (!_error_reported) {
        error::flowTerminated(pos, _term_pos_or_nul_if_not_term.cp());
        _error_reported = true;
    }
}

bool Filter::_terminated() const
{
    return _term_pos_or_nul_if_not_term.not_nul();
}

void Filter::_setTerminated(misc::position const& pos)
{
    _term_pos_or_nul_if_not_term.reset(new misc::position(pos));
}
