#ifndef __STEKIN_FLOWCHECK_FUNCTION_BODY_FILTER_H__
#define __STEKIN_FLOWCHECK_FUNCTION_BODY_FILTER_H__

#include "filter.h"
#include "symbol-table.h"

namespace flchk {

    struct FuncBodyFilter
        : public Filter
    {
        FuncBodyFilter(misc::position const& pos
                     , util::sref<SymbolTable const> ext_symbols
                     , std::vector<std::string> const& params)
            : _symbols(pos, ext_symbols, params)
        {}
    public:
        void defVar(misc::position const& pos
                  , std::string const& name
                  , util::sptr<Expression const>);
        util::sref<SymbolTable> getSymbols();
        void defFunc(misc::position const& pos
                   , std::string const& name
                   , std::vector<std::string> const&
                   , util::sptr<Filter> body);
    protected:
        FuncBodyFilter() {}
    protected:
        SymbolTable _symbols;
    };

}

#endif /* __STEKIN_FLOWCHECK_FUNCTION_BODY_FILTER_H__ */
