#include <semantic/node-base.h>

#include "yy-misc.h"
#include "clauses.h"

using namespace grammar;

int yywrap(void)
{
    return 1;
}

ClauseBuilder grammar::builder;
int grammar::last_indent = 0;
int grammar::lineno = 1;

misc::position grammar::here()
{
    return misc::position(grammar::lineno);
}
