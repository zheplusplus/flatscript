#include <iostream>
#include <deque>

#include "yy-misc.h"

int yywrap(void)
{
    return 1;
}

grammar::ClauseBuilder parser::builder;
int parser::last_indent = 0;

misc::position parser::here()
{
    return misc::position(yylineno);
}

misc::position parser::here(int lineno)
{
    return misc::position(lineno);
}
