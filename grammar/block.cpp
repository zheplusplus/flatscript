#include <semantic/filter.h>
#include <semantic/node-base.h>
#include <semantic/function.h>
#include <report/errors.h>

#include "block.h"
#include "node-base.h"
#include "function.h"

using namespace grammar;

void Block::addStmt(util::sptr<Statement> stmt)
{
    _stmts.append(std::move(stmt));
}

void Block::addFunc(util::sptr<Function const> Function)
{
    _funcs.append(std::move(Function));
}

void Block::acceptElse(misc::position const& else_pos, Block&& block)
{
    if (_stmts.empty()) {
        return error::elseNotMatchIf(else_pos);
    }
    _stmts.back()->acceptElse(else_pos, std::move(block));
}

util::sptr<semantic::Filter> Block::compile() const
{
    util::sptr<semantic::Filter> filter(new semantic::Filter);
    _funcs.iter([&](util::sptr<Function const> const& func, int)
                {
                    func->compile(*filter);
                });
    _stmts.iter([&](util::sptr<Statement> const& stmt, int)
                {
                    stmt->compile(*filter);
                });
    return std::move(filter);
}
