#ifndef __STEKIN_FLOWCHECK_SYMBOL_TABLE_H__
#define __STEKIN_FLOWCHECK_SYMBOL_TABLE_H__

#include <string>
#include <map>
#include <list>
#include <vector>

#include <proto/fwd-decl.h>
#include <util/pointer.h>
#include <misc/pos-type.h>

#include "fwd-decl.h"

namespace flchk {

    struct SymbolTable {
        SymbolTable(misc::position const& pos
                  , util::sref<SymbolTable const> ext_sym
                  , std::vector<std::string> const& params);

        SymbolTable()
            : ext_symbols(nullptr)
        {}

        void refVars(misc::position const& pos, std::vector<std::string> const& vars);
        void defVar(misc::position const& pos, std::string const& name);

        util::sptr<proto::Expression const> compileRef(misc::position const& pos
                                                     , std::string const& name);
        util::sptr<proto::Expression const> compileCall(
                        misc::position const& pos
                      , std::string const& name
                      , std::vector<util::sptr<Expression const>> const& args);
    public:
        util::sref<SymbolTable const> const ext_symbols;
    private:
        void _markReference(misc::position const& pos, std::string const& name);
        void _checkDefinition(misc::position const& pos, std::string const& name) const;
        std::vector<util::sptr<proto::Expression const>> _mkArgs(
                        std::vector<util::sptr<Expression const>> const& args);
    private:
        std::map<std::string, std::list<misc::position>> _external_var_refs;
        std::map<std::string, misc::position> _var_defs;
        std::vector<std::string> const _parameters;
    private:
        static Function _fake_function;
    };

}

#endif /* __STEKIN_FLOWCHECK_SYMBOL_TABLE_H__ */
