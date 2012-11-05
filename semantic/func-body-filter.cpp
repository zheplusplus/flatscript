#include <output/node-base.h>

#include "func-body-filter.h"
#include "stmt-nodes.h"

using namespace semantic;

void FuncBodyFilter::defName(misc::position const& pos
                           , std::string const& name
                           , util::sptr<Expression const> init)
{
    _checkNotTerminated(pos);
    _block.addStmt(util::mkptr(new NameDef(pos, name, std::move(init))));
}

void FuncBodyFilter::defFunc(misc::position const& pos
                           , std::string const& name
                           , std::vector<std::string> const& param_names
                           , util::sptr<Filter> body)
{
    _block.defFunc(pos, name, param_names, std::move(body));
}
