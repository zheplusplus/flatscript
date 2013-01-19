#include <semantic/function.h>
#include <semantic/filter.h>

#include "function.h"
#include "node-base.h"

using namespace grammar;

void Function::compile(util::sref<semantic::Filter> filter) const
{
    filter->defFunc(pos, name, param_names, body.compile());
}
