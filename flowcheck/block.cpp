#include <algorithm>

#include <proto/node-base.h>
#include <proto/function.h>
#include <util/vector-append.h>

#include "block.h"
#include "function.h"
#include "node-base.h"
#include "filter.h"

using namespace flchk;

void Block::addStmt(util::sptr<Statement const> stmt)
{
    _stmts.push_back(std::move(stmt));
}

util::sref<Function const> Block::defFunc(misc::position const& pos
                                        , std::string const& name
                                        , std::vector<std::string> const& param_names
                                        , util::sptr<Filter> body)
{
    _funcs.push_back(util::mkptr(new Function(pos, name, param_names, std::move(body))));
    return *_funcs.back();
}

util::sptr<proto::Statement const> Block::compile(util::sref<SymbolTable> st) const
{
    util::sptr<proto::Block> block(new proto::Block);
    std::for_each(_stmts.begin()
                , _stmts.end()
                , [&](util::sptr<Statement const> const& stmt)
                  {
                      block->addStmt(stmt->compile(st));
                  });
    std::for_each(_funcs.begin()
                , _funcs.end()
                , [&](util::sptr<Function const> const& func)
                  {
                      block->addFunc(func->compile());
                  });
    return std::move(block);
}

void Block::append(Block following)
{
    util::ptrs_append(_funcs, std::move(following._funcs));
    util::ptrs_append(_stmts, std::move(following._stmts));
}
