#include <globals.h>
#include <semantic/stmt-nodes.h>
#include <semantic/expr-nodes.h>
#include <report/errors.h>

#include "stmt-nodes.h"
#include "function.h"
#include "class.h"

using namespace grammar;

namespace {

    struct FakeStatement
        : semantic::Statement
    {
        explicit FakeStatement(misc::position const& pos)
            : Statement(pos)
        {}

        void compile(semantic::BaseCompilingSpace&) const {}
    };

}

util::sptr<semantic::Statement const> Arithmetics::compile() const
{
    return util::mkptr(new semantic::Arithmetics(pos, expr->reduceAsExpr()));
}

util::sptr<semantic::Statement const> Branch::compile() const
{
    return util::mkptr(new semantic::Branch(
                    pos
                  , predicate->reduceAsExpr()
                  , consequence.compile()
                  , _alternative.nul() ? semantic::Block() : _alternative->compile()));
}

void Branch::acceptElse(misc::position const& else_pos, Block&& block)
{
    if (_alternative.not_nul()) {
        return error::partialStmtDupMatch(_else_pos, else_pos, "else", "if");
    }
    _else_pos = else_pos;
    _alternative.reset(new Block(std::move(block)));
}

util::sptr<semantic::Statement const> BranchAlterOnly::compile() const
{
    return util::mkptr(new semantic::Branch(
            pos, predicate->reduceAsExpr(), semantic::Block(), alternative.compile()));
}

util::sptr<semantic::Statement const> Return::compile() const
{
    return util::mkptr(new semantic::Return(pos, ret_val->reduceAsExpr()));
}

util::sptr<semantic::Statement const> ReturnNothing::compile() const
{
    return util::mkptr(new semantic::Return(pos, util::mkptr(new semantic::Undefined(pos))));
}

util::sptr<semantic::Statement const> NameDef::compile() const
{
    return util::mkptr(new semantic::NameDef(pos, name, init->reduceAsExpr()));
}

util::sptr<semantic::Statement const> Extern::compile() const
{
    return util::mkptr(new semantic::Extern(pos, names));
}

util::sptr<semantic::Statement const> Export::compile() const
{
    flats::Globals::g.use_export = true;
    return util::mkptr(new semantic::Export(pos, export_point, value->reduceAsExpr()));
}

util::sptr<semantic::Statement const> AttrSet::compile() const
{
    return util::mkptr(new semantic::AttrSet(
                                pos, set_point->reduceAsLeftValue(), value->reduceAsExpr()));
}

util::sptr<semantic::Statement const> ExceptionStall::compile() const
{
    if (_catch.nul()) {
        error::tryWithoutCatch(pos);
        return util::mkptr(new FakeStatement(pos));
    }
    return util::mkptr(new semantic::ExceptionStall(pos, flow.compile(), _catch->compile()));
}

void ExceptionStall::acceptCatch(misc::position const& catch_pos, Block&& block)
{
    if (_catch.not_nul()) {
        return error::partialStmtDupMatch(_catch_pos, catch_pos, "catch", "try");
    }
    _catch_pos = catch_pos;
    _catch.reset(new Block(std::move(block)));
}

util::sptr<semantic::Statement const> Throw::compile() const
{
    return util::mkptr(new semantic::Throw(pos, exception->reduceAsExpr()));
}
