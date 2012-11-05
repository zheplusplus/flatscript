#include <algorithm>

#include <output/list-pipe.h>

#include "list-pipe.h"

using namespace semantic;

util::sptr<output::Expression const> ListPipeMapper::compile(util::sref<SymbolTable> st) const
{
    return util::mkptr(new output::ListPipeMapper(pos, list->compile(st), mapper->compile(st)));
}

util::sptr<output::Expression const> ListPipeFilter::compile(util::sref<SymbolTable> st) const
{
    return util::mkptr(new output::ListPipeFilter(pos, list->compile(st), filter->compile(st)));
}
