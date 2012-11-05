#include <algorithm>

#include <env.h>
#include <output/node-base.h>

#include "global-filter.h"
#include "stmt-nodes.h"

using namespace semantic;

util::sptr<output::Statement const> GlobalFilter::compile(util::sref<SymbolTable> st) const
{
    misc::position pos(0);
    st->imported(pos, "console");
    st->imported(pos, "setTimeout");
    st->imported(pos, "setInterval");
    st->imported(pos, "clearTimeout");
    std::for_each(stekin::preImported().begin()
                , stekin::preImported().end()
                , [&](std::string const& name)
                  {
                      st->imported(pos, name);
                  });
    return Filter::compile(st);
}
