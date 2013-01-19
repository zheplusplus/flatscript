#include <semantic/filter.h>
#include <semantic/node-base.h>
#include <semantic/function.h>

#include "block.h"
#include "node-base.h"
#include "function.h"

using namespace grammar;

void Block::addStmt(util::sptr<Statement const> stmt)
{
    _stmts.append(std::move(stmt));
}

void Block::addFunc(util::sptr<Function const> Function)
{
    _funcs.append(std::move(Function));
}

util::sptr<semantic::Filter> Block::compile(BaseReducingEnv const& env) const
{
    util::sptr<semantic::Filter> filter(new semantic::Filter);
    _funcs.iter([&](util::sptr<Function const> const& func, int)
                {
                    func->compile(*filter);
                });
    _stmts.iter([&](util::sptr<Statement const> const& stmt, int)
                {
                    stmt->compile(*filter, env);
                });
    return std::move(filter);
}

util::sptr<semantic::Filter> Block::compile() const
{
    return compile(ExprReducingEnv());
}
