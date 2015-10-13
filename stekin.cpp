#include <algorithm>
#include <iostream>
#include <sstream>

#include <grammar/yy-misc.h>
#include <grammar/node-base.h>
#include <grammar/function.h>
#include <grammar/class.h>
#include <semantic/function.h>
#include <semantic/compiling-space.h>
#include <output/function.h>
#include <output/class.h>
#include <output/global.h>
#include <report/errors.h>

#include "env.h"
#include "globals.h"

static semantic::CompilingSpace globalSpace()
{
    semantic::CompilingSpace space;
    space.sym()->importNames(misc::position(0), std::vector<std::string>(
            stekin::Globals::g.pre_imported.begin(), stekin::Globals::g.pre_imported.end()));
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
    semantic::CompilingSpace global_space(globalSpace());
    global_flow.compile(global_space);
    util::sptr<output::Statement const> global_scope(global_space.deliver());
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
