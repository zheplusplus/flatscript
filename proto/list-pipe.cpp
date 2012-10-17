#include <algorithm>

#include <output/func-writer.h>
#include <report/errors.h>

#include "list-pipe.h"

using namespace proto;

void PipeMap::writeBegin() const
{
    output::pipeMapBegin();
}

void PipeMap::writeEnd() const
{
    output::pipeMapEnd();
}

void PipeFilter::writeBegin() const
{
    output::pipeFilterBegin();
}

void PipeFilter::writeEnd() const
{
    output::pipeFilterEnd();
}

void ListPipeline::write() const
{
    std::for_each(pipeline.rbegin()
                , pipeline.rend()
                , [&](util::sptr<PipeBase const> const& p)
                  {
                      p->writeBegin();
                      p->expr->writeAsPipe();
                      p->writeEnd();
                  });
    list->write();
    std::for_each(pipeline.begin()
                , pipeline.end()
                , [&](util::sptr<PipeBase const> const&)
                  {
                      output::pipeFinished();
                  });
}

void ListPipeline::writeAsPipe() const
{
    std::for_each(pipeline.rbegin()
                , pipeline.rend()
                , [&](util::sptr<PipeBase const> const& p)
                  {
                      p->writeBegin();
                      p->expr->writeAsPipe();
                      p->writeEnd();
                  });
    list->writeAsPipe();
    std::for_each(pipeline.begin()
                , pipeline.end()
                , [&](util::sptr<PipeBase const> const&)
                  {
                      output::pipeFinished();
                  });
}
