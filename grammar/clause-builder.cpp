#include <algorithm>

#include <semantic/global-filter.h>
#include <semantic/node-base.h>
#include <semantic/function.h>
#include <report/errors.h>

#include "clause-builder.h"
#include "stmt-nodes.h"
#include "function.h"
#include "clauses.h"

using namespace grammar;

void ClauseBuilder::ClauseOfPack::acceptStmt(util::sptr<Statement> stmt)
{
    _pack.addStmt(std::move(stmt));
}

void ClauseBuilder::ClauseOfPack::acceptFunc(util::sptr<Function const> func)
{
    _pack.addFunc(std::move(func));
}

Block ClauseBuilder::ClauseOfPack::pack()
{
    return std::move(_pack);
}

void ClauseBuilder::addArith(int indent_len
                           , misc::position const& pos
                           , std::vector<util::sptr<Token>> const& sequence)
{
    if (_shrinkTo(indent_len, pos)) {
        _clauses.back()->setMemberIndent(indent_len, pos);
        _clauses.back()->prepareArith();
    }
    _pushSequence(pos, sequence);
}

void ClauseBuilder::addReturn(int indent_len
                            , misc::position const& pos
                            , std::vector<util::sptr<Token>> const& sequence)
{
    if (!_prepareLevel(indent_len, pos, "return")) {
        return;
    }
    _clauses.back()->prepareReturn();
    _pushSequence(pos, sequence);
}

void ClauseBuilder::addImport(int indent_len
                            , misc::position const& pos
                            , std::vector<std::string> const& names)
{
    if (!_prepareLevel(indent_len, pos, "import")) {
        return;
    }
    _clauses.back()->acceptStmt(util::mkptr(new Import(pos, names)));
}

void ClauseBuilder::addExport(int indent_len
                            , misc::position const& pos
                            , std::vector<std::string> const& names
                            , std::vector<util::sptr<Token>> const& sequence)
{
    if (!_prepareLevel(indent_len, pos, "export")) {
        return;
    }
    _clauses.back()->prepareExport(names);
    _pushSequence(pos, sequence);
}

void ClauseBuilder::addFunction(int indent_len
                              , misc::position const& pos
                              , std::string const& name
                              , std::vector<std::string> const& params)
{
    if (!_prepareLevel(indent_len, pos, "func")) {
        return;
    }
    _clauses.push_back(util::mkptr(new FunctionClause(indent_len, pos, name, params)));
}

void ClauseBuilder::addIf(int indent_len
                        , misc::position const& pos
                        , std::vector<util::sptr<Token>> const& sequence)
{
    if (!_prepareLevel(indent_len, pos, "if")) {
        return;
    }
    _clauses.push_back(util::mkptr(new IfClause(indent_len, pos)));
    _pushSequence(pos, sequence);
}

void ClauseBuilder::addIfnot(int indent_len
                           , misc::position const& pos
                           , std::vector<util::sptr<Token>> const& sequence)
{
    if (!_prepareLevel(indent_len, pos, "ifnot")) {
        return;
    }
    _clauses.push_back(util::mkptr(new IfnotClause(indent_len, pos)));
    _pushSequence(pos, sequence);
}

void ClauseBuilder::addElse(int indent_len, misc::position const& pos)
{
    _shrinkTo(indent_len + 1, pos);
    if (_clauses.back()->indent == indent_len) {
        _clauses.back()->acceptElse(pos);
    } else {
        error::elseNotMatchIf(pos);
    }
}

util::sptr<semantic::Filter> ClauseBuilder::buildAndClear()
{
    if (!_shrinkTo(0, misc::position())) {
        error::unexpectedEof();
    }
    _packer->tryEol(misc::position(), _clauses);
    return _packer->pack().compile(util::mkptr(new semantic::GlobalFilter));
}

util::sref<ClauseBuilder::ClauseOfPack> ClauseBuilder::_prepare1stClause()
{
    util::sptr<ClauseOfPack> packer(new ClauseOfPack);
    util::sref<ClauseOfPack> ref = *packer;
    _clauses.push_back(std::move(packer));
    return ref;
}

bool ClauseBuilder::_shrinkTo(int level, misc::position const& pos)
{
    while (level <= _clauses.back()->indent) {
        if (!_clauses.back()->tryEol(pos, _clauses)) {
            return false;
        }
        util::sptr<ClauseBase> deliverer(std::move(_clauses.back()));
        _clauses.pop_back();
        deliverer->deliverTo(*_clauses.back());
    }
    return _clauses.back()->tryEol(pos, _clauses);
}

bool ClauseBuilder::_prepareLevel(int level, misc::position const& pos, std::string const& token)
{
    if (_shrinkTo(level, pos)) {
        _clauses.back()->setMemberIndent(level, pos);
        return true;
    }
    error::unexpectedToken(pos, token);
    return false;
}

void ClauseBuilder::_pushSequence(misc::position const& pos
                                , std::vector<util::sptr<Token>> const& sequence)
{
    std::for_each(sequence.begin()
                , sequence.end()
                , [&](util::sptr<Token> const& token)
                  {
                      _clauses.back()->nextToken(token);
                  });
    _clauses.back()->eol(pos, _clauses);
}
