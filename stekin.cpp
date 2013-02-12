#include <algorithm>
#include <iostream>
#include <sstream>

#include <grammar/yy-misc.h>
#include <semantic/function.h>
#include <semantic/compiling-space.h>
#include <output/function.h>
#include <output/global.h>
#include <report/errors.h>

#include "env.h"

static semantic::CompilingSpace globalSpace()
{
    semantic::CompilingSpace space;
    misc::position pos(0);
    std::for_each(stekin::preImported().begin()
                , stekin::preImported().end()
                , [&](std::string const& name)
                  {
                      space.sym()->imported(pos, name);
                  });
    return std::move(space);
}

int main(int argc, char* argv[])
{
    stekin::initEnv(argc, argv);
    yyparse();
    if (error::hasError()) {
        return 1;
    }
    semantic::Block global_flow(grammar::builder.buildAndClear());
    if (error::hasError()) {
        return 1;
    }
    util::sptr<output::Statement const> global_scope(global_flow.compile(globalSpace()));
    if (error::hasError()) {
        return 1;
    }
    std::stringstream os;
    output::wrapGlobal(os, *global_scope);
    if (error::hasError()) {
        return 1;
    }
    std::cout << os.str();
    return 0;
}
