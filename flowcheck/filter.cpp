#include <proto/node-base.h>

#include "filter.h"
#include "node-base.h"
#include "function.h"

using namespace flchk;

void Filter::addReturn(misc::position const& pos, util::sptr<Expression const> ret_val)
{
    _accumulator.addReturn(pos, std::move(ret_val->fold()));
}

void Filter::addReturnNothing(misc::position const& pos)
{
    _accumulator.addReturnNothing(pos);
}

void Filter::addArith(misc::position const& pos, util::sptr<Expression const> expr)
{
    _accumulator.addArith(pos, std::move(expr->fold()));
}

void Filter::addImport(misc::position const& pos, std::vector<std::string> const& names)
{
    _accumulator.addImport(pos, names);
}

void Filter::addAttrSet(misc::position const& pos
                      , util::sptr<Expression const> set_point
                      , util::sptr<Expression const> value)
{
    _accumulator.addAttrSet(pos, std::move(set_point), std::move(value));
}

void Filter::addBranch(misc::position const& pos
                     , util::sptr<Expression const> predicate
                     , util::sptr<Filter> consequence
                     , util::sptr<Filter> alternative)
{
    util::sptr<Expression const> pred(std::move(predicate->fold()));
    if (pred->isLiteral()) {
        _accumulator.addBlock(std::move((pred->boolValue() ? consequence : alternative)
                                                                ->_accumulator));
        return;
    }
    _accumulator.addBranch(pos
                         , std::move(pred)
                         , std::move(consequence->_accumulator)
                         , std::move(alternative->_accumulator));
}

void Filter::addBranch(misc::position const& pos
                     , util::sptr<Expression const> predicate
                     , util::sptr<Filter> consequence)
{
    util::sptr<Expression const> pred(std::move(predicate->fold()));
    if (pred->isLiteral()) {
        if (pred->boolValue()) {
            _accumulator.addBlock(std::move(consequence->_accumulator));
        }
        return;
    }
    _accumulator.addBranch(pos, std::move(pred), std::move(consequence->_accumulator));
}

void Filter::addBranchAlterOnly(misc::position const& pos
                              , util::sptr<Expression const> predicate
                              , util::sptr<Filter> alternative)
{
    util::sptr<Expression const> pred(std::move(predicate->fold()));
    if (pred->isLiteral()) {
        if (!pred->boolValue()) {
            _accumulator.addBlock(std::move(alternative->_accumulator));
        }
        return;
    }
    _accumulator.addBranchAlterOnly(pos, std::move(pred), std::move(alternative->_accumulator));
}

util::sptr<proto::Statement const> Filter::compile()
{
    return _accumulator.compileBlock(getSymbols());
}

bool Filter::hintReturnVoid() const
{
    return _accumulator.hintReturnVoid();
}
