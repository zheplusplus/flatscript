#ifndef __STEKIN_SEMANTIC_SYMBOL_DEFINITION_FILTER_H__
#define __STEKIN_SEMANTIC_SYMBOL_DEFINITION_FILTER_H__

#include "filter.h"

namespace semantic {

    struct SymbolDefFilter
        : Filter
    {
        void defName(misc::position const& pos
                   , std::string const& name
                   , util::sptr<Expression const>);
        void defFunc(misc::position const& pos
                   , std::string const& name
                   , std::vector<std::string> const&
                   , util::sptr<Filter>);
    };

}

#endif /* __STEKIN_SEMANTIC_SYMBOL_DEFINITION_FILTER_H__ */
