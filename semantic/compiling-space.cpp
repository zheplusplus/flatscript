#include <algorithm>
#include <map>

#include <output/expr-nodes.h>
#include <output/stmt-nodes.h>
#include <output/list-pipe.h>
#include <output/function.h>
#include <output/class.h>
#include <output/name-mangler.h>
#include <report/errors.h>

#include "compiling-space.h"
#include "const-fold.h"

using namespace semantic;

namespace {

    struct BasicSymbolTable
        : SymbolTable
    {
        explicit BasicSymbolTable(util::sref<SymbolTable> ext_sym)
            : ext_symbols(ext_sym)
        {}

        void defName(misc::position const& pos, std::string const& name)
        {
            _checkNoRef(pos, name);
            _checkNoDef(pos, name);
            _name_defs.insert(std::make_pair(name, pos));
        }

        void defAsyncParam(misc::position const& pos, std::string const& name)
        {
            _checkNoRef(pos, name);
            _checkNoDef(pos, name);
            _async_param_defs.insert(std::make_pair(name, pos));
        }

        void defConst(misc::position const& pos
                    , std::string const& name
                    , util::sref<Expression const> value)
        {
            _checkNoRef(pos, name);
            _checkNoDef(pos, name);
            _const_defs.insert(std::make_pair(name, std::make_pair(pos, value)));
        }

        void addExternNames(misc::position const& pos, std::vector<std::string> const&)
        {
            error::externOnlyInGlobal(pos);
        }

        void refNames(misc::position const& pos, std::vector<std::string> const& names)
        {
            for (auto const& name: names) {
                _references[name].push_back(pos);
            }
        }

        util::sref<Expression const> literalOrNul(std::string const& name) const
        {
            auto find_result = _const_defs.find(name);
            if (find_result != _const_defs.end()) {
                return find_result->second.second;
            }
            if (_localDefPosOrNul(name).not_nul()) {
                return util::sref<Expression const>(nullptr);
            }
            if (ext_symbols.nul()) {
                return util::sref<Expression const>(nullptr);
            }
            return ext_symbols->literalOrNul(name);
        }

        util::sptr<output::Expression const> compileRef(misc::position const& pos
                                                      , std::string const& name)
        {
            _references[name].push_back(pos);
            auto const_find_result = _const_defs.find(name);
            if (_const_defs.end() != const_find_result) {
                return compileLiteral(const_find_result->second.second, util::mkref(*this));
            }
            if (_external.end() != _external.find(name)) {
                return util::mkptr(new output::ImportedName(pos, name));
            }
            if (_async_param_defs.find(name) != _async_param_defs.end()) {
                return util::mkptr(new output::TransientParamReference(pos, name));
            }
            if (_name_defs.end() != _name_defs.find(name)) {
                return _makeReference(pos, name);
            }
            if (ext_symbols.not_nul()) {
                return ext_symbols->compileRef(pos, name);
            }
            error::nameNotDef(pos, name);
            return _makeReference(pos, name);
        }
    protected:
        util::sref<SymbolTable> const ext_symbols;

        virtual util::sptr<output::Expression const> _makeReference(
            misc::position const& pos, std::string const& name) = 0;

        util::sref<misc::position const> _localDefPosOrNul(std::string const& name) const
        {
            if (_name_defs.end() != _name_defs.find(name)) {
                return util::mkref(_name_defs.find(name)->second);
            }
            if (_const_defs.end() != _const_defs.find(name)) {
                return util::mkref(_const_defs.find(name)->second.first);
            }
            if (_external.end() != _external.find(name)) {
                return util::mkref(_external.find(name)->second);
            }
            if (_async_param_defs.end() != _async_param_defs.find(name)) {
                return util::mkref(_async_param_defs.find(name)->second);
            }
            return util::sref<misc::position const>(nullptr);
        }

        void _checkNoRef(misc::position const& pos, std::string const& name)
        {
            auto local_refs = _references.find(name);
            if (_references.end() != local_refs) {
                error::nameRefBeforeDef(pos, std::vector<misc::position>(
                    local_refs->second.begin(), local_refs->second.end()), name);
            }
        }

        void _checkNoDef(misc::position const& pos, std::string const& name)
        {
            util::sref<misc::position const> local_pos(_localDefPosOrNul(name));
            if (local_pos.not_nul()) {
                error::nameAlreadyInLocal(misc::position(local_pos->line), pos, name);
            }
        }
    protected:
        std::map<std::string, std::vector<misc::position>> _references;
        std::map<std::string, misc::position> _name_defs;
        std::map<std::string, misc::position> _async_param_defs;
        std::map<std::string, misc::position> _external;
        std::map<std::string, std::pair<misc::position, util::sref<Expression const>>> _const_defs;
        std::vector<std::string> const _parameters;
    };

