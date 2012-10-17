#include <util/string.h>

#include "name-mangler.h"

std::string output::formVarName(bool local, std::string const& name)
{
    return (local ? "lc_" : "ext_") + name;
}

std::string output::formFuncName(std::string const& name, int param_count)
{
    return "f" + util::str(param_count) + "_" + name;
}
