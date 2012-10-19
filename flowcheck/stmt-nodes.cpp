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

util::sptr<proto::Statement const> NameDef::compile(util::sref<SymbolTable> st) const
{
    util::sptr<proto::Expression const> init_value(init->compile(st));
    st->defName(pos, name);
    return util::mkptr(new proto::NameDef(name, std::move(init_value)));
}

util::sptr<proto::Statement const> Return::compile(util::sref<SymbolTable> st) const
{
    return util::mkptr(new proto::Return(ret_val->compile(st)));
}

util::sptr<proto::Statement const> ReturnNothing::compile(util::sref<SymbolTable>) const
{
    return util::mkptr(new proto::ReturnNothing);
}

util::sptr<proto::Statement const> Import::compile(util::sref<SymbolTable> st) const
{
    std::for_each(names.begin()
                , names.end()
                , [&](std::string const& name)
                  {
                      st->defName(pos, name);
                  });
    return util::mkptr(new proto::Import(names));
}

util::sptr<proto::Statement const> AttrSet::compile(util::sref<SymbolTable> st) const
{
    return util::mkptr(new proto::AttrSet(set_point->compile(st), value->compile(st)));
}
