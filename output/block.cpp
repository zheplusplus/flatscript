#include "block.h"

using namespace output;

void Block::write(std::ostream& os) const
{
    _funcs.iter([&](util::sptr<Function const> const& func, int)
                {
                    func->write(os);
                });
    _stmts.iter([&](util::sptr<Statement const> const& stmt, int)
                {
                    stmt->write(os);
                });
}

int Block::count() const
{
    return _stmts.size() + _funcs.size();
}

void Block::addStmt(util::sptr<Statement const> stmt)
{
    _stmts.append(std::move(stmt));
}

void Block::addFunc(util::sptr<Function const> func)
{
    _funcs.append(std::move(func));
}

void Block::append(util::sptr<Block> b)
{
    _stmts.append(std::move(b->_stmts));
    _funcs.append(std::move(b->_funcs));
}
