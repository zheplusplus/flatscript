#include "version.h"

std::string const& misc::version()
{
    static std::string V("Saki");
    return V;
}
