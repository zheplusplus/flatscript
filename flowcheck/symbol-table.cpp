#include <algorithm>

#include <proto/expr-nodes.h>
#include <util/vector-append.h>
#include <report/errors.h>

#include "symbol-table.h"
#include "function.h"
#include "expr-nodes.h"
#include "global-filter.h"

using namespace flchk;

SymbolTable::SymbolTable(misc::position const& pos
                       , util::sref<SymbolTable const> ext_sym
                       , std::vector<std::string> const& params)
    : ext_symbols(ext_sym)
{
    std::for_each(params.begin()
                , params.end()
                , [&](std::string const& param)
                  {
                      defVar(pos, param);
                  });
}

void SymbolTable::refVars(misc::position const& pos, std::vector<std::string> const& vars)
{
    std::for_each(vars.begin()
                , vars.end()
                , [&](std::string const& name)
                  {
                      _markReference(pos, name);
                  });
}

void SymbolTable::defVar(misc::position const& pos, std::string const& name)
{
    auto local_refs = _external_var_refs.find(name);
    if (_external_var_refs.end() != local_refs) {
        error::varRefBeforeDef(pos, local_refs->second, name);
    }
    auto insert_result = _var_defs.insert(std::make_pair(name, pos));
    if (!insert_result.second) {
        error::varAlreadyInLocal(insert_result.first->second, pos, name);
    }
}

std::vector<util::sptr<proto::Expression const>> SymbolTable::_mkArgs(
              std::vector<util::sptr<Expression const>> const& args)
{
    std::vector<util::sptr<proto::Expression const>> arguments;
    arguments.reserve(args.size());
    std::for_each(args.begin()
                , args.end()
                , [&](util::sptr<Expression const> const& expr)
                  {
                      arguments.push_back(expr->compile(util::mkref(*this)));
                  });
    return std::move(arguments);
}

util::sptr<proto::Expression const> SymbolTable::compileRef(misc::position const& pos
                                                          , std::string const& name)
{
    _markReference(pos, name);
    return util::mkptr(new proto::Reference(pos, name));
}

util::sptr<proto::Expression const> SymbolTable::compileCall(
                    misc::position const& pos
                  , std::string const& name
                  , std::vector<util::sptr<Expression const>> const& args)
{
    _markReference(pos, name);
    return util::mkptr(new proto::Call(pos, name, _mkArgs(args)));
}

void SymbolTable::_markReference(misc::position const& pos, std::string const& name)
{
    if (_var_defs.end() == _var_defs.find(name)) {
        _external_var_refs[name].push_back(pos);
        _checkDefinition(pos, name);
    }
}

void SymbolTable::_checkDefinition(misc::position const& pos, std::string const& name) const
{
    if (_var_defs.end() == _var_defs.find(name)) {
        if (ext_symbols.nul()) {
            error::varNotDef(pos, name);
        } else {
            ext_symbols->_checkDefinition(pos, name);
        }
    }
}

Function SymbolTable::_fake_function(misc::position()
                                   , ""
                                   , std::vector<std::string>()
                                   , util::mkptr(new GlobalFilter));
