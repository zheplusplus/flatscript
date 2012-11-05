#include <algorithm>

#include <output/function.h>

#include "function.h"
#include "node-base.h"
#include "filter.h"
#include "symbol-table.h"

using namespace semantic;

util::sptr<output::Function const> Function::compile(util::sref<SymbolTable> st) const
{
    SymbolTable body_st(pos, st, param_names);
    return util::mkptr(new output::Function(
                            pos, name, param_names, _body->compile(util::mkref(body_st))));
}
