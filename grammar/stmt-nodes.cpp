#include <semantic/node-base.h>
#include <semantic/function.h>
#include <report/errors.h>

#include "stmt-nodes.h"
#include "function.h"

using namespace grammar;

void Arithmetics::compile(util::sref<semantic::Filter> filter) const
{
    filter->addArith(pos, expr->reduceAsExpr());
}

void Branch::compile(util::sref<semantic::Filter> filter) const
{
    if (_alternative.nul()) {
        filter->addBranch(pos, predicate->reduceAsExpr(), consequence.compile());
    } else {
        filter->addBranch(
                    pos, predicate->reduceAsExpr(), consequence.compile(), _alternative->compile());
    }
}

void Branch::acceptElse(misc::position const& else_pos, Block&& block)
{
    if (_alternative.not_nul()) {
        error::ifAlreadyMatchElse(_else_pos, else_pos);
    }
    _else_pos = else_pos;
    _alternative.reset(new Block(std::move(block)));
}

void BranchAlterOnly::compile(util::sref<semantic::Filter> filter) const
{
    filter->addBranchAlterOnly(pos, predicate->reduceAsExpr(), alternative.compile());
}

void Return::compile(util::sref<semantic::Filter> filter) const
{
    filter->addReturn(pos, ret_val->reduceAsExpr());
}

void ReturnNothing::compile(util::sref<semantic::Filter> filter) const
{
    filter->addReturnNothing(pos);
}

void NameDef::compile(util::sref<semantic::Filter> filter) const
{
    filter->defName(pos, name, init->reduceAsExpr());
}

void Import::compile(util::sref<semantic::Filter> filter) const
{
    filter->addImport(pos, names);
}

void Export::compile(util::sref<semantic::Filter> filter) const
{
    filter->addExport(pos, export_point, value->reduceAsExpr());
}

void AttrSet::compile(util::sref<semantic::Filter> filter) const
{
    filter->addAttrSet(pos, set_point->reduceAsLeftValue(), value->reduceAsExpr());
}
