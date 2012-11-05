#ifndef __STEKIN_SEMANTIC_FUNCTION_BODY_FILTER_H__
#define __STEKIN_SEMANTIC_FUNCTION_BODY_FILTER_H__

#include "filter.h"
#include "symbol-table.h"

namespace semantic {

    struct FuncBodyFilter
        : Filter
    {
        FuncBodyFilter(misc::position const& ps, std::vector<std::string> const& params)
            : pos(ps)
            , param_names(params)
        {}

        void defName(misc::position const& pos
                   , std::string const& name
                   , util::sptr<Expression const> init);
        void defFunc(misc::position const& pos
                   , std::string const& name
                   , std::vector<std::string> const&
                   , util::sptr<Filter> body);

        misc::position const pos;
        std::vector<std::string> const param_names;
    protected:
        FuncBodyFilter() {}
    protected:
        SymbolTable _symbols;
    };

}

#endif /* __STEKIN_SEMANTIC_FUNCTION_BODY_FILTER_H__ */
