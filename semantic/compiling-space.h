#ifndef __STEKIN_SEMANTIC_COMPILING_SPACE_H__
#define __STEKIN_SEMANTIC_COMPILING_SPACE_H__

#include <string>
#include <vector>
#include <map>
#include <list>

#include <output/block.h>
#include <util/pointer.h>
#include <misc/pos-type.h>

#include "fwd-decl.h"

namespace semantic {

    struct SymbolTable {
        SymbolTable(misc::position const& pos
                  , util::sref<SymbolTable const> ext_sym
                  , std::vector<std::string> const& params);

        explicit SymbolTable(util::sref<SymbolTable const> ext_sym)
            : ext_symbols(ext_sym)
        {}

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

    struct CompilingSpace {
        CompilingSpace()
            : CompilingSpace(SymbolTable())
        {}

        explicit CompilingSpace(util::sref<SymbolTable> ext_st)
            : CompilingSpace(SymbolTable(ext_st))
        {}

        CompilingSpace(misc::position const& pos
                     , util::sref<SymbolTable> ext_st
                     , std::vector<std::string> const& params)
            : CompilingSpace(SymbolTable(pos, ext_st, params))
        {}
    private:
        explicit CompilingSpace(SymbolTable const& st);
    public:
        CompilingSpace(CompilingSpace&& rhs)
            : _main_block(std::move(rhs._main_block))
            , _symbol(rhs._symbol)
            , _current_block(rhs._current_block)
        {}

        util::sref<SymbolTable> sym();
        util::sref<output::Block> block() const;

        void setAsyncSpace(misc::position const& pos
                         , std::vector<std::string> const& params
                         , util::sref<output::Block> block);
        void setAsyncSpace(util::sref<output::Block> block);

        util::sptr<output::Block> deliver();
    private:
        util::sptr<output::Block> _main_block;
        SymbolTable _symbol;
        util::sref<output::Block> _current_block;

        CompilingSpace(CompilingSpace const&) = delete;
    };

}

#endif /* __STEKIN_SEMANTIC_COMPILING_SPACE_H__ */
