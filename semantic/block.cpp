#include <algorithm>

#include <output/node-base.h>
#include <output/function.h>
#include <util/vector-append.h>

#include "block.h"
#include "function.h"
#include "node-base.h"
#include "filter.h"
#include "symbol-table.h"

using namespace semantic;

void Block::addStmt(util::sptr<Statement const> stmt)
{
    _stmts.push_back(std::move(stmt));
}

void Block::defFunc(misc::position const& pos
                  , std::string const& name
                  , std::vector<std::string> const& param_names
                  , util::sptr<Filter> body)
{
    _funcs.push_back(util::mkptr(new Function(pos, name, param_names, std::move(body))));
}

void Block::compile(util::sref<SymbolTable> st, util::sref<output::Block> block) const
{
    std::for_each(_funcs.begin()
                , _funcs.end()
                , [&](util::sptr<Function const> const& func)
                  {
                      st->defName(func->pos, func->name);
                  });
    std::for_each(_stmts.begin()
                , _stmts.end()
                , [&](util::sptr<Statement const> const& stmt)
                  {
                      stmt->compile(st, block);
                  });
    std::for_each(_funcs.begin()
                , _funcs.end()
                , [&](util::sptr<Function const> const& func)
                  {
                      block->addFunc(func->compile(st));
                  });
}

void Block::append(Block following)
{
    util::ptrs_append(_funcs, std::move(following._funcs));
    util::ptrs_append(_stmts, std::move(following._stmts));
}
