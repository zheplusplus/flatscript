#include <proto/node-base.h>
#include <report/errors.h>
#include <report/warnings.h>

#include "accumulator.h"
#include "stmt-nodes.h"
#include "function.h"
#include "filter.h"

using namespace flchk;

void Accumulator::addReturn(misc::position const& pos, util::sptr<Expression const> ret_val)
{
    _checkNotTerminated(pos);
    _setTerminatedNotByVoidReturn(pos);
    _block.addStmt(util::mkptr(new Return(pos, std::move(ret_val))));
}

void Accumulator::addReturnNothing(misc::position const& pos)
{
    _checkNotTerminated(pos);
    _setTerminatedByVoidReturn(pos);
    _block.addStmt(util::mkptr(new ReturnNothing(pos)));
}

void Accumulator::addArith(misc::position const& pos, util::sptr<Expression const> expr)
{
    _checkNotTerminated(pos);
    _block.addStmt(util::mkptr(new Arithmetics(pos, std::move(expr))));
}

void Accumulator::addImport(misc::position const& pos, std::vector<std::string> const& names)
{
    _checkNotTerminated(pos);
    _block.addStmt(util::mkptr(new Import(pos, names)));
}

void Accumulator::addAttrSet(misc::position const& pos
                           , util::sptr<Expression const> set_point
                           , util::sptr<Expression const> value)
{
    _checkNotTerminated(pos);
    _block.addStmt(util::mkptr(new AttrSet(pos, std::move(set_point), std::move(value))));
}

void Accumulator::addBranch(misc::position const& pos
                          , util::sptr<Expression const> predicate
                          , Accumulator consequence
                          , Accumulator alternative)
{
    _checkNotTerminated(pos);
    _checkBranchesTermination(consequence, alternative);
    _setTerminationBySubAccumulator(consequence);
    _setTerminationBySubAccumulator(alternative);
    _block.addStmt(util::mkptr(new Branch(pos
                                        , std::move(predicate)
                                        , std::move(consequence._block)
                                        , std::move(alternative._block))));
}

void Accumulator::addBranch(misc::position const& pos
                          , util::sptr<Expression const> predicate
                          , Accumulator consequence)
{
    _checkNotTerminated(pos);
    _setTerminationBySubAccumulator(consequence);
    _block.addStmt(util::mkptr(new Branch(pos
                                        , std::move(predicate)
                                        , std::move(consequence._block)
                                        , std::move(Block()))));
}

void Accumulator::addBranchAlterOnly(misc::position const& pos
                                   , util::sptr<Expression const> predicate
                                   , Accumulator alternative)
{
    _checkNotTerminated(pos);
    _setTerminationBySubAccumulator(alternative);
    _block.addStmt(util::mkptr(new Branch(pos
                                        , std::move(predicate)
                                        , std::move(Block())
                                        , std::move(alternative._block))));
}

void Accumulator::addBlock(Accumulator b)
{
    _block.append(std::move(b._block));
    _setSelfTerminated(std::move(b));
}

void Accumulator::defName(misc::position const& pos
                        , std::string const& name
                        , util::sptr<Expression const> init)
{
    _checkNotTerminated(pos);
    _block.addStmt(util::mkptr(new NameDef(pos, name, std::move(init))));
}

util::sref<Function const> Accumulator::defFunc(misc::position const& pos
                                              , std::string const& name
                                              , std::vector<std::string> const& param_names
                                              , util::sptr<Filter> body)
{
    return _block.defFunc(pos, name, param_names, std::move(body));
}

util::sptr<proto::Statement const> Accumulator::compileBlock(util::sref<SymbolTable> st) const
{
    return _block.compile(st);
}

bool Accumulator::hintReturnVoid() const
{
    return _contains_void_return || !_terminated();
}

void Accumulator::_setTerminatedByVoidReturn(misc::position const& pos)
{
    _setTerminatedNotByVoidReturn(pos);
    _contains_void_return = true;
}

void Accumulator::_setTerminatedNotByVoidReturn(misc::position const& pos)
{
    _term_pos_or_nul_if_not_term.reset(new misc::position(pos));
}

void Accumulator::_setTerminationBySubAccumulator(Accumulator const& sub)
{
    _contains_void_return = _contains_void_return || sub._contains_void_return;
}

void Accumulator::_checkBranchesTermination(Accumulator const& consequence
                                          , Accumulator const& alternative)
{
    if (consequence._terminated() || alternative._terminated()) {
        warning::oneOrTwoBranchesTerminated(*consequence._term_pos_or_nul_if_not_term
                                          , *alternative._term_pos_or_nul_if_not_term);
    }
}

void Accumulator::_checkNotTerminated(misc::position const& pos)
{
    if (_terminated()) {
        _reportTerminated(pos);
    }
}

void Accumulator::_reportTerminated(misc::position const& pos)
{
    if (!_error_reported) {
        error::flowTerminated(pos, _term_pos_or_nul_if_not_term.cp());
        _error_reported = true;
    }
}

bool Accumulator::_terminated() const
{
    return _term_pos_or_nul_if_not_term.not_nul();
}

void Accumulator::_setSelfTerminated(Accumulator term)
{
    _setTerminationBySubAccumulator(term);
    _term_pos_or_nul_if_not_term = std::move(term._term_pos_or_nul_if_not_term);
    if (_terminated()) {
        _error_reported = true;
    }
}
