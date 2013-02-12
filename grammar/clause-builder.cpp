#include <algorithm>

#include <semantic/filter.h>
#include <semantic/node-base.h>
#include <semantic/function.h>
#include <report/errors.h>

#include "clause-builder.h"
#include "stmt-nodes.h"
#include "function.h"
#include "clauses.h"

using namespace grammar;

namespace {

    struct GlobalClause
        : ClauseBase
    {
        GlobalClause()
            : ClauseBase(-1)
        {}

        Block* getGlobalBlockPtr()
        {
            return &_block;
        }

        void deliver() {}
    };

}

ClauseBuilder::ClauseBuilder()
{
    util::sptr<GlobalClause> global(new GlobalClause);
    _global = global->getGlobalBlockPtr();
    _clauses.push_back(std::move(global));
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

void ClauseBuilder::addIfnot(int indent_len
                           , misc::position const& pos
                           , std::vector<util::sptr<Token>> const& sequence)
{
    if (!_prepareLevel(indent_len, pos, "ifnot")) {
        return;
    }
    _clauses.push_back(util::mkptr(new IfnotClause(indent_len, pos, *_clauses.back())));
    _pushSequence(pos, sequence);
}

semantic::Block ClauseBuilder::buildAndClear()
{
    if (!_shrinkTo(0, misc::position())) {
        error::unexpectedEof();
    }
    _clauses[0]->tryFinish(misc::position(), _clauses);
    return _global->compile()->deliver();
}

bool ClauseBuilder::_shrinkTo(int level, misc::position const& pos)
{
    while (_clauses.back()->shrinkOn(level)) {
        if (!_clauses.back()->tryFinish(pos, _clauses)) {
            return false;
        }
        _clauses.back()->deliver();
        _clauses.pop_back();
    }
    return _clauses.back()->tryFinish(pos, _clauses);
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
    _clauses.back()->tryFinish(pos, _clauses);
}
