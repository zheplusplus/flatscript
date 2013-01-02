#include <algorithm>

#include <output/function.h>
#include <output/stmt-nodes.h>

#include "stmt-nodes.h"
#include "function.h"
#include "filter.h"
#include "compiling-space.h"

using namespace semantic;

void Arithmetics::compile(CompilingSpace& space) const
{
    util::sptr<output::Expression const> cexpr(expr->compile(space));
    space.block()->addStmt(util::mkptr(new output::Arithmetics(std::move(cexpr))));
}

void Branch::compile(CompilingSpace& space) const
{
    if (predicate->isLiteral(space.sym())) {
        CompilingSpace branch_space(space.sym());
        (predicate->boolValue(space.sym()) ? consequence : alternative).compile(branch_space);
        space.block()->append(branch_space.deliver());
        return;
    }
    util::sptr<output::Expression const> compiled_pred(predicate->compile(space));
    CompilingSpace consq_space(space.sym());
    consequence.compile(consq_space);
    CompilingSpace alter_space(space.sym());
    alternative.compile(alter_space);
    space.block()->addStmt(util::mkptr(new output::Branch(
                        std::move(compiled_pred), consq_space.deliver(), alter_space.deliver())));
}

void NameDef::compile(CompilingSpace& space) const
{
    util::sptr<output::Expression const> init_value(init->compile(space));
    if (init->isLiteral(space.sym())) {
        space.sym()->defConst(pos, name, *init);
        return;
    }
    space.sym()->defName(pos, name);
    space.block()->addStmt(util::mkptr(new output::NameDef(name, std::move(init_value))));
}

void Return::compile(CompilingSpace& space) const
{
    util::sptr<output::Expression const> cret(ret_val->compile(space));
    space.block()->addStmt(util::mkptr(new output::Return(std::move(cret))));
}

void ReturnNothing::compile(CompilingSpace& space) const
{
    space.block()->addStmt(util::mkptr(new output::ReturnNothing));
}

void Import::compile(CompilingSpace& space) const
{
    std::for_each(names.begin()
                , names.end()
                , [&](std::string const& name)
                  {
                      space.sym()->imported(pos, name);
                  });
}

void Export::compile(CompilingSpace& space) const
{
    space.sym()->compileRef(pos, export_point[0]);
    util::sptr<output::Expression const> cval(value->compile(space));
    space.block()->addStmt(util::mkptr(new output::Export(export_point, std::move(cval))));
}

void AttrSet::compile(CompilingSpace& space) const
{
    util::sptr<output::Expression const> csp(set_point->compile(space));
    util::sptr<output::Expression const> cval(value->compile(space));
    space.block()->addStmt(util::mkptr(new output::AttrSet(std::move(csp), std::move(cval))));
}
