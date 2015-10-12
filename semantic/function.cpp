#include <output/function.h>

#include "function.h"
#include "class.h"
#include "node-base.h"
#include "compiling-space.h"

using namespace semantic;

util::sptr<output::Function const> Function::compile(util::sref<SymbolTable> st) const
{
    CompilingSpace body_space(pos, st, param_names);
    body.compile(body_space);
    return util::mkptr(new output::RegularFunction(name, param_names, body_space.deliver()));
}

util::sptr<output::Function const> RegularAsyncFunction::compile(util::sref<SymbolTable> st) const
{
    RegularAsyncCompilingSpace body_space(pos, st, param_names);
    body.compile(body_space);
    return util::mkptr(new output::RegularAsyncFunction(
                                name, param_names, async_param_index, body_space.deliver()));
}
