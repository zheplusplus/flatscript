#include <semantic/expr-nodes.h>
#include <semantic/function.h>
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

std::string Expression::reduceAsProperty() const
{
    return reduceAsName();
}

util::sptr<semantic::Expression const> Expression::reduceAsLeftValue(BaseReducingEnv const&) const
{
    error::invalidLeftValue(pos);
    return util::mkptr(new semantic::ListSlice::Default(pos));
}

util::sptr<semantic::Expression const> Expression::reduceAsArg(ArgReducingEnv& env, int) const
{
    return reduceAsExpr(env);
}
