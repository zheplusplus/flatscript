#include <semantic/func-body-filter.h>

#include "function.h"
#include "node-base.h"

using namespace grammar;

void Function::compile(util::sref<semantic::Filter> filter) const
{
    filter->defFunc(pos, name, param_names, body.compile(
                                util::mkptr(new semantic::FuncBodyFilter(pos, param_names))));
}
