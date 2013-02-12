#ifndef __STEKIN_OUTPUT_GLOBAL_H__
#define __STEKIN_OUTPUT_GLOBAL_H__

#include "node-base.h"

namespace output {

    void wrapGlobal(std::ostream& os, util::sref<Statement const> global);

}

#endif /* __STEKIN_OUTPUT_GLOBAL_H__ */
