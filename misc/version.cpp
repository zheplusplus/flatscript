#include "version.h"

std::string const& misc::version()
{
    static std::string V("2015-02-12-Asuka");
    return V;
}
