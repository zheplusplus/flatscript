#ifndef __STEKIN_SEMANTIC_GLOBAL_FILTER_H__
#define __STEKIN_SEMANTIC_GLOBAL_FILTER_H__

#include "func-body-filter.h"

namespace semantic {

    struct GlobalFilter
        : FuncBodyFilter
    {
        util::sptr<output::Statement const> compile(util::sref<SymbolTable> st) const;
    };

}

#endif /* __STEKIN_SEMANTIC_GLOBAL_FILTER_H__ */
