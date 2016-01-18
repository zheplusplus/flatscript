#include <sstream>

#include "pos-type.h"

using namespace misc;

std::string position::str() const
{
    std::stringstream ss;
    if (this->file.empty()) {
        ss << "<stdin>";
    } else {
        ss << this->file;
    }
    ss << ":" << this->line;
    return ss.str();
}

std::string position::as_line() const
{
    std::stringstream ss;
    ss << "Line " << line;
    return ss.str();
}

bool position::operator==(position const& rhs) const
{
    return this->file == rhs.file && this->line == rhs.line;
}

position& position::operator=(position const& rhs)
{
    this->file = rhs.file;
    this->line = rhs.line;
    return *this;
}
