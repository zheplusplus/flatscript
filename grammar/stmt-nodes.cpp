#include <semantic/node-base.h>
#include <semantic/symbol-def-filter.h>

#include "stmt-nodes.h"
#include "function.h"

using namespace grammar;

static util::sptr<semantic::Filter> mkSymDefFilter()
{
    return util::mkptr(new semantic::SymbolDefFilter);
}

void Arithmetics::compile(util::sref<semantic::Filter> filter) const
{
    filter->addArith(pos, expr->reduceAsExpr(false));
}

void Branch::compile(util::sref<semantic::Filter> filter) const
{
    filter->addBranch(pos
                    , predicate->reduceAsExpr(false)
                    , consequence.compile(mkSymDefFilter())
                    , alternative.compile(mkSymDefFilter()));
}

void BranchConsqOnly::compile(util::sref<semantic::Filter> filter) const
{
    filter->addBranch(pos, predicate->reduceAsExpr(false), consequence.compile(mkSymDefFilter()));
}

void BranchAlterOnly::compile(util::sref<semantic::Filter> filter) const
{
    filter->addBranchAlterOnly(
            pos, predicate->reduceAsExpr(false), alternative.compile(mkSymDefFilter()));
}

void Return::compile(util::sref<semantic::Filter> filter) const
{
    filter->addReturn(pos, ret_val->reduceAsExpr(false));
}

void ReturnNothing::compile(util::sref<semantic::Filter> filter) const
{
    filter->addReturnNothing(pos);
}

void NameDef::compile(util::sref<semantic::Filter> filter) const
{
    filter->defName(pos, name, init->reduceAsExpr(false));
}

void Import::compile(util::sref<semantic::Filter> filter) const
{
    filter->addImport(pos, names);
}

void Export::compile(util::sref<semantic::Filter> filter) const
{
    filter->addExport(pos, export_point, value->reduceAsExpr(false));
}

void AttrSet::compile(util::sref<semantic::Filter> filter) const
{
    filter->addAttrSet(pos, set_point->reduceAsLeftValue(false), value->reduceAsExpr(false));
}
