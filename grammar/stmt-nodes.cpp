#include <flowcheck/node-base.h>
#include <flowcheck/symbol-def-filter.h>

#include "stmt-nodes.h"
#include "function.h"

using namespace grammar;

static util::sptr<flchk::Filter> mkSymDefFilter(util::sref<flchk::Filter> ext_filter)
{
    return util::mkptr(new flchk::SymbolDefFilter(ext_filter->getSymbols()));
}

void Arithmetics::compile(util::sref<flchk::Filter> filter)
{
    filter->addArith(pos, std::move(expr));
}

void Branch::compile(util::sref<flchk::Filter> filter)
{
    filter->addBranch(pos
                    , std::move(predicate)
                    , consequence.compile(mkSymDefFilter(filter))
                    , alternative.compile(mkSymDefFilter(filter)));
}

void BranchConsqOnly::compile(util::sref<flchk::Filter> filter)
{
    filter->addBranch(pos, std::move(predicate), consequence.compile(mkSymDefFilter(filter)));
}

void BranchAlterOnly::compile(util::sref<flchk::Filter> filter)
{
    filter->addBranchAlterOnly(
            pos, std::move(predicate), alternative.compile(mkSymDefFilter(filter)));
}

void Return::compile(util::sref<flchk::Filter> filter)
{
    filter->addReturn(pos, std::move(ret_val));
}

void ReturnNothing::compile(util::sref<flchk::Filter> filter)
{
    filter->addReturnNothing(pos);
}

void NameDef::compile(util::sref<flchk::Filter> filter)
{
    filter->defName(pos, name, std::move(init));
}

void Import::compile(util::sref<flchk::Filter> filter)
{
    filter->addImport(pos, names);
}

void AttrSet::compile(util::sref<flchk::Filter> filter)
{
    filter->addAttrSet(pos, std::move(set_point), std::move(value));
}
