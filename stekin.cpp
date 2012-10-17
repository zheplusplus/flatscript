#include <map>
#include <list>
#include <algorithm>
#include <vector>

#include <parser/yy-misc.h>
#include <grammar/clause-builder.h>
#include <flowcheck/filter.h>
#include <flowcheck/node-base.h>
#include <flowcheck/function.h>
#include <proto/node-base.h>
#include <proto/function.h>
#include <output/func-writer.h>
#include <util/pointer.h>
#include <report/errors.h>
#include <inspect/trace.h>

namespace {

    struct CompileFailure {};

}

static util::sptr<flchk::Filter> frontEnd()
{
    yyparse();
    if (error::hasError()) {
        throw CompileFailure();
    }

    util::sptr<flchk::Filter> global_flow(std::move(parser::builder.buildAndClear()));
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
    output::writeMainBegin();
    proto_global_block->write();
    output::writeMainEnd();
}

int main()
{
    inspect::prepare_for_trace();
    try {
        semantic(frontEnd());
        return 0;
    } catch (CompileFailure) {
        return 1;
    }
}
