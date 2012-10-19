#ifndef __STEKIN_FLOWCHECK_SYMBOL_DEFINITION_FILTER_H__
#define __STEKIN_FLOWCHECK_SYMBOL_DEFINITION_FILTER_H__

#include "filter.h"

namespace flchk {

    struct SymbolDefFilter
        : public Filter
    {
        explicit SymbolDefFilter(util::sref<SymbolTable> symbols)
            : _symbols(symbols)
        {}
    public:
        void defName(misc::position const& pos
                   , std::string const& name
                   , util::sptr<Expression const>);
        util::sref<SymbolTable> getSymbols();
        void defFunc(misc::position const& pos
                   , std::string const& name
                   , std::vector<std::string> const&
                   , util::sptr<Filter>);
    private:
        util::sref<SymbolTable> const _symbols;
    };

}

#endif /* __STEKIN_FLOWCHECK_SYMBOL_DEFINITION_FILTER_H__ */
