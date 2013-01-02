#include <algorithm>

#include <env.h>
#include <output/node-base.h>

#include "global-filter.h"
#include "stmt-nodes.h"

using namespace semantic;

util::sptr<output::Statement const> GlobalFilter::compile(CompilingSpace space) const
{
    misc::position pos(0);
    std::for_each(stekin::preImported().begin()
                , stekin::preImported().end()
                , [&](std::string const& name)
                  {
                      space.sym()->imported(pos, name);
                  });
    return Filter::compile(std::move(space));
}
