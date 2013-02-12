#include <sstream>

#include "pos-type.h"

using namespace misc;

std::string position::str() const
{
    std::stringstream ss;
    ss << "Line: " << line;
    return ss.str();
}

bool position::operator==(position const& rhs) const
{
    return line == rhs.line;
}

position& position::operator=(position const& rhs)
{
    line = rhs.line;
    return *this;
}
