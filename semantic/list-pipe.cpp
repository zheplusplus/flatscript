#include <output/list-pipe.h>
#include <output/stmt-nodes.h>

#include "compiling-space.h"
#include "list-pipe.h"

using namespace semantic;

util::sptr<output::Expression const> Pipeline::compile(CompilingSpace& space) const
{
    return section->isAsync() ? _compileAsync(space) : _compileSync(space);
}

util::sptr<output::Expression const> Pipeline::_compileAsync(CompilingSpace& space) const
{
    util::sref<output::Block> current_flow(space.block());
    util::sptr<output::Expression const> compl_list(list->compile(space));

    util::sptr<output::Block> recursion_flow(new output::Block);
    space.setAsyncSpace(*recursion_flow);
    util::sptr<output::Expression const> compl_sec(section->compile(space));
    space.block()->addStmt(util::mkptr(new output::AsyncPipeBody(std::move(compl_sec), pipe_type)));

    util::sptr<output::Block> succession_flow(new output::Block);
    space.setAsyncSpace(*succession_flow);

    current_flow->addStmt(util::mkptr(new output::AsyncCallResultDef(util::mkptr(
                                            new output::AsyncPipe(pos
                                                                , std::move(compl_list)
                                                                , std::move(recursion_flow)
                                                                , std::move(succession_flow))))));
    return util::mkptr(new output::AsyncPipeResult(pos));
}

util::sptr<output::Expression const> Pipeline::_compileSync(CompilingSpace& space) const
{
    return util::mkptr(new output::Pipeline(
                pos, list->compile(space), section->compile(space), pipe_type));
}
