#include <report/errors.h>

#include "node-base.h"

using namespace output;

std::string Expression::strAsProp() const
{
    error::invalidPropertyName(pos, str());
    return "";
}
