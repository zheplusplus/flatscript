#include "block.h"

using namespace output;

void Block::write(std::ostream& os) const
{
    os << "{" << std::endl;
    _funcs.iter([&](util::sptr<Function const> const& func, int)
                {
                    func->write(os);
                });
    _stmts.iter([&](util::sptr<Statement const> const& stmt, int)
                {
                    stmt->write(os);
                });
    os << "}" << std::endl;
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
