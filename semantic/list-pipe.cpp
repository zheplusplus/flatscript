#include <output/list-pipe.h>
#include <output/stmt-nodes.h>
#include <output/function.h>
#include <report/errors.h>

#include "compiling-space.h"
#include "function.h"
#include "list-pipe.h"
#include "stmt-nodes.h"
#include "expr-nodes.h"

using namespace semantic;

util::sptr<output::Expression const> Pipeline::compile(BaseCompilingSpace& space) const
{
    return section.isAsync() ? _compileAsync(space) : _compileSync(space);
}

bool Pipeline::isAsync() const
{
    return list->isAsync() || section.isAsync();
}

util::sptr<output::Expression const> Pipeline::_compileAsync(BaseCompilingSpace& space) const
{
    util::sref<output::Block> current_flow(space.block());
    util::sptr<output::Expression const> compl_list(list->compile(space));

    util::sptr<output::Block> succession_flow(new output::Block);
    space.setAsyncSpace(*succession_flow);

    current_flow->addStmt(util::mkptr(new output::AsyncCallResultDef(util::mkptr(
                                new output::AsyncPipeline(pos
                                                        , std::move(compl_list)
                                                        , section.compile(AsyncPipelineSpace(space))
                                                        , std::move(succession_flow)
                                                        , space.raiseMethod())))));
    return util::mkptr(new output::PipeResult(pos));
}

util::sptr<output::Expression const> Pipeline::_compileSync(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> clist(list->compile(space));
    return util::mkptr(new output::SyncPipeline(
                        pos, std::move(clist), section.compile(PipelineSpace(space))));
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
    Block filter;
    misc::position sec_pos(sec->pos);
    filter.addStmt(util::mkptr(new Branch(
                    sec_pos
                  , std::move(sec)
                  , pushElementToResult(util::mkptr(new PipeElement(sec_pos)))
                  , Block())));
    return util::mkptr(new Pipeline(pos, std::move(ls), std::move(filter)));
}

util::sptr<output::Expression const> PipeElement::compile(BaseCompilingSpace& space) const
{
    if (!space.inPipe()) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new output::PipeElement(pos));
}

util::sptr<output::Expression const> PipeIndex::compile(BaseCompilingSpace& space) const
{
    if (!space.inPipe()) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new output::PipeIndex(pos));
}

util::sptr<output::Expression const> PipeKey::compile(BaseCompilingSpace& space) const
{
    if (!space.inPipe()) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new output::PipeKey(pos));
}

util::sptr<output::Expression const> PipeResult::compile(BaseCompilingSpace& space) const
{
    if (!space.inPipe()) {
        error::pipeReferenceNotInListContext(pos);
    }
    return util::mkptr(new output::PipeResult(pos));
}