    struct RegularSymbolTable
        : BasicSymbolTable
    {
        explicit RegularSymbolTable(util::sref<SymbolTable> ext_sym)
            : BasicSymbolTable(ext_sym)
        {}

        RegularSymbolTable(misc::position const& pos
                         , util::sref<SymbolTable> ext_sym
                         , std::vector<std::string> const& params)
            : BasicSymbolTable(ext_sym)
        {
            for (std::string const& p: params) {
                defParam(pos, p);
            }
        }

        void defFunc(misc::position const& pos, std::string const& name)
        {
            defName(pos, name);
            _exclude_decls.insert(name);
        }

        void defClass(misc::position const& pos, std::string const& name
                    , std::string const& base_class_name)
        {
            if (!base_class_name.empty()) {
                this->compileRef(pos, base_class_name);
            }
            this->defName(pos, name);
        }

        void defParam(misc::position const& pos, std::string const& name)
        {
            defName(pos, name);
            _exclude_decls.insert(name);
        }

        std::set<std::string> localNames() const
        {
            std::set<std::string> names;
            for (std::pair<std::string, misc::position> const& def: _name_defs) {
                names.insert(output::formName(def.first));
            }
            for (std::string const& e: _exclude_decls) {
                names.erase(output::formName(e));
            }
            return names;
        }
    protected:
        RegularSymbolTable()
            : BasicSymbolTable(util::sref<SymbolTable>(nullptr))
        {}
    private:
        util::sptr<output::Expression const> _makeReference(
            misc::position const& pos, std::string const& name)
        {
            return util::mkptr(new output::Reference(pos, name));
        }

        std::set<std::string> _exclude_decls;
    };

    struct GlobalSymbolTable
        : RegularSymbolTable
    {
        GlobalSymbolTable() = default;

        void addExternNames(misc::position const& pos, std::vector<std::string> const& names)
        {
            for (auto const& name: names) {
                _checkNoRef(pos, name);
                _checkNoDef(pos, name);
                _external.insert(std::make_pair(name, pos));
            }
        }
    };

    struct SubSymbolTable
        : BasicSymbolTable
    {
        explicit SubSymbolTable(util::sref<SymbolTable> ext_sym)
            : BasicSymbolTable(ext_sym)
        {}

        void defFunc(misc::position const& pos, std::string const& name)
        {
            error::forbidDefFunc(pos, name);
        }

        void defClass(misc::position const& pos, std::string const& name, std::string const&)
        {
            error::forbidDefClass(pos, name);
        }

        void defParam(misc::position const&, std::string const&) {}

        std::set<std::string> localNames() const
        {
            std::set<std::string> names;
            for (std::pair<std::string, misc::position> const& def: _name_defs) {
                names.insert(output::formSubName(def.first, util::id(this)));
            }
            return names;
        }
    private:
        util::sptr<output::Expression const> _makeReference(
            misc::position const& pos, std::string const& name)
        {
            return util::mkptr(new output::SubReference(pos, name, util::id(this)));
        }
    };

}

BaseCompilingSpace::BaseCompilingSpace(util::sptr<SymbolTable> symbols, bool allow_super)
    : _allow_super(allow_super)
    , _terminated_err_reported(false)
    , _term_pos_or_nul_if_not_term(nullptr)
    , _symbols(std::move(symbols))
    , _main_block(new output::Block)
    , _current_block(*_main_block)
{}

util::sref<SymbolTable> BaseCompilingSpace::sym()
{
    return *_symbols;
}

void BaseCompilingSpace::addStmt(misc::position const& pos
                               , util::sptr<output::Statement const> stmt)
{
    _testOrReportTerminated(pos);
    _current_block->addStmt(std::move(stmt));
}

