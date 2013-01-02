#include <report/errors.h>

#include "symbol-def-filter.h"

using namespace semantic;

void SymbolDefFilter::defName(misc::position const& pos
                            , std::string const& name
                            , util::sptr<Expression const>)
{
    error::forbidDefName(pos, name);
}

void SymbolDefFilter::defFunc(misc::position const& pos
                            , std::string const& name
                            , std::vector<std::string> const&
                            , util::sptr<Filter>)
{
    error::forbidDefFunc(pos, name);
}
