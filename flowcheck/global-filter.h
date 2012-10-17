#ifndef __STEKIN_FLOWCHECK_GLOBAL_FILTER_H__
#define __STEKIN_FLOWCHECK_GLOBAL_FILTER_H__

#include "func-body-filter.h"

namespace flchk {

    struct GlobalFilter
        : public FuncBodyFilter
    {
        GlobalFilter();
    };

}

#endif /* __STEKIN_FLOWCHECK_GLOBAL_FILTER_H__ */
