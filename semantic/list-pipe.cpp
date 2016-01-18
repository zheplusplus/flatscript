#include <output/list-pipe.h>
#include <output/stmt-nodes.h>
#include <report/errors.h>

#include "scope.h"
#include "block.h"
#include "list-pipe.h"
#include "stmt-nodes.h"
#include "expr-nodes.h"

using namespace semantic;

util::sptr<output::Expression const> Pipeline::_compile(util::sref<Scope> scope, bool root) const
{
    return this->section->isAsync() ? this->_compileAsync(scope, root)
                                    : this->_compileSync(scope, root);
}

bool Pipeline::isAsync() const
{
    return this->list->isAsync() || this->section->isAsync();
}

util::sptr<output::Expression const> Pipeline::_compileAsync(
                        util::sref<Scope> scope, bool root) const
{
    util::sptr<output::Expression const> compl_list(list->compile(scope));

    util::sref<output::Block> current_flow(scope->block());
    util::sptr<output::Block> succession_flow(new output::Block);
    scope->setAsyncSpace(pos, std::vector<std::string>(), *succession_flow);

    util::sptr<Scope> pipe_scope(scope->makeAsyncPipelineScope(this->id, root));
    this->section->compile(*pipe_scope);
    current_flow->addStmt(util::mkptr(new output::AsyncCallResultDef(
            util::mkptr(new output::AsyncPipeline(std::move(compl_list)
                                                , pipe_scope->deliver()
                                                , std::move(succession_flow)
                                                , this->id
                                                , scope->throwMethod())),
            false, this->id)));
    if (root) {
        return util::sptr<output::Expression const>(nullptr);
    }
    return util::mkptr(new output::PipeResult(this->id));
}

util::sptr<output::Expression const> Pipeline::_compileSync(
                        util::sref<Scope> scope, bool root) const
{
    util::sptr<output::Expression const> clist(list->compile(scope));
    util::sptr<Scope> pipe_scope(scope->makeSyncPipelineScope(this->id, root));
    this->section->compile(*pipe_scope);
    if (root) {
        output::Method ext_return(output::method::place());
        bool has_ret = false;
        if (pipe_scope->firstReturn().not_nul()) {
            ext_return = scope->retMethod(misc::position(pipe_scope->firstReturn()->line));
            has_ret = true;
        }
        return util::mkptr(new output::RootSyncPipeline(
                  std::move(clist), pipe_scope->deliver(), std::move(ext_return)
                , this->id, has_ret, pipe_scope->hasBreak()));
    }
    return util::mkptr(new output::SyncPipeline(
                std::move(clist), pipe_scope->deliver(), this->id));
}

static util::sptr<Statement const> pushElementToResult(util::sptr<Expression const> sec)
{
    misc::position sec_pos(sec->pos);
    util::sptr<Block> push(new Block(sec_pos));
    util::sptr<Expression const> callee(
            new MemberAccess(sec_pos, util::mkptr(new PipeResult(sec_pos)), "push"));
    util::ptrarr<Expression const> args;
    args.append(std::move(sec));
    push->addStmt(util::mkptr(new Arithmetics(sec_pos, util::mkptr(new Call(
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
    misc::position sec_pos(sec->pos);
    util::sptr<Block> block(new Block(sec_pos));
    block->addStmt(util::mkptr(new Branch(
                    sec_pos
                  , std::move(sec)
                  , pushElementToResult(util::mkptr(new PipeElement(sec_pos)))
                  , util::mkptr(new Block(sec_pos)))));
    return util::mkptr(new Pipeline(pos, std::move(ls), std::move(block)));
}

util::sptr<output::Expression const> PipeElement::compile(util::sref<Scope> scope) const
{
    if (!scope->inPipe()) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new output::PipeElement(scope->scopeId()));
}

util::sptr<output::Expression const> PipeIndex::compile(util::sref<Scope> scope) const
{
    if (!scope->inPipe()) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new output::PipeIndex(scope->scopeId()));
}

util::sptr<output::Expression const> PipeKey::compile(util::sref<Scope> scope) const
{
    if (!scope->inPipe()) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new output::PipeKey(scope->scopeId()));
}

util::sptr<output::Expression const> PipeResult::compile(util::sref<Scope> scope) const
{
    if (!scope->inPipe()) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new output::PipeResult(scope->scopeId()));
}
