#include <report/errors.h>

#include "clause-builder.h"
#include "clauses.h"

using namespace grammar;

namespace {

    struct GlobalClause
        : ClauseBase
    {
        GlobalClause()
            : ClauseBase(-1)
        {}

        util::sref<Block> getGlobalBlockPtr()
        {
            return *_block;
        }

        void deliver() {}
    };

}

ClauseBuilder::ClauseBuilder()
    : _global(nullptr)
{
    util::sptr<GlobalClause> global(new GlobalClause);
    _global = global->getGlobalBlockPtr();
    _clauses.push_back(std::move(global));
}

void ClauseBuilder::addTokens(
        int indent_len, misc::position const& pos, std::vector<util::sptr<Token>> sequence)
{
    if (_shrinkTo(indent_len, pos)) {
        _clauses.back()->setMemberIndent(indent_len, pos);
        _clauses.back()->prepareArith();
    }
    for (auto& token: sequence) {
        this->_clauses.back()->nextToken(token);
    }
    this->_clauses.back()->tryFinish(pos, this->_clauses);
}

util::sptr<semantic::Statement const> ClauseBuilder::buildAndClear(misc::position const& pos)
{
    if (!_shrinkTo(0, pos)) {
        error::unexpectedEof(pos);
    }
    _clauses[0]->tryFinish(misc::position(), _clauses);
    return _global->compile();
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
