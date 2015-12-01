#include <output/expr-nodes.h>
#include <output/stmt-nodes.h>
#include <output/list-pipe.h>
#include <output/function.h>
#include <report/errors.h>
#include <globals.h>

#include "scope-impl.h"
#include "common.h"

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
    this->_current_block = this->replaceSpace(pos, block);
}

util::sref<output::Block> Scope::replaceSpace(
                misc::position const&, util::sref<output::Block> block)
{
    return block;
}

util::sptr<output::Block> Scope::deliver()
{
    this->_main_block->setLocalDecls(this->sym()->localNames());
    return std::move(this->_main_block);
}

util::sptr<output::Block> BasicFunctionScope::deliver()
{
    if (this->_this_referenced) {
        util::sptr<output::Block> block(new output::Block);
        block->addStmt(util::mkptr(new output::ThisDeclaration));
        block->append(Scope::deliver());
        return std::move(block);
    }
    return Scope::deliver();
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
                                           , util::sref<SymbolTable> ext_st
                                           , std::vector<std::string> const& params
                                           , bool allow_super)
    : BasicFunctionScope(pos, ext_st, params, allow_super)
    , ret_pos(pos)
{
    this->_current_block = this->replaceSpace(pos, this->_current_block);
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
            output::method::asyncRet(), util::mkptr(new output::Undefined(this->ret_pos)))));
    }
    return BasicFunctionScope::deliver();
}

typedef std::function<util::sptr<output::Statement const>(
            util::sptr<output::Expression const>)> MakeCatcher;

static util::sref<output::Block> _replaceSpaceInAsyncTry(
                misc::position const& pos, util::sref<output::Block> block,
                MakeCatcher make_catcher)
{
    util::sptr<output::Block> try_block(new output::Block);
    util::sref<output::Block> try_flow(*try_block);

    block->addStmt(util::mkptr(new output::ExceptionStall(
                std::move(try_block)
              , make_catcher(util::mkptr(new output::ExceptionObj(pos))))));
    return try_flow;
}

util::sref<output::Block> RegularAsyncFuncScope::replaceSpace(
                misc::position const& pos, util::sref<output::Block> block)
{
    return ::_replaceSpaceInAsyncTry(
        pos, block, [this](util::sptr<output::Expression const> exception)
                    {
                        return util::mkptr(new output::ExprScheme(
                            this->throwMethod(), std::move(exception)));
                    });
}

output::Method AsyncTryScope::throwMethod() const
{
    return output::method::asyncCatcher(catch_func->mangledName());
}

util::sref<output::Block> AsyncTryScope::replaceSpace(
                misc::position const& pos, util::sref<output::Block> block)
{
    return ::_replaceSpaceInAsyncTry(
        pos, block, [&](util::sptr<output::Expression const> exception)
                    {
                        util::ptrarr<output::Expression const> args;
                        args.append(std::move(exception));
                        return makeArith(this->catch_func->callMe(pos, std::move(args)));
                    });
}

namespace {

    struct GlobalScope
        : Scope
    {
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

    struct PipelineScope
        : SubScope
    {
        PipelineScope(util::sref<Scope> parent_scope, util::id pid, bool root)
            : SubScope(parent_scope)
            , pipe_id(pid)
            , _sym(parent_scope->sym())
            , _first_return_or_nul_if_not_returned(nullptr)
            , _root(root)
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

        util::sref<misc::position const> firstReturn() const
        {
            return *this->_first_return_or_nul_if_not_returned;
        }

        util::sref<SymbolTable> sym()
        {
            return util::mkref(this->_sym);
        }

        util::id const pipe_id;
    private:
        virtual output::Method _retMethod(misc::position const&)
        {
            return output::method::syncPipeRet(this->pipe_id);
        }

        RegularSymbolTable _sym;
        util::sptr<misc::position const> _first_return_or_nul_if_not_returned;
        bool const _root;
    };

    struct AsyncPipelineScope
        : PipelineScope
    {
        AsyncPipelineScope(util::sref<Scope> parent_scope, util::id pipe_id, bool root)
            : PipelineScope(parent_scope, pipe_id, root)
        {}

        util::sptr<output::Block> deliver()
        {
            if (!this->terminated()) {
                this->block()->addStmt(util::mkptr(new output::PipelineContinue(this->pipe_id)));
            }
            return SubScope::deliver();
        }
    private:
        output::Method _retMethod(misc::position const& p)
        {
            return this->_parent_scope->retMethod(p);
        }
    };

}

util::sptr<Scope> Scope::makeSyncPipelineScope(util::id pipe_id, bool root)
{
    return util::mkptr(new PipelineScope(util::mkref(*this), pipe_id, root));
}

util::sptr<Scope> Scope::makeAsyncPipelineScope(util::id pipe_id, bool root)
{
    return util::mkptr(new AsyncPipelineScope(util::mkref(*this), pipe_id, root));
}

util::sptr<Scope> Scope::global()
{
    util::sptr<Scope> scope(new GlobalScope);
    scope->sym()->addExternNames(misc::position(0), std::vector<std::string>(
            flats::Globals::g.external_syms.begin(), flats::Globals::g.external_syms.end()));
    return std::move(scope);
}
