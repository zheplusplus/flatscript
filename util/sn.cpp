#include "sn.h"

using namespace util;

serial_num serial_num::next()
{
    static int x = 0;
    return serial_num(x++);
}
