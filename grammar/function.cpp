#include <flowcheck/func-body-filter.h>
#include <flowcheck/node-base.h>
#include <flowcheck/function.h>

#include "function.h"
#include "node-base.h"

using namespace grammar;

void Function::compile(util::sref<flchk::Filter> filter) const
{
    util::sptr<flchk::Filter> func_body_filter(
            new flchk::FuncBodyFilter(pos, filter->getSymbols(), param_names));
    filter->defFunc(pos, name, param_names, body.compile(std::move(func_body_filter)));
}
