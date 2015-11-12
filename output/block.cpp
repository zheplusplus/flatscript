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

bool Block::mayThrow() const
{
    return _stmts.any([](util::sptr<Statement const> const& stmt, int)
                      {
                          return stmt->mayThrow();
                      });
}
