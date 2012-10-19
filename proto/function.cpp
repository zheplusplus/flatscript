#include <algorithm>
#include <iostream>

#include <util/string.h>

#include "function.h"
#include "node-base.h"
#include "name-mangler.h"

using namespace proto;

static std::string const FUNCTION(
    "function $FUNC_NAME($PARAMETERS) {"
    "$BODY"
    "}"
);

void Function::write() const
{
    std::cout << "function " << formName(name) << "(" << util::join(",", formNames(param_names))
              << ") {" << std::endl;
    body->write();
    std::cout << "}" << std::endl;
}
