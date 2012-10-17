#ifndef __STEKIN_TEST_COMMON_H__
#define __STEKIN_TEST_COMMON_H__

#include <ostream>

#include <misc/pos-type.h>

namespace test {

    std::ostream& operator<<(std::ostream& os, misc::position const& pos); 

}

#endif /* __STEKIN_TEST_COMMON_H__ */
