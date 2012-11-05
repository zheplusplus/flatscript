#ifndef __STEKIN_SEMANTIC_SYMBOL_TABLE_H__
#define __STEKIN_SEMANTIC_SYMBOL_TABLE_H__

#include <string>
#include <map>
#include <list>
#include <vector>

#include <output/fwd-decl.h>
#include <util/pointer.h>
#include <misc/pos-type.h>

#include "fwd-decl.h"

namespace semantic {

    struct SymbolTable {
        SymbolTable(misc::position const& pos
                  , util::sref<SymbolTable const> ext_sym
                  , std::vector<std::string> const& params);

        SymbolTable()
            : ext_symbols(nullptr)
        {}

        void reference(misc::position const& pos, std::string const& name);
        void refNames(misc::position const& pos, std::vector<std::string> const& names);
        void defName(misc::position const& pos, std::string const& name);
        void defConst(misc::position const& pos
                    , std::string const& name
                    , util::sref<Expression const> value);
        void imported(misc::position const& pos, std::string const& name);

        util::sref<Expression const> literalOrNul(std::string const& name) const;
        bool isImported(std::string const& name) const;
        util::sptr<output::Expression const> compileRef(misc::position const& pos
                                                      , std::string const& name);
    public:
        util::sref<SymbolTable const> const ext_symbols;
    private:
        util::sref<misc::position const> _localDefPosOrNul(std::string const& name) const;
        void _checkNoRef(misc::position const& pos, std::string const& name);
        void _checkNoDef(misc::position const& pos, std::string const& name);
        void _checkDefinition(misc::position const& pos, std::string const& name) const;
    private:
        std::map<std::string, std::list<misc::position>> _external_name_refs;
        std::map<std::string, misc::position> _name_defs;
        std::map<std::string, misc::position> _imported;
        std::map<std::string, std::pair<misc::position, util::sref<Expression const>>> _const_defs;
        std::vector<std::string> const _parameters;
    };

}

#endif /* __STEKIN_SEMANTIC_SYMBOL_TABLE_H__ */
