#include <output/function-impl.h>
#include <output/stmt-nodes.h>
#include <output/expr-nodes.h>
#include <output/list-pipe.h>
#include <report/errors.h>

#include "stmt-nodes.h"
#include "scope-impl.h"
#include "common.h"

using namespace semantic;

void Arithmetics::compile(util::sref<Scope> scope) const
{
    util::sptr<output::Expression const> cexpr(expr->compileAsRoot(scope));
    if (cexpr.nul()) {
        return scope->checkNotTerminated(this->pos);
    }
    scope->addStmt(pos, makeArith(std::move(cexpr)));
}

bool Arithmetics::isAsync() const
{
    return expr->isAsync();
}

void Branch::compile(util::sref<Scope> scope) const
{
    if (predicate->isLiteral(scope->sym())) {
        auto block(predicate->boolValue(scope->sym()) ? *this->consequence : *this->alternative);
        BranchingSubScope sub_scope(scope);
        block->compile(sub_scope);
        return scope->addStmt(pos, sub_scope.deliver());
    }
    util::sptr<output::Expression const> compiled_pred(predicate->compile(scope));
    BranchingSubScope consq_scope(scope);
    this->consequence->compile(consq_scope);
    BranchingSubScope alter_scope(scope);
    this->alternative->compile(alter_scope);

    scope->addStmt(pos, util::mkptr(new output::Branch(
                        std::move(compiled_pred), consq_scope.deliver(), alter_scope.deliver())));
    if (consq_scope.terminated() && alter_scope.terminated()) {
        return scope->terminate(pos);
    }

    if (this->consequence->isAsync() || this->alternative->isAsync()) {
        util::sptr<output::NoParamCallback> succession(new output::NoParamCallback);
        if (!consq_scope.terminated()) {
            consq_scope.addStmt(pos, makeArith(succession->callMe()));
        }
        if (!alter_scope.terminated()) {
            alter_scope.addStmt(pos, makeArith(succession->callMe()));
        }
        util::sref<output::Block> sf(succession->bodyFlow());
        scope->block()->addFunc(std::move(succession));
        scope->setAsyncSpace(pos, std::vector<std::string>(), sf);
    }
}

bool Branch::isAsync() const
{
    return this->predicate->isAsync() || this->consequence->isAsync()
                                      || this->alternative->isAsync();
}

void NameDef::compile(util::sref<Scope> scope) const
{
    util::sptr<output::Expression const> init_value(init->compile(scope));
    if (init->isLiteral(scope->sym())) {
        return scope->sym()->defConst(pos, name, *init);
    }
    scope->sym()->defName(pos, name);
    scope->addStmt(pos, makeArith(util::mkptr(
        new output::Assignment(scope->sym()->compileRef(pos, name), std::move(init_value)))));
}

bool NameDef::isAsync() const
{
    return init->isAsync();
}

void Return::compile(util::sref<Scope> scope) const
{
    util::sptr<output::Expression const> r(ret_val->compile(scope));
    scope->addStmt(pos, util::mkptr(new output::ExprScheme(scope->retMethod(pos), std::move(r))));
    scope->terminate(pos);
}

bool Return::isAsync() const
{
    return ret_val->isAsync();
}

void Extern::compile(util::sref<Scope> scope) const
{
    scope->sym()->addExternNames(pos, names);
}

void Export::compile(util::sref<Scope> scope) const
{
    util::sptr<output::Expression const> cval(value->compile(scope));
    scope->addStmt(pos, util::mkptr(new output::Export(export_point, std::move(cval))));
}

bool Export::isAsync() const
{
    return value->isAsync();
}

void AttrSet::compile(util::sref<Scope> scope) const
{
    util::sptr<output::Expression const> csp(set_point->compile(scope));
    util::sptr<output::Expression const> cval(value->compile(scope));
    scope->addStmt(pos, makeArith(util::mkptr(
            new output::Assignment(std::move(csp), std::move(cval)))));
}

bool AttrSet::isAsync() const
{
    return set_point->isAsync() || value->isAsync();
}

void ExceptionStall::compile(util::sref<Scope> scope) const
{
    if (!isAsync()) {
        BranchingSubScope try_scope(scope);
        this->try_block->compile(try_scope);
        CatchScope catch_scope(scope, this->pos, this->except_name);
        this->catch_block->compile(catch_scope);
        scope->addStmt(pos, util::mkptr(new output::ExceptionStall(
                try_scope.deliver(), this->except_name,
                catch_scope.scopeId(), catch_scope.deliver())));
        if (try_scope.terminated() && catch_scope.terminated()) {
            scope->terminate(this->pos);
        }
        return;
    }

    CatchScope catch_scope(scope, this->pos, this->except_name);
    util::sptr<output::AsyncCatchFunc> catch_func(new output::AsyncCatchFunc(
                this->except_name, catch_scope.scopeId()));
    util::sref<output::AsyncCatchFunc> catch_func_ref = *catch_func;
    scope->block()->addFunc(std::move(catch_func));

    catch_scope.setAsyncSpace(pos, std::vector<std::string>(), catch_func_ref->bodyFlow());
    this->catch_block->compile(catch_scope);

    AsyncTryScope try_scope(scope, catch_func_ref);
    this->try_block->compile(try_scope);

    util::sptr<output::Block> staller(new output::Block);
    staller->addStmt(makeArith(catch_func_ref->callMe(util::mkptr(
            new output::SubReference(this->except_name, catch_scope.scopeId())))));
    scope->addStmt(pos, util::mkptr(new output::ExceptionStall(
            try_scope.deliver(), this->except_name, catch_scope.scopeId(), std::move(staller))));
    if (try_scope.terminated() && catch_scope.terminated()) {
        return scope->terminate(pos);
    }

    util::sptr<output::NoParamCallback> succession(new output::NoParamCallback);
    if (!try_scope.terminated()) {
        try_scope.addStmt(pos, makeArith(succession->callMe()));
    }
    if (!catch_scope.terminated()) {
        catch_scope.addStmt(pos, makeArith(succession->callMe()));
    }

    util::sptr<output::Block> succ_try(new output::Block);
    util::sref<output::Block> succ_try_ref = *succ_try;

    util::sptr<output::Block> succ_catch(new output::Block);
    succ_catch->addStmt(util::mkptr(new output::ExprScheme(
        scope->throwMethod(), util::mkptr(new output::ExceptionObj))));

    succession->bodyFlow()->addStmt(util::mkptr(
        new output::ExceptionStallDeprecated(std::move(succ_try), std::move(succ_catch))));
    scope->block()->addFunc(std::move(succession));
    scope->setAsyncSpace(pos, std::vector<std::string>(), succ_try_ref);
}

