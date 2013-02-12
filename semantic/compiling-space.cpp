#include <algorithm>
#include <map>

#include <output/expr-nodes.h>
#include <output/stmt-nodes.h>
#include <output/list-pipe.h>
#include <output/function.h>
#include <report/errors.h>

#include "compiling-space.h"
#include "const-fold.h"

using namespace semantic;

namespace {

    struct RegularSymbolTable
        : SymbolTable
    {
        explicit RegularSymbolTable(util::sref<SymbolTable const> ext_sym)
            : ext_symbols(ext_sym)
        {}

        RegularSymbolTable(misc::position const& pos
                         , util::sref<SymbolTable const> ext_sym
                         , std::vector<std::string> const& params)
            : ext_symbols(ext_sym)
        {
            std::for_each(params.begin()
                        , params.end()
                        , [&](std::string const& param)
                          {
                              defParam(pos, param);
                          });
        }

        RegularSymbolTable()
            : ext_symbols(nullptr)
        {}

        void defName(misc::position const& pos, std::string const& name)
        {
            _checkNoRef(pos, name);
            _checkNoDef(pos, name);
            _name_defs.insert(std::make_pair(name, pos));
        }

        void defFunc(misc::position const& pos, std::string const& name)
        {
            defName(pos, name);
            _exclude_decls.insert(name);
        }

        void defParam(misc::position const& pos, std::string const& name)
        {
            defName(pos, name);
            _exclude_decls.insert(name);
        }

        void defAsyncParam(misc::position const& pos, std::string const& name)
        {
            defName(pos, name);
        }

        void defConst(misc::position const& pos
                    , std::string const& name
                    , util::sref<Expression const> value)
        {
            _checkNoRef(pos, name);
            _checkNoDef(pos, name);
            _const_defs.insert(std::make_pair(name, std::make_pair(pos, value)));
        }

        void imported(misc::position const& pos, std::string const& name)
        {
            _checkNoRef(pos, name);
            _checkNoDef(pos, name);
            _imported.insert(std::make_pair(name, pos));
        }

        void refNames(misc::position const& pos, std::vector<std::string> const& names)
        {
            std::for_each(names.begin()
                        , names.end()
                        , [&](std::string const& name)
                          {
                              _reference(pos, name);
                          });
        }

        util::sref<Expression const> literalOrNul(std::string const& name) const
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

        bool isImported(std::string const& name) const
        {
            if (_imported.find(name) != _imported.end()) {
                return true;
            }
            if (ext_symbols.nul()) {
                return false;
            }
            return ext_symbols->isImported(name);
        }

        util::sptr<output::Expression const> compileRef(misc::position const& pos
                                                      , std::string const& name)
        {
            _reference(pos, name);
            util::sref<Expression const> literal(literalOrNul(name));
            if (literal.not_nul()) {
                return compileLiteral(literal, util::mkref(*this));
            }
            if (isImported(name)) {
                return util::mkptr(new output::ImportedName(pos, name));
            }
            return util::mkptr(new output::Reference(pos, name));
        }

        void checkDefinition(misc::position const& pos, std::string const& name) const
        {
            if (_localDefPosOrNul(name).nul()) {
                if (ext_symbols.nul()) {
                    error::nameNotDef(pos, name);
                } else {
                    ext_symbols->checkDefinition(pos, name);
                }
            }
        }

        std::set<std::string> localNames() const
        {
            std::set<std::string> names;
            std::for_each(_name_defs.begin()
                        , _name_defs.end()
                        , [&](std::pair<std::string, misc::position> const& def)
                          {
                              names.insert(def.first);
                          });
            std::for_each(_exclude_decls.begin()
                        , _exclude_decls.end()
                        , [&](std::string const& e)
                          {
                              names.erase(e);
                          });
            return names;
        }
    public:
        util::sref<SymbolTable const> const ext_symbols;
    private:
        util::sref<misc::position const> _localDefPosOrNul(std::string const& name) const
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

        void _checkNoRef(misc::position const& pos, std::string const& name)
        {
            auto local_refs = _external_name_refs.find(name);
            if (_external_name_refs.end() != local_refs) {
                error::nameRefBeforeDef(pos, std::vector<misc::position>(
                                    local_refs->second.begin(), local_refs->second.end()), name);
            }
        }

        void _checkNoDef(misc::position const& pos, std::string const& name)
        {
            util::sref<misc::position const> local_pos(_localDefPosOrNul(name));
            if (local_pos.not_nul()) {
                error::nameAlreadyInLocal(local_pos.cp(), pos, name);
            }
        }

        void _reference(misc::position const& pos, std::string const& name)
        {
            if (_name_defs.end() == _name_defs.find(name)) {
                _external_name_refs[name].push_back(pos);
                checkDefinition(pos, name);
            }
        }
    private:
        std::map<std::string, std::vector<misc::position>> _external_name_refs;
        std::map<std::string, misc::position> _name_defs;
        std::map<std::string, misc::position> _imported;
        std::map<std::string, std::pair<misc::position, util::sref<Expression const>>> _const_defs;
        std::vector<std::string> const _parameters;
        std::set<std::string> _exclude_decls;
    };

