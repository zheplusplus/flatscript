#include <algorithm>

#include <output/function.h>
#include <output/stmt-nodes.h>
#include <output/expr-nodes.h>

#include "stmt-nodes.h"
#include "function.h"
#include "compiling-space.h"

using namespace semantic;

void Arithmetics::compile(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> cexpr(expr->compile(space));
    space.block()->addStmt(util::mkptr(new output::Arithmetics(std::move(cexpr))));
}

bool Arithmetics::isAsync() const
{
    return expr->isAsync();
}

void Branch::compile(BaseCompilingSpace& space) const
{
    if (predicate->isLiteral(space.sym())) {
        Block const& block(predicate->boolValue(space.sym()) ? consequence : alternative);
        return space.block()->addStmt(block.compile(SubCompilingSpace(space)));
    }
    util::sptr<output::Expression const> compiled_pred(predicate->compile(space));
    util::sptr<output::Statement const> consq_stmt(consequence.compile(SubCompilingSpace(space)));
    util::sptr<output::Statement const> alter_stmt(alternative.compile(SubCompilingSpace(space)));
    space.block()->addStmt(util::mkptr(new output::Branch(
                        std::move(compiled_pred), std::move(consq_stmt), std::move(alter_stmt))));
}

bool Branch::isAsync() const
{
    return predicate->isAsync() || consequence.isAsync() || alternative.isAsync();
}

void NameDef::compile(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> init_value(init->compile(space));
    if (init->isLiteral(space.sym())) {
        return space.sym()->defConst(pos, name, *init);
    }
    space.sym()->defName(pos, name);
    space.block()->addStmt(util::mkptr(new output::Arithmetics(util::mkptr(new output::Assignment(
                                                pos
                                              , util::mkptr(new output::Reference(pos, name))
                                              , std::move(init_value))))));
}

bool NameDef::isAsync() const
{
    return init->isAsync();
}

void Return::compile(BaseCompilingSpace& space) const
{
    space.terminate();
    util::sptr<output::Expression const> ret(space.ret(*ret_val));
    space.block()->addStmt(util::mkptr(new output::Return(std::move(ret))));
}

bool Return::isAsync() const
{
    return ret_val->isAsync();
}

void Import::compile(BaseCompilingSpace& space) const
{
    std::for_each(names.begin()
                , names.end()
                , [&](std::string const& name)
                  {
                      space.sym()->imported(pos, name);
                  });
}

void Export::compile(BaseCompilingSpace& space) const
{
    space.sym()->compileRef(pos, export_point[0]);
    util::sptr<output::Expression const> cval(value->compile(space));
    space.block()->addStmt(util::mkptr(new output::Export(export_point, std::move(cval))));
}

bool Export::isAsync() const
{
    return value->isAsync();
}

void AttrSet::compile(BaseCompilingSpace& space) const
{
    util::sptr<output::Expression const> csp(set_point->compile(space));
    util::sptr<output::Expression const> cval(value->compile(space));
    space.block()->addStmt(util::mkptr(new output::Arithmetics(util::mkptr(new output::Assignment(
                                                pos, std::move(csp), std::move(cval))))));
}

bool AttrSet::isAsync() const
{
    return set_point->isAsync() || value->isAsync();
}