bool ExceptionStall::isAsync() const
{
    return this->try_block->isAsync() || this->catch_block->isAsync();
}

static util::sptr<output::Statement const> _catch_stmt(
            util::sref<output::AsyncCatcherDeprecated> catch_func)
{
    return makeArith(catch_func->callMe(util::mkptr(new output::ExceptionObj)));
}

void ExceptionStallDeprecated::compile(util::sref<Scope> scope) const
{
    if (!isAsync()) {
        BranchingSubScope try_scope(scope);
        this->try_block->compile(try_scope);
        CatchScopeDeprecated catch_scope(scope);
        this->catch_block->compile(catch_scope);
        scope->addStmt(pos, util::mkptr(new output::ExceptionStallDeprecated(
                        try_scope.deliver(), catch_scope.deliver())));
        if (try_scope.terminated() && catch_scope.terminated()) {
            scope->terminate(pos);
        }
        return;
    }

    util::sptr<output::AsyncCatcherDeprecated> catch_func(new output::AsyncCatcherDeprecated);
    util::sref<output::AsyncCatcherDeprecated> catch_func_ref = *catch_func;
    scope->block()->addFunc(std::move(catch_func));

    CatchScopeDeprecated catch_scope(scope);
    catch_scope.setAsyncSpace(pos, std::vector<std::string>(), catch_func_ref->bodyFlow());
    this->catch_block->compile(catch_scope);

    AsyncTryScope try_scope(scope, catch_func_ref);
    this->try_block->compile(try_scope);

    util::sptr<output::Block> staller(new output::Block);
    staller->addStmt(_catch_stmt(catch_func_ref));
    scope->addStmt(pos, util::mkptr(new output::ExceptionStallDeprecated(
                                    try_scope.deliver(), std::move(staller))));
    if (try_scope.terminated() && catch_scope.terminated()) {
        return scope->terminate(pos);
    }

    util::sptr<output::NoParamCallback> succession(new output::NoParamCallback);
    if (!try_scope.terminated()) {
        try_scope.addStmt(pos, makeArith(succession->callMe()));
    }
    if (!catch_scope.terminated()) {
        catch_scope.addStmt(pos, makeArith(succession->callMe()));
    }

    util::sptr<output::Block> succ_try(new output::Block);
    util::sref<output::Block> succ_try_ref = *succ_try;

    util::sptr<output::Block> succ_catch(new output::Block);
    succ_catch->addStmt(util::mkptr(new output::ExprScheme(
        scope->throwMethod(), util::mkptr(new output::ExceptionObj))));

    succession->bodyFlow()->addStmt(util::mkptr(
        new output::ExceptionStallDeprecated(std::move(succ_try), std::move(succ_catch))));
    scope->block()->addFunc(std::move(succession));
    scope->setAsyncSpace(pos, std::vector<std::string>(), succ_try_ref);
}

bool ExceptionStallDeprecated::isAsync() const
{
    return this->try_block->isAsync() || this->catch_block->isAsync();
}

void Throw::compile(util::sref<Scope> scope) const
{
    if (exception->isAsync()) {
        return error::asyncNotAllowedInThrow(pos);
    }
    scope->addStmt(pos, util::mkptr(new output::ExprScheme(
                scope->throwMethod(), exception->compile(scope))));
    scope->terminate(pos);
}

void Break::compile(util::sref<Scope> scope) const
{
    scope->addStmt(pos, util::mkptr(new output::ExprScheme(
            scope->breakMethod(this->pos)
          , util::mkptr(new output::PipeBreak(scope->scopeId())))));
    scope->terminate(this->pos);
}

void Continue::compile(util::sref<Scope> scope) const
{
    scope->addStmt(pos, util::mkptr(new output::ExprScheme(
            scope->continueMethod(this->pos)
          , util::mkptr(new output::PipeContinue(scope->scopeId())))));
    scope->terminate(this->pos);
}

void IncludeFile::compile(util::sref<Scope> scope) const
{
    util::uid include_id(scope->includeFile(this->pos, this->file));
    scope->sym()->defModule(this->pos, this->module_alias, include_id);
}
