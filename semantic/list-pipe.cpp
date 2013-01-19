#include <output/list-pipe.h>
#include <output/stmt-nodes.h>

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
                                                        , section.compile(PipelineSpace(space))
                                                        , std::move(succession_flow))))));
    return util::mkptr(new output::AsyncPipeResult(pos));
}

util::sptr<output::Expression const> Pipeline::_compileSync(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> clist(list->compile(space));
    return util::mkptr(new output::SyncPipeline(
                        pos, std::move(clist), section.compile(PipelineSpace(space))));
}

util::sptr<Expression const> Pipeline::createMapper(
      misc::position const& pos, util::sptr<Expression const> ls, util::sptr<Expression const> sec)
{
    Block ret;
    ret.addStmt(util::mkptr(new Return(sec->pos, std::move(sec))));
    return util::mkptr(new Pipeline(pos, std::move(ls), std::move(ret)));
}

util::sptr<Expression const> Pipeline::createFilter(
      misc::position const& pos, util::sptr<Expression const> ls, util::sptr<Expression const> sec)
{
    Block ret;
    ret.addStmt(util::mkptr(new Return(sec->pos, util::mkptr(new PipeElement(sec->pos)))));
    Block filter;
    filter.addStmt(util::mkptr(new Branch(sec->pos, std::move(sec), std::move(ret), Block())));
    return util::mkptr(new Pipeline(pos, std::move(ls), std::move(filter)));
}
