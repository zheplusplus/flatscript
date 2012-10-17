#include <flowcheck/node-base.h>
#include <flowcheck/symbol-def-filter.h>
#include <flowcheck/function.h>

#include "stmt-nodes.h"
#include "function.h"

using namespace grammar;

static util::sptr<flchk::Filter> mkSymDefFilter(util::sref<flchk::Filter> ext_filter)
{
    return util::mkptr(new flchk::SymbolDefFilter(ext_filter->getSymbols()));
}

void Arithmetics::compile(util::sref<flchk::Filter> filter) const
{
    filter->addArith(pos, expr->compile());
}

void Branch::compile(util::sref<flchk::Filter> filter) const
{
    filter->addBranch(pos
                    , predicate->compile()
                    , consequence.compile(mkSymDefFilter(filter))
                    , alternative.compile(mkSymDefFilter(filter)));
}

void BranchConsqOnly::compile(util::sref<flchk::Filter> filter) const
{
    filter->addBranch(pos, predicate->compile(), consequence.compile(mkSymDefFilter(filter)));
}

void BranchAlterOnly::compile(util::sref<flchk::Filter> filter) const
{
    filter->addBranchAlterOnly(pos
                             , predicate->compile()
                             , alternative.compile(mkSymDefFilter(filter)));
}

void Return::compile(util::sref<flchk::Filter> filter) const
{
    filter->addReturn(pos, ret_val->compile());
}

void ReturnNothing::compile(util::sref<flchk::Filter> filter) const
{
    filter->addReturnNothing(pos);
}

void VarDef::compile(util::sref<flchk::Filter> filter) const
{
    filter->defVar(pos, name, init->compile());
}
