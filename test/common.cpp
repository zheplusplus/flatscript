#include "common.h"

using namespace test;

std::ostream& test::operator<<(std::ostream& os, misc::position const& pos)
{
    return os << pos.str();
}
