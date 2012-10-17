#include <algorithm>

#include <output/func-writer.h>

#include "function.h"
#include "stmt-nodes.h"

using namespace proto;

void Function::write() const
{
    output::writeFuncBegin(name, param_names);
    body->write();
    output::writeFuncEnd();
}
