#include <algorithm>

#include <flowcheck/list-pipe.h>

#include "list-pipe.h"

using namespace grammar;

util::sptr<flchk::PipeBase const> PipeMap::compile() const
{
    return util::mkptr(new flchk::PipeMap(expr->compile()));
}

util::sptr<flchk::PipeBase const> PipeFilter::compile() const
{
    return util::mkptr(new flchk::PipeFilter(expr->compile()));
}

util::sptr<flchk::Expression const> ListPipeline::compile() const
{
    std::vector<util::sptr<flchk::PipeBase const>> compiled_pipe;
    std::for_each(pipeline.begin()
                , pipeline.end()
                , [&](util::sptr<PipeBase const> const& pipe)
                  {
                      compiled_pipe.push_back(pipe->compile());
                  });
    return util::mkptr(new flchk::ListPipeline(pos, list->compile(), std::move(compiled_pipe)));
}
