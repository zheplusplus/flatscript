#include <semantic/node-base.h>
#include <semantic/function.h>

#include "stmt-nodes.h"
#include "function.h"

using namespace grammar;

void Arithmetics::compile(util::sref<semantic::Filter> filter, BaseReducingEnv const& env) const
{
    filter->addArith(pos, expr->reduceAsExpr(env));
}

void Branch::compile(util::sref<semantic::Filter> filter, BaseReducingEnv const& env) const
{
    filter->addBranch(pos
                    , predicate->reduceAsExpr(env)
                    , consequence.compile(env)
                    , alternative.compile(env));
}

void BranchConsqOnly::compile(util::sref<semantic::Filter> filter, BaseReducingEnv const& env) const
{
    filter->addBranch(pos, predicate->reduceAsExpr(env), consequence.compile(env));
}

void BranchAlterOnly::compile(util::sref<semantic::Filter> filter, BaseReducingEnv const& env) const
{
    filter->addBranchAlterOnly(pos, predicate->reduceAsExpr(env), alternative.compile(env));
}

void Return::compile(util::sref<semantic::Filter> filter, BaseReducingEnv const& env) const
{
    filter->addReturn(pos, ret_val->reduceAsExpr(env));
}

void ReturnNothing::compile(util::sref<semantic::Filter> filter, BaseReducingEnv const&) const
{
    filter->addReturnNothing(pos);
}

void NameDef::compile(util::sref<semantic::Filter> filter, BaseReducingEnv const& env) const
{
    filter->defName(pos, name, init->reduceAsExpr(env));
}

void Import::compile(util::sref<semantic::Filter> filter, BaseReducingEnv const&) const
{
    filter->addImport(pos, names);
}

void Export::compile(util::sref<semantic::Filter> filter, BaseReducingEnv const& env) const
{
    filter->addExport(pos, export_point, value->reduceAsExpr(env));
}

void AttrSet::compile(util::sref<semantic::Filter> filter, BaseReducingEnv const& env) const
{
    filter->addAttrSet(pos, set_point->reduceAsLeftValue(env), value->reduceAsExpr(env));
}
