#include <semantic/node-base.h>

#include "node-base.h"
#include "yy-misc.h"

using namespace grammar;

int yywrap(void)
{
    return 1;
}

ClauseBuilder grammar::builder;
int grammar::last_indent = 0;

misc::position grammar::here()
{
    return misc::position(yylineno);
}
