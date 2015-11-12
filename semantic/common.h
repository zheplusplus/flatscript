#ifndef __FLSC_SEMANTIC_COMMON_H__
#define __FLSC_SEMANTIC_COMMON_H__

#include <util/pointer.h>
#include <output/node-base.h>

#include "node-base.h"

namespace semantic {

    util::sptr<output::Statement const> makeArith(util::sptr<output::Expression const> e);

}

#endif /* __FLSC_SEMANTIC_COMMON_H__ */
