#include <stdexcept>
#include <iostream>
#include <sstream>

#include <semantic/scope.h>
#include <semantic/node-base.h>
#include <output/global.h>
#include <report/errors.h>

#include "env.h"
#include "globals.h"
#include "including.h"

static util::sptr<output::Statement const> compileGlobal(util::sptr<semantic::Statement const> flow)
{
    util::sptr<semantic::Scope> global_scope(semantic::Scope::global());
    flow->compile(*global_scope);
    return global_scope->deliver();
}

static void compile()
{
    util::sptr<output::Statement const> global_scope(
            compileGlobal(flats::compileFile(flats::Globals::g.input_file, misc::position())));
    if (error::hasError()) {
        throw flats::CompileError();
    }
    std::stringstream os;
    output::wrapGlobal(os, *global_scope);
    std::cout << os.str();
}

int main(int argc, char* argv[])
{
    if (!flats::initEnv(argc - 1, argv + 1)) {
        return 1;
    }
    try {
        compile();
        return 0;
    } catch (flats::CompileError&) {
        return 1;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
