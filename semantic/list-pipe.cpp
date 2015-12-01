#include <output/list-pipe.h>
#include <output/stmt-nodes.h>
#include <output/function.h>
#include <report/errors.h>

#include "scope.h"
#include "function.h"
#include "list-pipe.h"
#include "stmt-nodes.h"
#include "expr-nodes.h"

using namespace semantic;

util::sptr<output::Expression const> Pipeline::_compile(util::sref<Scope> scope, bool root) const
{
    return section.isAsync() ? this->_compileAsync(scope, root) : this->_compileSync(scope, root);
}

bool Pipeline::isAsync() const
{
    return list->isAsync() || section.isAsync();
}

util::sptr<output::Expression const> Pipeline::_compileAsync(
                        util::sref<Scope> scope, bool root) const
{
    util::sptr<output::Expression const> compl_list(list->compile(scope));

    util::sref<output::Block> current_flow(scope->block());
    util::sptr<output::Block> succession_flow(new output::Block);
    scope->setAsyncSpace(pos, std::vector<std::string>(), *succession_flow);

    util::sptr<Scope> pipe_scope(scope->makeAsyncPipelineScope(util::id(this), root));
    section.compile(*pipe_scope);
    current_flow->addStmt(util::mkptr(new output::AsyncCallResultDef(util::mkptr(
                                new output::AsyncPipeline(pos
                                                        , std::move(compl_list)
                                                        , pipe_scope->deliver()
                                                        , std::move(succession_flow)
                                                        , util::id(this)
                                                        , scope->throwMethod())), false)));
    if (root) {
        return util::sptr<output::Expression const>(nullptr);
    }
    return util::mkptr(new output::PipeResult(pos));
}

util::sptr<output::Expression const> Pipeline::_compileSync(
                        util::sref<Scope> scope, bool root) const
{
    util::sptr<output::Expression const> clist(list->compile(scope));
    util::sptr<Scope> pipe_scope(scope->makeSyncPipelineScope(util::id(this), root));
    section.compile(*pipe_scope);
    if (root) {
        output::Method ext_return(output::method::place());
        if (pipe_scope->firstReturn().not_nul()) {
            ext_return = scope->retMethod(misc::position(pipe_scope->firstReturn()->line));
        }
        return util::mkptr(new output::RootSyncPipeline(
                  pos, std::move(clist), pipe_scope->deliver(), std::move(ext_return)
                , util::id(this)));
    }
    return util::mkptr(new output::SyncPipeline(pos, std::move(clist), pipe_scope->deliver()));
}

static Block pushElementToResult(util::sptr<Expression const> sec)
{
    Block push;
    misc::position sec_pos(sec->pos);
    util::sptr<Expression const> callee(
            new MemberAccess(sec->pos, util::mkptr(new PipeResult(sec->pos)), "push"));
    util::ptrarr<Expression const> args;
    args.append(std::move(sec));
    push.addStmt(util::mkptr(new Arithmetics(sec_pos, util::mkptr(new Call(
                            sec_pos, std::move(callee), std::move(args))))));
    return std::move(push);
}

util::sptr<Expression const> Pipeline::createMapper(
      misc::position const& pos, util::sptr<Expression const> ls, util::sptr<Expression const> sec)
{
    return util::mkptr(new Pipeline(pos, std::move(ls), pushElementToResult(std::move(sec))));
}

util::sptr<Expression const> Pipeline::createFilter(
      misc::position const& pos, util::sptr<Expression const> ls, util::sptr<Expression const> sec)
{
    Block block;
    misc::position sec_pos(sec->pos);
    block.addStmt(util::mkptr(new Branch(
                    sec_pos
                  , std::move(sec)
                  , pushElementToResult(util::mkptr(new PipeElement(sec_pos)))
                  , Block())));
    return util::mkptr(new Pipeline(pos, std::move(ls), std::move(block)));
}

util::sptr<output::Expression const> PipeElement::compile(util::sref<Scope> scope) const
{
    if (!scope->inPipe()) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new output::PipeElement(pos));
}

util::sptr<output::Expression const> PipeIndex::compile(util::sref<Scope> scope) const
{
    if (!scope->inPipe()) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new output::PipeIndex(pos));
}

util::sptr<output::Expression const> PipeKey::compile(util::sref<Scope> scope) const
{
    if (!scope->inPipe()) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new output::PipeKey(pos));
}

util::sptr<output::Expression const> PipeResult::compile(util::sref<Scope> scope) const
{
    if (!scope->inPipe()) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new output::PipeResult(pos));
}
