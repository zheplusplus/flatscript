#include <iostream>

#include <grammar/yy-misc.h>
#include <flowcheck/filter.h>
#include <proto/node-base.h>
#include <report/errors.h>

namespace {

    struct CompileFailure {};

}

static util::sptr<flchk::Filter> frontEnd()
{
    yyparse();
    if (error::hasError()) {
        throw CompileFailure();
    }

    util::sptr<flchk::Filter> global_flow(grammar::builder.buildAndClear());
    if (error::hasError()) {
        throw CompileFailure();
    }
    return std::move(global_flow);
}

static void semantic(util::sptr<flchk::Filter> global_flow)
{
    util::sptr<proto::Statement const> proto_global_block(global_flow->compile());
    if (error::hasError()) {
        throw CompileFailure();
    }
    std::cout << "(function() {" << std::endl;
    proto_global_block->write();
    std::cout << "})();" << std::endl;
}

int main()
{
    try {
        semantic(frontEnd());
        return 0;
    } catch (CompileFailure) {
        return 1;
    }
}
