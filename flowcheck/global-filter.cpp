#include "global-filter.h"

using namespace flchk;

GlobalFilter::GlobalFilter()
{
    getSymbols()->defVar(misc::position(), "write");
}
