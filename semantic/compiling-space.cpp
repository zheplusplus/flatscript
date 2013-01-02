#include <algorithm>

#include <output/expr-nodes.h>
#include <report/errors.h>

#include "compiling-space.h"
#include "const-fold.h"

using namespace semantic;

SymbolTable::SymbolTable(misc::position const& pos
                       , util::sref<SymbolTable const> ext_sym
                       , std::vector<std::string> const& params)
    : ext_symbols(ext_sym)
{
    std::for_each(params.begin()
                , params.end()
                , [&](std::string const& param)
                  {
                      defName(pos, param);
                  });
}

void SymbolTable::reference(misc::position const& pos, std::string const& name)
{
    if (_name_defs.end() == _name_defs.find(name)) {
        _external_name_refs[name].push_back(pos);
        _checkDefinition(pos, name);
    }
}

void SymbolTable::refNames(misc::position const& pos, std::vector<std::string> const& names)
{
    std::for_each(names.begin()
                , names.end()
                , [&](std::string const& name)
                  {
                      reference(pos, name);
                  });
}

void SymbolTable::defName(misc::position const& pos, std::string const& name)
{
    _checkNoRef(pos, name);
    _checkNoDef(pos, name);
    _name_defs.insert(std::make_pair(name, pos));
}

void SymbolTable::defConst(
            misc::position const& pos, std::string const& name, util::sref<Expression const> value)
{
    _checkNoRef(pos, name);
    _checkNoDef(pos, name);
    _const_defs.insert(std::make_pair(name, std::make_pair(pos, value)));
}

void SymbolTable::imported(misc::position const& pos, std::string const& name)
{
    _checkNoRef(pos, name);
    _checkNoDef(pos, name);
    _imported.insert(std::make_pair(name, pos));
}

util::sref<Expression const> SymbolTable::literalOrNul(std::string const& name) const
{
    auto find_result = _const_defs.find(name);
    if (find_result != _const_defs.end()) {
        return find_result->second.second;
    }
    if (ext_symbols.nul()) {
        return util::sref<Expression const>(nullptr);
    }
    return ext_symbols->literalOrNul(name);
}

bool SymbolTable::isImported(std::string const& name) const
{
    if (_imported.find(name) != _imported.end()) {
        return true;
    }
    if (ext_symbols.nul()) {
        return false;
    }
    return ext_symbols->isImported(name);
}

util::sptr<output::Expression const> SymbolTable::compileRef(
                                            misc::position const& pos, std::string const& name)
{
    reference(pos, name);
    util::sref<Expression const> literal(literalOrNul(name));
    if (literal.not_nul()) {
        return compileLiteral(literal, util::mkref(*this));
    }
    if (isImported(name)) {
        return util::mkptr(new output::ImportedName(pos, name));
    }
    return util::mkptr(new output::Reference(pos, name));
}

util::sref<misc::position const> SymbolTable::_localDefPosOrNul(std::string const& name) const
{
    if (_name_defs.end() != _name_defs.find(name)) {
        return util::mkref(_name_defs.find(name)->second);
    }
    if (_const_defs.end() != _const_defs.find(name)) {
        return util::mkref(_const_defs.find(name)->second.first);
    }
    if (_imported.end() != _imported.find(name)) {
        return util::mkref(_imported.find(name)->second);
    }
    return util::sref<misc::position const>(nullptr);
}

void SymbolTable::_checkNoDef(misc::position const& pos, std::string const& name)
{
    util::sref<misc::position const> local_pos(_localDefPosOrNul(name));
    if (local_pos.not_nul()) {
        error::nameAlreadyInLocal(local_pos.cp(), pos, name);
    }
}

void SymbolTable::_checkNoRef(misc::position const& pos, std::string const& name)
{
    auto local_refs = _external_name_refs.find(name);
    if (_external_name_refs.end() != local_refs) {
        error::nameRefBeforeDef(pos, std::vector<misc::position>(local_refs->second.begin()
                                                               , local_refs->second.end()), name);
    }
}

void SymbolTable::_checkDefinition(misc::position const& pos, std::string const& name) const
{
    if (_localDefPosOrNul(name).nul()) {
        if (ext_symbols.nul()) {
            error::nameNotDef(pos, name);
        } else {
            ext_symbols->_checkDefinition(pos, name);
        }
    }
}

CompilingSpace::CompilingSpace(SymbolTable const& st)
    : _main_block(new output::Block)
    , _symbol(st)
    , _current_block(*_main_block)
{}

util::sref<SymbolTable> CompilingSpace::sym()
{
    return util::mkref(_symbol);
}

util::sref<output::Block> CompilingSpace::block() const
{
    return _current_block;
}

void CompilingSpace::setAsyncSpace(misc::position const& pos
                                 , std::vector<std::string> const& params
                                 , util::sref<output::Block> block)
{
    std::for_each(params.begin()
                , params.end()
                , [&](std::string const& param)
                  {
                      _symbol.defName(pos, param);
                  });
    setAsyncSpace(block);
}

void CompilingSpace::setAsyncSpace(util::sref<output::Block> block)
{
    _current_block = block;
}

util::sptr<output::Block> CompilingSpace::deliver()
{
    return std::move(_main_block);
}
