#include "function.h"

using namespace grammar;

util::sptr<semantic::Function const> Function::compile() const
{
    if (-1 == async_param_index) {
        return util::mkptr(new semantic::Function(pos, name, param_names, body->compile()));
    }
    return util::mkptr(new semantic::RegularAsyncFunction(
                    pos, name, param_names, async_param_index, body->compile()));
}
