#include <stdexcept>
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

static util::sptr<output::Statement const> compileGlobal(semantic::Block flow)
{
    semantic::CompilingSpace space;
    space.sym()->addExternNames(misc::position(0), std::vector<std::string>(
            flats::Globals::g.external_syms.begin(), flats::Globals::g.external_syms.end()));
    flow.compile(space);
    return space.deliver();
}

static int compile()
{
    grammar::parse();
    if (error::hasError()) {
        return 1;
    }
    semantic::Block global_flow(grammar::builder.buildAndClear());
    if (error::hasError()) {
        return 1;
    }
    util::sptr<output::Statement const> global_scope(compileGlobal(std::move(global_flow)));
    if (error::hasError()) {
        return 1;
    }
    std::stringstream os;
    output::wrapGlobal(os, *global_scope);
    std::cout << os.str();
    return 0;
}

int main(int argc, char* argv[])
{
    flats::initEnv(argc, argv);
    try {
        return compile();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
