#include <report/errors.h>

#include "node-base.h"
#include "expr-nodes.h"
#include "filter.h"

using namespace semantic;

bool Expression::isLiteral(util::sref<SymbolTable const>) const
{
    return false;
}

std::string Expression::literalType(util::sref<SymbolTable const>) const
{
    return "";
}

bool Expression::boolValue(util::sref<SymbolTable const> st) const
{
    error::condNotBool(pos, literalType(st));
    return false;
}

mpz_class Expression::intValue(util::sref<SymbolTable const>) const
{
    return 0;
}

mpf_class Expression::floatValue(util::sref<SymbolTable const>) const
{
    return 0;
}

std::string Expression::stringValue(util::sref<SymbolTable const>) const
{
    return "";
}
