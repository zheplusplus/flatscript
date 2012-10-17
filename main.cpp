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
#include <proto/symbol-table.h>
#include <proto/func-inst-draft.h>
#include <proto/variable.h>
#include <proto/type.h>
#include <proto/func-reference-type.h>
#include <instance/node-base.h>
#include <output/func-writer.h>
#include <util/pointer.h>
#include <report/errors.h>
#include <inspect/trace.h>

namespace {

    struct CompileFailure {};

    struct Functions {
        util::serial_num const main_sn;
        std::vector<util::sptr<inst::Function const>> funcs;

        Functions(util::serial_num s, std::vector<util::sptr<inst::Function const>> f)
            : main_sn(s)
            , funcs(std::move(f))
        {}

        Functions(Functions&& rhs)
            : main_sn(rhs.main_sn)
            , funcs(std::move(rhs.funcs))
        {}
    };

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

static Functions semantic(util::sptr<flchk::Filter> global_flow)
{
    proto::Block proto_global_block;
    global_flow->compile(util::mkref(proto_global_block));
    if (error::hasError()) {
        throw CompileFailure();
    }

    proto::SymbolTable st;
    util::sptr<proto::FuncInstDraft> inst_global_func(proto::FuncInstDraft::createGlobal());
    misc::trace trace;
    inst_global_func->instantiate(util::mkref(proto_global_block), trace);
    if (error::hasError()) {
        throw CompileFailure();
    }
    std::vector<util::sptr<inst::Function const>> funcs(proto_global_block.deliverFuncs());
    funcs.push_back(inst_global_func->deliver());
    return Functions(inst_global_func->sn, std::move(funcs));
}

static void outputAll(Functions funcs)
{
    std::for_each(funcs.funcs.begin()
                , funcs.funcs.end()
                , [&](util::sptr<inst::Function const> const& func)
                  {
                      func->writeDecl();
                  });
    output::writeMainBegin();
    output::stknMainFunc(funcs.main_sn);
    output::writeMainEnd();
    std::for_each(funcs.funcs.begin()
                , funcs.funcs.end()
                , [&](util::sptr<inst::Function const> const& func)
                  {
                      func->writeImpl();
                  });
}

int main()
{
    inspect::prepare_for_trace();
    try {
        outputAll(semantic(frontEnd()));
        return 0;
    } catch (CompileFailure) {
        return 1;
    }
}
