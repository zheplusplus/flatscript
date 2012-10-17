#include <algorithm>

#include <proto/list-pipe.h>

#include "list-pipe.h"

using namespace flchk;

util::sptr<proto::PipeBase const> PipeMap::compile(util::sref<SymbolTable> st) const
{
    return util::mkptr(new proto::PipeMap(expr->compile(st)));
}

util::sptr<PipeBase const> PipeMap::fold() const
{
    return util::mkptr(new PipeMap(expr->fold()));
}

util::sptr<proto::PipeBase const> PipeFilter::compile(util::sref<SymbolTable> st) const
{
    return util::mkptr(new proto::PipeFilter(expr->compile(st)));
}

util::sptr<PipeBase const> PipeFilter::fold() const
{
    return util::mkptr(new PipeFilter(expr->fold()));
}

util::sptr<proto::Expression const> ListPipeline::compile(util::sref<SymbolTable> st) const
{
    std::vector<util::sptr<proto::PipeBase const>> compiled_pipe;
    std::for_each(pipeline.begin()
                , pipeline.end()
                , [&](util::sptr<PipeBase const> const& pipe)
                  {
                      compiled_pipe.push_back(pipe->compile(st));
                  });
    return util::mkptr(new proto::ListPipeline(pos, list->compile(st), std::move(compiled_pipe)));
}

std::string ListPipeline::typeName() const
{
    return list->typeName() + " | pipeline";
}

util::sptr<Expression const> ListPipeline::fold() const
{
    std::vector<util::sptr<PipeBase const>> folded_pipe;
    std::for_each(pipeline.begin()
                , pipeline.end()
                , [&](util::sptr<PipeBase const> const& pipe)
                  {
                      folded_pipe.push_back(pipe->fold());
                  });
    return util::mkptr(new ListPipeline(pos, list->fold(), std::move(folded_pipe)));
}
