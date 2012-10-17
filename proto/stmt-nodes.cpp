#include <algorithm>

#include <output/stmt-writer.h>
#include <output/expr-writer.h>
#include <util/vector-append.h>

#include "stmt-nodes.h"

using namespace proto;

void Branch::write() const
{
    output::branchIf();
    output::beginExpr();
    predicate->write();
    output::endExpr();
    consequence->write();
    output::branchElse();
    alternative->write();
}

void Arithmetics::write() const
{
    expr->write();
    output::endOfStatement();
}

void VarDef::write() const
{
    output::kwDeclare(name);
    output::beginExpr();
    init->write();
    output::endExpr();
    output::endOfStatement();
}

void Return::write() const
{
    output::kwReturn();
    ret_val->write();
    output::endOfStatement();
}

void ReturnNothing::write() const
{
    output::kwReturn();
    output::endOfStatement();
}
