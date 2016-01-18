#include "block.h"
#include "scope.h"
#include "symbol-table.h"

using namespace semantic;

void Block::compile(util::sref<Scope> scope) const
{
    util::sref<output::Block> root_block(scope->block());
    _funcs.iter([&](util::sptr<Function const> const& func, int)
                {
                    scope->sym()->defFunc(func->pos, func->name);
                });
    _stmts.iter([&](util::sptr<Statement const> const& stmt, int)
                {
                    stmt->compile(scope);
                });
    _funcs.iter([&](util::sptr<Function const> const& func, int)
                {
                    root_block->addFunc(func->compile(scope));
                });
}

bool Block::isAsync() const
{
    return _stmts.any([](util::sptr<Statement const> const& stmt, int)
                      {
                          return stmt->isAsync();
                      });
}

void Block::append(Block following)
{
    _funcs.append(std::move(following._funcs));
    _stmts.append(std::move(following._stmts));
}
