#include <util/string.h>

#include "function.h"
#include "node-base.h"
#include "name-mangler.h"

using namespace output;

void Function::write(std::ostream& os) const
{
    os << "function " << formName(name) << "(" << util::join(",", formNames(param_names)) << ") {"
       << std::endl;
    body->write(os);
    os << "}" << std::endl;
}
