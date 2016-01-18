#include <output/expr-nodes.h>
#include <output/stmt-nodes.h>
#include <output/list-pipe.h>
#include <output/name-mangler.h>
#include <output/function-impl.h>
#include <report/errors.h>
#include <globals.h>
#include <including.h>

#include "scope-impl.h"
#include "common.h"
#include "function.h"

using namespace semantic;

Scope::Scope()
    : _terminated_err_reported(false)
    , _term_pos_or_nul_if_not_term(nullptr)
    , _main_block(new output::Block)
    , _current_block(*_main_block)
{}

void Scope::addStmt(misc::position const& pos, util::sptr<output::Statement const> stmt)
{
    this->checkNotTerminated(pos);
    this->_current_block->addStmt(std::move(stmt));
}

void Scope::checkNotTerminated(misc::position const& pos)
{
    if (this->terminated() && !this->_terminated_err_reported) {
        error::flowTerminated(pos, misc::position(this->_term_pos_or_nul_if_not_term->line));
        this->_terminated_err_reported = true;
    }
}

void Scope::terminate(misc::position const& pos)
{
    this->_term_pos_or_nul_if_not_term.reset(new misc::position(pos));
}

void Scope::setAsyncSpace(misc::position const& pos
                        , std::vector<std::string> const& params
                        , util::sref<output::Block> block)
{
    for (std::string const& p: params) {
        this->sym()->defAsyncParam(pos, p);
    }
    this->_current_block = this->replaceSpace(block);
}

util::sref<output::Block> Scope::replaceSpace(util::sref<output::Block> block)
{
    return block;
}

util::sptr<output::Block> Scope::deliver()
{
    this->_main_block->setLocalDecls(this->sym()->localNames());
    return std::move(this->_main_block);
}

void DomainScope::addClass(misc::position const& pos
                         , std::string const& class_name
                         , util::sptr<output::Expression const> base_class
                         , util::sptr<output::Constructor const> ctor
                         , std::map<std::string, util::sref<Function const>> memfns)
{
    this->_class_inits.push_back(ClassInitFunc(pos, class_name, base_class.not_nul()
                                             , std::move(ctor), std::move(memfns)));
    this->sym()->defName(pos, class_name);
    this->addStmt(pos, util::mkptr(new output::ClassInitCall(class_name, std::move(base_class))));
}

output::Method DomainScope::breakMethod(misc::position const& p)
{
    error::invalidBreak(p, "break");
    return output::method::place();
}

output::Method DomainScope::continueMethod(misc::position const& p)
{
    error::invalidBreak(p, "continue");
    return output::method::place();
}

static util::uid const NUL_ID(util::uid::next_id());

util::uid DomainScope::scopeId() const
{
    return NUL_ID;
}

util::sptr<output::Block> DomainScope::deliver()
{
    if (this->_class_inits.empty()) {
        return Scope::deliver();
    }
    util::sptr<output::Block> block(new output::Block);
    for (ClassInitFunc& i: this->_class_inits) {
        std::map<std::string, util::sptr<output::Expression const>> memfuncs;
        for (auto name_fn: i.memfns) {
            memfuncs.insert(std::make_pair(
                name_fn.first, name_fn.second->compileToLambda(util::mkref(*this), true)));
        }
        block->addStmt(util::mkptr(new output::ClassInitFunc(
                i.class_name, i.inherit, std::move(memfuncs), std::move(i.ctor))));
    }
    block->append(Scope::deliver());
    return std::move(block);
}

util::sptr<output::Block> BasicFunctionScope::deliver()
{
    if (this->_this_referenced) {
        util::sptr<output::Block> block(new output::Block);
        block->addStmt(util::mkptr(new output::ThisDeclaration));
        block->append(DomainScope::deliver());
        return std::move(block);
    }
    return DomainScope::deliver();
}

output::Method SyncFunctionScope::retMethod(misc::position const& p)
{
    this->_first_return_or_nul_if_not_returned.reset(new misc::position(p));
    return output::method::ret();
}

output::Method SyncFunctionScope::throwMethod() const
{
    return output::method::throwExc();
}

RegularAsyncFuncScope::RegularAsyncFuncScope(misc::position const& pos
                                           , util::sref<Scope> parent
                                           , std::vector<std::string> const& params
                                           , bool allow_super)
    : BasicFunctionScope(pos, parent, params, allow_super)
    , ret_pos(pos)
{
    this->_current_block = this->replaceSpace(this->_current_block);
}

output::Method RegularAsyncFuncScope::retMethod(misc::position const& p)
{
    this->_first_return_or_nul_if_not_returned.reset(new misc::position(p));
    return output::method::asyncRet();
}

output::Method RegularAsyncFuncScope::throwMethod() const
{
    return output::method::callbackExc();
}

util::sptr<output::Block> RegularAsyncFuncScope::deliver()
{
    if (!this->terminated()) {
        this->block()->addStmt(util::mkptr(new output::ExprScheme(
            output::method::asyncRet(), util::mkptr(new output::Undefined))));
    }
    return BasicFunctionScope::deliver();
}

