#include <proto/node-base.h>

#include "func-body-filter.h"
#include "node-base.h"
#include "expr-nodes.h"
#include "function.h"

using namespace flchk;

void FuncBodyFilter::defVar(misc::position const& pos
                          , std::string const& name
                          , util::sptr<Expression const> init)
{
    _accumulator.defVar(pos, name, init->fold());
}

void FuncBodyFilter::defFunc(misc::position const& pos
                           , std::string const& name
                           , std::vector<std::string> const& param_names
                           , util::sptr<Filter> body)
{
    _accumulator.defFunc(pos, name, param_names, std::move(body));
    _symbols.defVar(pos, name);
}

util::sref<SymbolTable> FuncBodyFilter::getSymbols()
{
    return util::mkref(_symbols);
}
