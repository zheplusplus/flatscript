#include <output/node-base.h>
#include <output/function.h>

#include "block.h"
#include "function.h"
#include "class.h"
#include "node-base.h"
#include "compiling-space.h"

using namespace semantic;

void Block::compile(BaseCompilingSpace& space) const
{
    util::sref<SymbolTable> root_sym(space.sym());
    util::sref<output::Block> root_block(space.block());
    _funcs.iter([&](util::sptr<Function const> const& func, int)
                {
                    root_sym->defFunc(func->pos, func->name);
                });
    _stmts.iter([&](util::sptr<Statement const> const& stmt, int)
                {
                    stmt->compile(space);
                });
    _funcs.iter([&](util::sptr<Function const> const& func, int)
                {
                    root_block->addFunc(func->compile(root_sym));
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
    _classes.append(std::move(following._classes));
}
