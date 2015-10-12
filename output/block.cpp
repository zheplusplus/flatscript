#include <util/string.h>

#include "name-mangler.h"
#include "block.h"
#include "function.h"
#include "class.h"

using namespace output;

void Block::write(std::ostream& os) const
{
    if (!_local_decls.empty()) {
        os << "var "
           << util::join(",", std::vector<std::string>(_local_decls.begin(), _local_decls.end()))
           << ";" << std::endl;
    }
    _classes.iter([&](util::sptr<Class const> const& cls, int)
                  {
                      cls->write(os);
                  });
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
    return _classes.size() + _stmts.size() + _funcs.size();
}

void Block::append(util::sptr<Block> b)
{
    _stmts.append(std::move(b->_stmts));
    _funcs.append(std::move(b->_funcs));
    _classes.append(std::move(b->_classes));
    _local_decls.insert(b->_local_decls.begin(), b->_local_decls.end());
}