typedef std::function<util::sptr<output::Statement const>(
            util::sptr<output::Expression const>)> MakeCatcher;

static util::sref<output::Block> _replaceSpaceInAsyncTry(
                util::sref<output::Block> block, MakeCatcher make_catcher)
{
    util::sptr<output::Block> try_block(new output::Block);
    util::sref<output::Block> try_flow(*try_block);

    block->addStmt(util::mkptr(new output::ExceptionStallDeprecated(
                std::move(try_block)
              , make_catcher(util::mkptr(new output::ExceptionObj)))));
    return try_flow;
}

util::sref<output::Block> RegularAsyncFuncScope::replaceSpace(util::sref<output::Block> block)
{
    return ::_replaceSpaceInAsyncTry(
        block, [this](util::sptr<output::Expression const> exception)
               {
                   return util::mkptr(new output::ExprScheme(
                            this->throwMethod(), std::move(exception)));
               });
}

void SubScope::addClass(misc::position const& pos
                      , std::string const& class_name
                      , util::sptr<output::Expression const>
                      , util::sptr<output::Constructor const>
                      , std::map<std::string, util::sref<Function const>>)
{
    error::forbidDefClass(pos, class_name);
}

CatchScope::CatchScope(util::sref<Scope> parent_scope, misc::position const& pos,
                       std::string except_name)
    : BranchingSubScope(parent_scope)
{
    this->sym()->defName(pos, except_name);
}

output::Method AsyncTryScope::throwMethod() const
{
    return output::method::asyncCatcher(catch_func->mangledName());
}

util::sref<output::Block> AsyncTryScope::replaceSpace(util::sref<output::Block> block)
{
    return ::_replaceSpaceInAsyncTry(
        block, [&](util::sptr<output::Expression const> exception)
               {
                   return makeArith(this->catch_func->callMe(std::move(exception)));
               });
}

namespace {

    struct FileScope
        : DomainScope
    {
        FileScope()
        {
            this->sym()->addExternNames(misc::position(), std::vector<std::string>(
                flats::Globals::g.external_syms.begin(), flats::Globals::g.external_syms.end()));
        }

        bool inPipe() const { return false; }
        bool inCatch() const { return false; }
        bool allowSuper() const { return false; }
        void allowSuper(bool) {}

        void referenceThis(misc::position const& pos)
        {
            error::referenceThisInGlobal(pos);
        }

        output::Method retMethod(misc::position const& pos)
        {
            error::returnInGlobal(pos);
            return output::method::ret();
        }

        output::Method throwMethod() const
        {
            return output::method::throwExc();
        }

        util::sref<misc::position const> firstReturn() const
        {
            return util::sref<misc::position const>(nullptr);
        }

        util::sref<SymbolTable> sym()
        {
            return util::mkref(this->_sym);
        }
    private:
        GlobalSymbolTable _sym;
    };

    struct IncludeFileScope
        : FileScope
    {
        IncludeFileScope(util::sref<Scope> parent, util::uid fid)
            : file_id(fid)
            , _parent(parent)
        {}

        util::uid scopeId() const
        {
            return this->file_id;
        }

        util::uid includeFile(misc::position const& p, std::string const& file)
        {
            return this->_parent->includeFile(p, file);
        }

        void setAsyncSpace(misc::position const& pos
                         , std::vector<std::string> const&
                         , util::sref<output::Block>)
        {
            error::asyncNotAllowedInIncludedFile(pos);
        }

        util::uid const file_id;
    private:
        util::sref<Scope> const _parent;
    };

    struct GlobalScope
        : FileScope
    {
        struct IncludedFile {
            util::uid const include_id;
            util::sptr<semantic::Statement const> file;

            IncludedFile(util::uid i, util::sptr<semantic::Statement const> f)
                : include_id(i)
                , file(std::move(f))
            {}

            IncludedFile(IncludedFile&& rhs)
                : include_id(rhs.include_id)
                , file(std::move(rhs.file))
            {}
        };

        util::uid includeFile(misc::position const& p, std::string const& file)
        {
            auto f(this->_includings.find(file));
            if (f != this->_includings.end()) {
                return f->second.include_id;
            }
            util::uid include_id(util::uid::next_id());
            this->_includings.insert(std::make_pair(
                        file, IncludedFile(include_id, flats::compileFile(file, p))));
            return include_id;
        }

        util::sptr<output::Block> deliver()
        {
            std::set<std::string> inc_decls;
            std::set<std::string> compiled;
            std::vector<util::sptr<output::Function const>> init_funcs;
            std::vector<util::sptr<output::Expression const>> init_calls;
            while (compiled.size() != this->_includings.size()) {
                for (auto& i: this->_includings) {
                    if (compiled.find(i.first) == compiled.end()) {
                        compiled.insert(i.first);
                        this->_compileInclude(init_funcs, init_calls, inc_decls, i.second);
                    }
                }
            }
            util::sptr<output::Block> g(new output::Block);
            g->setLocalDecls(inc_decls);
            for (auto& f: init_funcs) {
                g->addFunc(std::move(f));
            }
            for (auto i = init_calls.rbegin(); i != init_calls.rend(); ++i) {
                g->addStmt(makeArith(std::move(*i)));
            }
            g->append(FileScope::deliver());
            return std::move(g);
        }
    private:
        void _compileInclude(std::vector<util::sptr<output::Function const>>& init_funcs,
                             std::vector<util::sptr<output::Expression const>>& init_calls,
                             std::set<std::string>& inc_decls, IncludedFile const& inc)
        {
            util::uid include_id(inc.include_id);
            inc_decls.insert(output::formModuleExportName(inc.include_id));
            IncludeFileScope file_scope(util::mkref(*this), inc.include_id);
            inc.file->compile(file_scope);
            util::sptr<output::ModuleInitFunc const> f(new output::ModuleInitFunc(
                            inc.include_id, file_scope.deliver()));
            init_calls.push_back(f->callMe(f->exportArg()));
            init_funcs.push_back(std::move(f));
        }

        std::map<std::string, IncludedFile> _includings;
    };

