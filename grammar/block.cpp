#include <algorithm>

#include <semantic/filter.h>
#include <semantic/node-base.h>
#include <semantic/function.h>

#include "block.h"
#include "node-base.h"
#include "function.h"

using namespace grammar;

void Block::addStmt(util::sptr<Statement> stmt)
{
    _stmts.push_back(std::move(stmt));
}

void Block::addFunc(util::sptr<Function const> Function)
{
    _funcs.push_back(std::move(Function));
}

util::sptr<semantic::Filter> Block::compile(util::sptr<semantic::Filter> filter) const
{
    std::for_each(_funcs.begin()
                , _funcs.end()
                , [&](util::sptr<Function const> const& def)
                  {
                      def->compile(*filter);
                  });
    std::for_each(_stmts.begin()
                , _stmts.end()
                , [&](util::sptr<Statement> const& stmt)
                  {
                      stmt->compile(*filter);
                  });
    return std::move(filter);
}
