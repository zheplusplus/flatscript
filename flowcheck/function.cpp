#include <algorithm>

#include <proto/function.h>

#include "function.h"
#include "node-base.h"
#include "filter.h"

using namespace flchk;

util::sptr<proto::Function const> Function::compile() const
{
    return util::mkptr(new proto::Function(pos, name, param_names, _body->compile()));
}
