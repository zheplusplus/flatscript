#include <algorithm>

#include <proto/function.h>
#include <proto/stmt-nodes.h>

#include "stmt-nodes.h"
#include "function.h"
#include "filter.h"
#include "symbol-table.h"

using namespace flchk;

util::sptr<proto::Statement const> Arithmetics::compile(util::sref<SymbolTable> st) const
{
    return util::mkptr(new proto::Arithmetics(expr->compile(st)));
}

util::sptr<proto::Statement const> Branch::compile(util::sref<SymbolTable> st) const
{
    return util::mkptr(new proto::Branch(predicate->compile(st)
                                       , consequence.compile(st)
                                       , alternative.compile(st)));
}

util::sptr<proto::Statement const> VarDef::compile(util::sref<SymbolTable> st) const
{
    util::sptr<proto::Expression const> init_value(init->compile(st));
    st->defVar(pos, name);
    return util::mkptr(new proto::VarDef(name, std::move(init_value)));
}

util::sptr<proto::Statement const> Return::compile(util::sref<SymbolTable> st) const
{
    return util::mkptr(new proto::Return(ret_val->compile(st)));
}

util::sptr<proto::Statement const> ReturnNothing::compile(util::sref<SymbolTable>) const
{
    return util::mkptr(new proto::ReturnNothing);
}
