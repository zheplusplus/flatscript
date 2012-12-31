#include <iostream>
#include <sstream>

#include <grammar/yy-misc.h>
#include <semantic/filter.h>
#include <semantic/symbol-table.h>
#include <output/node-base.h>
#include <report/errors.h>

#include "env.h"

int main(int argc, char* argv[])
{
    stekin::initEnv(argc, argv);
    yyparse();
    if (error::hasError()) {
        return 1;
    }
    util::sptr<semantic::Filter> global_flow(grammar::builder.buildAndClear());
    if (error::hasError()) {
        return 1;
    }
    semantic::SymbolTable st;
    util::sptr<output::Statement const> global_scope(global_flow->compile(util::mkref(st)));
    if (error::hasError()) {
        return 1;
    }
    std::stringstream os;
    os << "(function() {" << std::endl;
    global_scope->write(os);
    os << "})();" << std::endl;
    if (error::hasError()) {
        return 1;
    }
    std::cout << os.str();
    return 0;
}