util::sref<output::Block> BaseCompilingSpace::block() const
{
    return _current_block;
}

void BaseCompilingSpace::terminate(misc::position const& pos)
{
    _term_pos_or_nul_if_not_term.reset(new misc::position(pos));
}

bool BaseCompilingSpace::terminated() const
{
    return _term_pos_or_nul_if_not_term.not_nul();
}

void BaseCompilingSpace::setAsyncSpace(misc::position const& pos
                                     , std::vector<std::string> const& params
                                     , util::sref<output::Block> block)
{
    std::for_each(params.begin()
                , params.end()
                , [&](std::string const& param)
                  {
                      _symbols->defAsyncParam(pos, param);
                  });
    _current_block = replaceSpace(pos, block);
}

util::sref<output::Block> BaseCompilingSpace::replaceSpace(
        misc::position const&, util::sref<output::Block> block)
{
    return block;
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

void BaseCompilingSpace::_testOrReportTerminated(misc::position const& pos)
{
    if (terminated() && !_terminated_err_reported) {
        error::flowTerminated(pos, misc::position(_term_pos_or_nul_if_not_term->line));
        _terminated_err_reported = true;
    }
}

CompilingSpace::CompilingSpace()
    : BaseCompilingSpace(util::mkptr(new GlobalSymbolTable))
    , _this_referenced(false)
{}

CompilingSpace::CompilingSpace(misc::position const& pos
                             , util::sref<SymbolTable> ext_st
                             , std::vector<std::string> const& params
                             , bool allow_super)
    : BaseCompilingSpace(util::mkptr(new RegularSymbolTable(pos, ext_st, params)), allow_super)
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

SubCompilingSpace::SubCompilingSpace(BaseCompilingSpace& ext_space)
    : SubCompilingSpace(util::mkptr(new SubSymbolTable(ext_space.sym())), ext_space)
{}

bool SubCompilingSpace::inPipe() const
{
    return _ext_space.inPipe();
}

bool SubCompilingSpace::inCatch() const
{
    return _ext_space.inCatch();
}

bool SubCompilingSpace::allowSuper() const
{
    return _ext_space.allowSuper();
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

util::sref<output::Block> SubCompilingSpace::replaceSpace(
        misc::position const& pos, util::sref<output::Block> block)
{
    return _ext_space.replaceSpace(pos, block);
}

util::sptr<output::Expression const> BaseCompilingSpace::makeReference(
    misc::position const& pos, std::string const& name)
{
    return sym()->compileRef(pos, name);
}

RegularSubCompilingSpace::RegularSubCompilingSpace(BaseCompilingSpace& ext_space)
    : SubCompilingSpace(util::mkptr(new RegularSymbolTable(ext_space.sym())), ext_space)
{}

PipelineSpace::PipelineSpace(BaseCompilingSpace& ext_space)
    : SubCompilingSpace(util::mkptr(new RegularSymbolTable(ext_space.sym())), ext_space)
{}

util::sptr<output::Expression const> PipelineSpace::ret(util::sref<Expression const> val)
{
    error::returnNotAllowedInPipe(val->pos);
    return BaseCompilingSpace::ret(val);
}

util::sptr<output::Block> AsyncPipelineSpace::deliver()
{
    block()->addStmt(util::mkptr(new output::PipelineContinue));
    return BaseCompilingSpace::deliver();
}

output::Method AsyncTrySpace::raiseMethod() const
{
    return output::method::asyncCatcher(catch_func->mangledName());
}

util::sref<output::Block> AsyncTrySpace::replaceSpace(
        misc::position const& pos, util::sref<output::Block> block)
{
    util::sptr<output::Block> try_block(new output::Block);
    util::sref<output::Block> try_flow(*try_block);

    BaseCompilingSpace& me = *this;
    RegularSubCompilingSpace staller(me);
    util::ptrarr<output::Expression const> args;
    args.append(util::mkptr(new output::ExceptionObj(pos)));
    staller.addStmt(pos, util::mkptr(
            new output::Arithmetics(catch_func->callMe(pos, std::move(args)))));

    block->addStmt(util::mkptr(new output::ExceptionStall(
                    std::move(try_block), staller.deliver())));
    return try_flow;
}