    struct PipelineScope
        : SubScope
    {
        PipelineScope(util::sref<Scope> parent_scope, util::uid pid, bool root)
            : SubScope(parent_scope)
            , pipe_id(pid)
            , _sym(parent_scope->sym())
            , _first_return_or_nul_if_not_returned(nullptr)
            , _root(root)
            , _has_break(false)
        {}

        bool inPipe() const { return true; }

        output::Method retMethod(misc::position const& p)
        {
            if (!this->_root) {
                error::returnNotAllowedInExprPipe(p);
            }
            this->_first_return_or_nul_if_not_returned.reset(new misc::position(p));
            return this->_retMethod(p);
        }

        output::Method breakMethod(misc::position const& p)
        {
            if (!this->_root) {
                error::invalidBreak(p, "break");
            }
            this->_has_break = true;
            return this->_breakMethod();
        }

        output::Method continueMethod(misc::position const& p)
        {
            if (!this->_root) {
                error::invalidBreak(p, "continue");
            }
            return this->_continueMethod();
        }

        util::sref<misc::position const> firstReturn() const
        {
            return *this->_first_return_or_nul_if_not_returned;
        }

        bool hasBreak() const
        {
            return this->_has_break;
        }

        util::sref<SymbolTable> sym()
        {
            return util::mkref(this->_sym);
        }

        util::uid scopeId() const
        {
            return this->pipe_id;
        }

        util::uid const pipe_id;
    private:
        virtual output::Method _retMethod(misc::position const&)
        {
            return output::method::syncPipeRet(this->pipe_id);
        }

        virtual output::Method _breakMethod()
        {
            return output::method::syncBreak();
        }

        virtual output::Method _continueMethod()
        {
            return output::method::ret();
        }

        RegularSymbolTable _sym;
        util::sptr<misc::position const> _first_return_or_nul_if_not_returned;
        bool const _root;
        bool _has_break;
    };

    struct AsyncPipelineScope
        : PipelineScope
    {
        AsyncPipelineScope(util::sref<Scope> parent_scope, util::uid pipe_id, bool root)
            : PipelineScope(parent_scope, pipe_id, root)
        {}

        util::sptr<output::Block> deliver()
        {
            if (!this->terminated()) {
                this->block()->addStmt(util::mkptr(new output::ExprScheme(
                    this->_continueMethod()
                  , util::mkptr(new output::PipeContinue(this->pipe_id)))));
            }
            return SubScope::deliver();
        }
    private:
        output::Method _retMethod(misc::position const& p)
        {
            return this->_parent_scope->retMethod(p);
        }

        output::Method _breakMethod()
        {
            return output::method::callNext();
        }

        output::Method _continueMethod()
        {
            return output::method::callNext();
        }
    };

    struct SyncRangeScope
        : PipelineScope
    {
        SyncRangeScope(util::sref<Scope> parent_scope, util::uid pipe_id)
            : PipelineScope(parent_scope, pipe_id, true)
        {}

        bool inPipe() const { return false; }
    };

    struct AsyncRangeScope
        : AsyncPipelineScope
    {
        AsyncRangeScope(util::sref<Scope> parent_scope, util::uid pipe_id)
            : AsyncPipelineScope(parent_scope, pipe_id, true)
        {}

        bool inPipe() const { return false; }
    };

}

util::sptr<Scope> Scope::makeSyncPipelineScope(util::uid pipe_id, bool root)
{
    return util::mkptr(new PipelineScope(util::mkref(*this), pipe_id, root));
}

util::sptr<Scope> Scope::makeAsyncPipelineScope(util::uid pipe_id, bool root)
{
    return util::mkptr(new AsyncPipelineScope(util::mkref(*this), pipe_id, root));
}

util::sptr<Scope> Scope::makeSyncRangeScope(util::uid pipe_id)
{
    return util::mkptr(new SyncRangeScope(util::mkref(*this), pipe_id));
}

util::sptr<Scope> Scope::makeAsyncRangeScope(util::uid pipe_id)
{
    return util::mkptr(new AsyncRangeScope(util::mkref(*this), pipe_id));
}

util::sptr<Scope> Scope::global()
{
    return util::mkptr(new GlobalScope);
}
