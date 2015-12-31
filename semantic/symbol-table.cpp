#include <report/errors.h>
#include <output/name-mangler.h>
#include <output/expr-nodes.h>

#include "symbol-table.h"
#include "const-fold.h"

using namespace semantic;

void SymbolTable::defName(misc::position const& pos, std::string const& name)
{
    this->_checkNoRef(pos, name);
    this->_checkNoDef(pos, name);
    this->_name_defs.insert(std::make_pair(name, pos));
}

void SymbolTable::defAsyncParam(misc::position const& pos, std::string const& name)
{
    this->_checkNoRef(pos, name);
    this->_checkNoDef(pos, name);
    this->_async_param_defs.insert(std::make_pair(name, pos));
}

void SymbolTable::defConst(misc::position const& pos
                         , std::string const& name
                         , util::sref<Expression const> value)
{
    this->_checkNoRef(pos, name);
    this->_checkNoDef(pos, name);
    this->_const_defs.insert(std::make_pair(name, std::make_pair(pos, value)));
}

void SymbolTable::addExternNames(misc::position const& pos, std::vector<std::string> const&)
{
    error::externOnlyInGlobal(pos);
}

void SymbolTable::refNames(misc::position const& pos, std::vector<std::string> const& names)
{
    for (auto const& name: names) {
        this->_references[name].push_back(pos);
    }
}

util::sref<Expression const> SymbolTable::literalOrNul(std::string const& name) const
{
    auto find_result = this->_const_defs.find(name);
    if (find_result != this->_const_defs.end()) {
        return find_result->second.second;
    }
    if (this->_localDefPosOrNul(name).not_nul()) {
        return util::sref<Expression const>(nullptr);
    }
    if (this->_ext_symbols.nul()) {
        return util::sref<Expression const>(nullptr);
    }
    return this->_ext_symbols->literalOrNul(name);
}

util::sptr<output::Expression const> SymbolTable::compileRef(
                    misc::position const& pos, std::string const& name)
{
    this->_references[name].push_back(pos);
    auto const_find_result = this->_const_defs.find(name);
    if (this->_const_defs.end() != const_find_result) {
        return compileLiteral(const_find_result->second.second, util::mkref(*this));
    }
    if (this->_external.end() != this->_external.find(name)) {
        return util::mkptr(new output::ImportedName(pos, name));
    }
    if (this->_async_param_defs.find(name) != this->_async_param_defs.end()) {
        return util::mkptr(new output::TransientParamReference(pos, name));
    }
    if (this->_name_defs.end() != this->_name_defs.find(name)) {
        return _makeReference(pos, name);
    }
    if (this->_ext_symbols.not_nul()) {
        return this->_ext_symbols->compileRef(pos, name);
    }
    error::nameNotDef(pos, name);
    return this->_makeReference(pos, name);
}

util::sref<misc::position const> SymbolTable::_localDefPosOrNul(std::string const& name) const
{
    if (this->_name_defs.end() != this->_name_defs.find(name)) {
        return util::mkref(this->_name_defs.find(name)->second);
    }
    if (this->_const_defs.end() != this->_const_defs.find(name)) {
        return util::mkref(this->_const_defs.find(name)->second.first);
    }
    if (this->_external.end() != this->_external.find(name)) {
        return util::mkref(this->_external.find(name)->second);
    }
    if (this->_async_param_defs.end() != this->_async_param_defs.find(name)) {
        return util::mkref(this->_async_param_defs.find(name)->second);
    }
    return util::sref<misc::position const>(nullptr);
}

void SymbolTable::_checkNoRef(misc::position const& pos, std::string const& name)
{
    auto local_refs = this->_references.find(name);
    if (this->_references.end() != local_refs) {
        error::nameRefBeforeDef(pos, std::vector<misc::position>(
            local_refs->second.begin(), local_refs->second.end()), name);
    }
}

void SymbolTable::_checkNoDef(misc::position const& pos, std::string const& name)
{
    util::sref<misc::position const> local_pos(this->_localDefPosOrNul(name));
    if (local_pos.not_nul()) {
        error::nameAlreadyInLocal(misc::position(local_pos->line), pos, name);
    }
}

RegularSymbolTable::RegularSymbolTable(misc::position const& pos
                                     , util::sref<SymbolTable> ext_sym
                                     , std::vector<std::string> const& params)
    : SymbolTable(ext_sym)
{
    for (std::string const& p: params) {
        this->defParam(pos, p);
    }
}

void RegularSymbolTable::defFunc(misc::position const& pos, std::string const& name)
{
    this->defName(pos, name);
    this->_exclude_decls.insert(name);
}

void RegularSymbolTable::defParam(misc::position const& pos, std::string const& name)
{
    this->defName(pos, name);
    this->_exclude_decls.insert(name);
}

std::set<std::string> RegularSymbolTable::localNames() const
{
    std::set<std::string> names;
    for (std::pair<std::string, misc::position> const& def: _name_defs) {
        names.insert(output::formName(def.first));
    }
    for (std::string const& e: this->_exclude_decls) {
        names.erase(output::formName(e));
    }
    return names;
}

util::sptr<output::Expression const> RegularSymbolTable::_makeReference(
    misc::position const& pos, std::string const& name)
{
    return util::mkptr(new output::Reference(pos, name));
}

void GlobalSymbolTable::addExternNames(misc::position const& pos
                                     , std::vector<std::string> const& names)
{
    for (auto const& name: names) {
        this->_checkNoRef(pos, name);
        this->_checkNoDef(pos, name);
        this->_external.insert(std::make_pair(name, pos));
    }
}

void SubSymbolTable::defFunc(misc::position const& pos, std::string const& name)
{
    error::forbidDefFunc(pos, name);
}

std::set<std::string> SubSymbolTable::localNames() const
{
    std::set<std::string> names;
    for (std::pair<std::string, misc::position> const& def: this->_name_defs) {
        names.insert(output::formSubName(def.first, this->id));
    }
    return names;
}

util::sptr<output::Expression const> SubSymbolTable::_makeReference(
    misc::position const& pos, std::string const& name)
{
    return util::mkptr(new output::SubReference(pos, name, this->id));
}
