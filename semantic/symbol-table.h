#ifndef __FLSC_SEMANTIC_SYMBOL_TABLE_H__
#define __FLSC_SEMANTIC_SYMBOL_TABLE_H__

#include <map>
#include <set>
#include <string>
#include <output/node-base.h>

#include "node-base.h"

namespace semantic {

    struct SymbolTable {
        virtual ~SymbolTable() {}

        SymbolTable()
            : _ext_symbols(nullptr)
        {}

        explicit SymbolTable(util::sref<SymbolTable> ext_sym)
            : _ext_symbols(ext_sym)
        {}

        virtual void defFunc(misc::position const& pos, std::string const& name) = 0;
        virtual void defParam(misc::position const& pos, std::string const& name) = 0;
        virtual std::set<std::string> localNames() const = 0;

        virtual void defName(misc::position const& pos, std::string const& name);
        virtual void defAsyncParam(misc::position const& pos, std::string const& name);
        virtual void defConst(misc::position const& pos
                            , std::string const& name
                            , util::sref<Expression const> value);
        virtual void addExternNames(misc::position const& pos, std::vector<std::string> const&);
        virtual void refNames(misc::position const& pos, std::vector<std::string> const& names);
        virtual util::sref<Expression const> literalOrNul(std::string const& name) const;
        virtual util::sptr<output::Expression const> compileRef(
                misc::position const& pos, std::string const& name);
    protected:
        util::sref<SymbolTable> const _ext_symbols;

        virtual util::sptr<output::Expression const> _makeReference(
            misc::position const& pos, std::string const& name) = 0;

        util::sref<misc::position const> _localDefPosOrNul(std::string const& name) const;
        void _checkNoRef(misc::position const& pos, std::string const& name);
        void _checkNoDef(misc::position const& pos, std::string const& name);
    protected:
        std::map<std::string, std::vector<misc::position>> _references;
        std::map<std::string, misc::position> _name_defs;
        std::map<std::string, misc::position> _async_param_defs;
        std::map<std::string, misc::position> _external;
        std::map<std::string, std::pair<misc::position, util::sref<Expression const>>> _const_defs;
        std::vector<std::string> const _parameters;
    };

    struct RegularSymbolTable
        : SymbolTable
    {
        explicit RegularSymbolTable(util::sref<SymbolTable> ext_sym)
            : SymbolTable(ext_sym)
        {}

        RegularSymbolTable(misc::position const& pos
                         , util::sref<SymbolTable> ext_sym
                         , std::vector<std::string> const& params);

        void defFunc(misc::position const& pos, std::string const& name);
        void defParam(misc::position const& pos, std::string const& name);
        std::set<std::string> localNames() const;
    protected:
        RegularSymbolTable() = default;
    private:
        util::sptr<output::Expression const> _makeReference(
            misc::position const& pos, std::string const& name);

        std::set<std::string> _exclude_decls;
    };

    struct GlobalSymbolTable
        : RegularSymbolTable
    {
        GlobalSymbolTable() = default;

        void addExternNames(misc::position const& pos, std::vector<std::string> const& names);
    };

    struct SubSymbolTable
        : SymbolTable
    {
        explicit SubSymbolTable(util::sref<SymbolTable> ext_sym)
            : SymbolTable(ext_sym)
        {}

        void defParam(misc::position const&, std::string const&) {}

        void defFunc(misc::position const& pos, std::string const& name);
        std::set<std::string> localNames() const;
    private:
        util::sptr<output::Expression const> _makeReference(
            misc::position const& pos, std::string const& name);
    };

}

#endif /* __FLSC_SEMANTIC_SYMBOL_TABLE_H__ */
