#include <semantic/function.h>
#include <semantic/filter.h>

#include "function.h"
#include "node-base.h"

using namespace grammar;

void Function::compile(util::sref<semantic::Filter> filter) const
{
    if (-1 == async_param_index) {
        return filter->defFunc(util::mkptr(
                new semantic::Function(pos, name, param_names, body.compile()->deliver())));
    }
    return filter->defFunc(util::mkptr(new semantic::RegularAsyncFunction(
                    pos, name, param_names, async_param_index, body.compile()->deliver())));
}