    struct ReferralSymbolTable
        : SymbolTable
    {
        explicit ReferralSymbolTable(util::sref<SymbolTable> ext_sym)
            : _ext_sym(ext_sym)
        {}

        void defName(misc::position const& pos, std::string const& name)
        {
            error::forbidDefName(pos, name);
        }

        void defFunc(misc::position const& pos, std::string const& name)
        {
            defName(pos, name);
        }

        void defParam(misc::position const& pos, std::string const& name)
        {
            defName(pos, name);
        }

        void defAsyncParam(misc::position const& pos, std::string const& name)
        {
            if (_async_param_defs.end() == _async_param_defs.find(name)) {
                _async_param_defs.insert(std::make_pair(name, pos));
            } else {
                error::nameAlreadyInLocal(_async_param_defs.find(name)->second, pos, name);
            }
        }

        void defConst(misc::position const& pos
                    , std::string const& name
                    , util::sref<Expression const>)
        {
            error::forbidDefName(pos, name);
        }

        void imported(misc::position const& pos, std::string const& name)
        {
            error::forbidDefName(pos, name);
        }

        void refNames(misc::position const& pos, std::vector<std::string> const& names)
        {
            _ext_sym->refNames(pos, names);
        }

        util::sref<Expression const> literalOrNul(std::string const& name) const
        {
            if (_async_param_defs.end() == _async_param_defs.find(name)) {
                return _ext_sym->literalOrNul(name);
            }
            return util::sref<Expression const>(nullptr);
        }

        bool isImported(std::string const& name) const
        {
            if (_async_param_defs.end() == _async_param_defs.find(name)) {
                return _ext_sym->isImported(name);
            }
            return false;
        }

        util::sptr<output::Expression const> compileRef(misc::position const& pos
                                                      , std::string const& name)
        {
            if (_async_param_defs.end() == _async_param_defs.find(name)) {
                return _ext_sym->compileRef(pos, name);
            }
            return util::mkptr(new output::Reference(pos, name));
        }

        void checkDefinition(misc::position const& pos, std::string const& name) const
        {
            if (_async_param_defs.end() == _async_param_defs.find(name)) {
                _ext_sym->checkDefinition(pos, name);
            }
        }

        std::set<std::string> localNames() const
        {
            return std::set<std::string>();
        }
    private:
        util::sref<SymbolTable> const _ext_sym;
        std::map<std::string, misc::position> _async_param_defs;
    };

}

BaseCompilingSpace::BaseCompilingSpace(util::sptr<SymbolTable> symbols)
    : _terminated(false)
    , _symbols(std::move(symbols))
    , _main_block(new output::Block)
    , _current_block(*_main_block)
{}

util::sref<SymbolTable> BaseCompilingSpace::sym()
{
    return *_symbols;
}

util::sref<output::Block> BaseCompilingSpace::block() const
{
    return _current_block;
}

void BaseCompilingSpace::terminate()
{
    _terminated = true;
}

