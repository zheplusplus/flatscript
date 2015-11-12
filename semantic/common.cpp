#include "common.h"

#include <output/methods.h>
#include <output/stmt-nodes.h>

using namespace semantic;

util::sptr<output::Statement const> semantic::makeArith(util::sptr<output::Expression const> e)
{
    return util::mkptr(new output::ExprScheme(output::method::place(), std::move(e)));
}
