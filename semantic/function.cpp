#include <output/function.h>

#include "function.h"
#include "node-base.h"
#include "filter.h"
#include "compiling-space.h"

using namespace semantic;

util::sptr<output::Function const> Function::compile(util::sref<SymbolTable> st) const
{
    return util::mkptr(new output::Function(
                    pos, name, param_names, body.compile(CompilingSpace(pos, st, param_names))));
}
