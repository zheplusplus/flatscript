#include <sstream>
#include <algorithm>

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

trace& trace::add(position const& pos)
{
    _trace.push_back(pos);
    return *this;
}

position trace::top() const
{
    return *_trace.rbegin();
}

std::string trace::str(std::string const& message) const
{
    std::stringstream ss;
    std::for_each(_trace.rbegin()
                , _trace.rend()
                , [&](misc::position const& pos)
                  {
                      ss << message << pos.str() << std::endl;
                  });
    return ss.str();
}

bool trace::operator==(trace const& rhs) const
{
    return _trace == rhs._trace;
}
