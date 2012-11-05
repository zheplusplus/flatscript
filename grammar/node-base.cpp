#include <semantic/expr-nodes.h>
#include <semantic/filter.h>
#include <report/errors.h>

#include "node-base.h"

using namespace grammar;

bool Expression::empty() const
{
    return false;
}

bool Expression::isName() const
{
    return false;
}

std::string Expression::reduceAsName() const
{
    error::invalidName(pos);
    return "";
}

util::sptr<semantic::Expression const> Expression::reduceAsLeftValue() const
{
    error::invalidLeftValue(pos);
    return util::mkptr(new semantic::ListSlice::Default(pos));
}
