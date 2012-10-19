#include <algorithm>
#include <iostream>

#include "block.h"
#include "function.h"

using namespace proto;

void Block::addStmt(util::sptr<Statement const> stmt)
{
    _stmts.push_back(std::move(stmt));
}

void Block::addFunc(util::sptr<Function const> func)
{
    _funcs.push_back(std::move(func));
}

void Block::write() const
{
    std::cout << "{" << std::endl;
    std::for_each(_funcs.begin()
                , _funcs.end()
                , [&](util::sptr<Function const> const& func)
                  {
                      func->write();
                  });
    std::for_each(_stmts.begin()
                , _stmts.end()
                , [&](util::sptr<Statement const> const& stmt)
                  {
                      stmt->write();
                  });
    std::cout << "}" << std::endl;
}
