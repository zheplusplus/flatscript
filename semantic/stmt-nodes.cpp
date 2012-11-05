#include <algorithm>

#include <output/function.h>
#include <output/stmt-nodes.h>

#include "stmt-nodes.h"
#include "function.h"
#include "filter.h"
#include "symbol-table.h"

using namespace semantic;

void Arithmetics::compile(util::sref<SymbolTable> st, util::sref<output::Block> block) const
{
    block->addStmt(util::mkptr(new output::Arithmetics(expr->compile(st))));
}

void Branch::compile(util::sref<SymbolTable> st, util::sref<output::Block> block) const
{
    if (predicate->isLiteral(st)) {
        (predicate->boolValue(st) ? consequence : alternative).compile(st, block);
        return;
    }
    util::sptr<output::Block> consq(new output::Block);
    consequence.compile(st, *consq);
    util::sptr<output::Block> alter(new output::Block);
    alternative.compile(st, *alter);
    block->addStmt(util::mkptr(new output::Branch(
                        predicate->compile(st), std::move(consq), std::move(alter))));
}

void NameDef::compile(util::sref<SymbolTable> st, util::sref<output::Block> block) const
{
    util::sptr<output::Expression const> init_value(init->compile(st));
    if (init->isLiteral(st)) {
        st->defConst(pos, name, *init);
        return;
    }
    st->defName(pos, name);
    block->addStmt(util::mkptr(new output::NameDef(name, std::move(init_value))));
}

void Return::compile(util::sref<SymbolTable> st, util::sref<output::Block> block) const
{
    block->addStmt(util::mkptr(new output::Return(ret_val->compile(st))));
}

void ReturnNothing::compile(util::sref<SymbolTable>, util::sref<output::Block> block) const
{
    block->addStmt(util::mkptr(new output::ReturnNothing));
}

void Import::compile(util::sref<SymbolTable> st, util::sref<output::Block>) const
{
    std::for_each(names.begin()
                , names.end()
                , [&](std::string const& name)
                  {
                      st->imported(pos, name);
                  });
}

void Export::compile(util::sref<SymbolTable> st, util::sref<output::Block> block) const
{
    st->compileRef(pos, export_point[0]);
    block->addStmt(util::mkptr(new output::Export(export_point, value->compile(st))));
}

void AttrSet::compile(util::sref<SymbolTable> st, util::sref<output::Block> block) const
{
    block->addStmt(util::mkptr(new output::AttrSet(set_point->compile(st), value->compile(st))));
}
