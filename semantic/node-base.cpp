#include <report/errors.h>

#include "node-base.h"
#include "expr-nodes.h"
#include "filter.h"

using namespace semantic;

bool Expression::boolValue(util::sref<SymbolTable const> st) const
{
    error::condNotBool(pos, literalType(st));
    return false;
}