bool BaseCompilingSpace::terminated() const
{
    return _terminated;
}

void BaseCompilingSpace::setAsyncSpace(misc::position const& pos
                                     , std::vector<std::string> const& params
                                     , util::sref<output::Block> block)
{
    setAsyncSpace(block);
    std::for_each(params.begin()
                , params.end()
                , [&](std::string const& param)
                  {
                      _symbols->defAsyncParam(pos, param);
                  });
}

void BaseCompilingSpace::setAsyncSpace(util::sref<output::Block> block)
{
    _current_block = block;
}

util::sptr<output::Expression const> BaseCompilingSpace::ret(util::sref<Expression const> val)
{
    return val->compile(*this);
}

output::Method BaseCompilingSpace::raiseMethod() const
{
    return output::method::throwExc();
}

util::sptr<output::Block> BaseCompilingSpace::deliver()
{
    _main_block->setLocalDecls(_symbols->localNames());
    return std::move(_main_block);
}

CompilingSpace::CompilingSpace()
    : BaseCompilingSpace(util::mkptr(new RegularSymbolTable))
    , _this_referenced(false)
{}

CompilingSpace::CompilingSpace(misc::position const& pos
                             , util::sref<SymbolTable> ext_st
                             , std::vector<std::string> const& params)
    : BaseCompilingSpace(util::mkptr(new RegularSymbolTable(pos, ext_st, params)))
    , _this_referenced(false)
{}

void CompilingSpace::referenceThis()
{
    _this_referenced = true;
}

util::sptr<output::Block> CompilingSpace::deliver()
{
    if (_this_referenced) {
        util::sptr<output::Block> block(new output::Block);
        block->addStmt(util::mkptr(new output::ThisDeclaration));
        block->append(BaseCompilingSpace::deliver());
        return std::move(block);
    }
    return BaseCompilingSpace::deliver();
}

util::sptr<output::Expression const> RegularAsyncCompilingSpace::ret(
                                        util::sref<Expression const> val)
{
    misc::position const pos(val->pos);
    return util::mkptr(new output::RegularAsyncReturnCall(pos, val->compile(*this)));
}

output::Method RegularAsyncCompilingSpace::raiseMethod() const
{
    return output::method::callbackExc();
}

util::sptr<output::Block> RegularAsyncCompilingSpace::deliver()
{
    if (!terminated()) {
        block()->addStmt(util::mkptr(new output::Return(util::mkptr(
                        new output::RegularAsyncReturnCall(compile_pos, util::mkptr(
                                                          new output::Undefined(compile_pos)))))));
    }
    return CompilingSpace::deliver();
}

PipelineSpace::PipelineSpace(BaseCompilingSpace& ext_space)
    : BaseCompilingSpace(util::mkptr(new RegularSymbolTable(ext_space.sym())))
    , _ext_space(ext_space)
{}

void PipelineSpace::referenceThis()
{
    _ext_space.referenceThis();
}

util::sptr<output::Expression const> PipelineSpace::ret(util::sref<Expression const> val)
{
    error::returnNotAllowedInPipe(val->pos);
    return BaseCompilingSpace::ret(val);
}

output::Method PipelineSpace::raiseMethod() const
{
    return _ext_space.raiseMethod();
}

util::sptr<output::Block> AsyncPipelineSpace::deliver()
{
    block()->addStmt(util::mkptr(new output::PipelineContinue));
    return BaseCompilingSpace::deliver();
}

SubCompilingSpace::SubCompilingSpace(BaseCompilingSpace& ext_space)
    : BaseCompilingSpace(util::mkptr(new ReferralSymbolTable(ext_space.sym())))
    , _ext_space(ext_space)
{}

bool SubCompilingSpace::inPipe() const
{
    return _ext_space.inPipe();
}

void SubCompilingSpace::referenceThis()
{
    _ext_space.referenceThis();
}

util::sptr<output::Expression const> SubCompilingSpace::ret(util::sref<Expression const> val)
{
    return _ext_space.ret(val);
}

output::Method SubCompilingSpace::raiseMethod() const
{
    return _ext_space.raiseMethod();
}
