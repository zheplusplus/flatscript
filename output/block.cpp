#include <algorithm>

#include "block.h"

using namespace output;

void Block::write(std::ostream& os) const
{
    os << "{" << std::endl;
    std::for_each(_funcs.begin()
                , _funcs.end()
                , [&](util::sptr<Function const> const& func)
                  {
                      func->write(os);
                  });
    std::for_each(_stmts.begin()
                , _stmts.end()
                , [&](util::sptr<Statement const> const& stmt)
                  {
                      stmt->write(os);
                  });
    os << "}" << std::endl;
}

void Block::addStmt(util::sptr<Statement const> stmt)
{
    _stmts.push_back(std::move(stmt));
}

void Block::addFunc(util::sptr<Function const> func)
{
    _funcs.push_back(std::move(func));
}
