#include <iostream>
#include <globals.h>
#include <semantic/stmt-nodes.h>
#include <semantic/loop.h>
#include <semantic/block.h>
#include <report/errors.h>

#include "stmt-nodes.h"

using namespace grammar;

namespace {

    struct FakeStatement
        : semantic::Statement
    {
        explicit FakeStatement(misc::position const& pos)
            : Statement(pos)
        {}

        void compile(util::sref<semantic::Scope>) const {}

        bool isAsync() const { return false; }
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
          , consequence->compile()
          , _alternative.nul() ? util::sptr<semantic::Statement const>(new semantic::Block(pos))
                               : _alternative->compile()));
}

void Branch::acceptElse(misc::position const& else_pos, util::sptr<Statement const> block)
{
    if (_alternative.not_nul()) {
        return error::partialStmtDupMatch(_else_pos, else_pos, "else", "if");
    }
    _else_pos = else_pos;
    _alternative = std::move(block);
}

util::sptr<semantic::Statement const> BranchAlterOnly::compile() const
{
    return util::mkptr(new semantic::Branch(
            pos, predicate->reduceAsExpr(),
            util::mkptr(new semantic::Block(pos)), alternative->compile()));
}

util::sptr<semantic::Statement const> Return::compile() const
{
    return util::mkptr(new semantic::Return(pos, ret_val->reduceAsExpr()));
}

util::sptr<semantic::Statement const> NameDef::compile() const
{
    return util::mkptr(new semantic::NameDef(pos, name, init->reduceAsExpr()));
}

util::sptr<semantic::Statement const> Extern::compile() const
{
    for (auto const& name: this->names) {
        if (flats::isReserved(name)) {
            error::importReservedWord(pos, name);
        }
    }
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
    if (this->_except_name.empty()) {
        std::cerr << this->pos.str()
                  << ": Deprecated catch syntax; use `catch exception_name` instead" << std::endl;
        return util::mkptr(new semantic::ExceptionStallDeprecated(
                    pos, flow->compile(), _catch->compile()));
    }
    return util::mkptr(new semantic::ExceptionStall(
                pos, this->flow->compile(), this->_except_name, this->_catch->compile()));
}

void ExceptionStall::acceptCatch(misc::position const& catch_pos,
                                 util::sptr<Statement const> block, std::string except_name)
{
    if (this->_catch.not_nul()) {
        return error::partialStmtDupMatch(this->_catch_pos, catch_pos, "catch", "try");
    }
    this->_catch_pos = catch_pos;
    this->_catch = std::move(block);
    this->_except_name = std::move(except_name);
}

util::sptr<semantic::Statement const> Throw::compile() const
{
    return util::mkptr(new semantic::Throw(pos, exception->reduceAsExpr()));
}

util::sptr<semantic::Statement const> Break::compile() const
{
    return util::mkptr(new semantic::Break(pos));
}

util::sptr<semantic::Statement const> Continue::compile() const
{
    return util::mkptr(new semantic::Continue(pos));
}

util::sptr<semantic::Statement const> RangeIteration::compile() const
{
    return util::mkptr(new semantic::RangeIteration(
            this->pos, this->reference, this->begin->reduceAsExpr(), this->end->reduceAsExpr()
          , this->step->reduceAsExpr(), this->loop->compile()));
}

util::sptr<semantic::Statement const> IncludeFile::compile() const
{
    return util::mkptr(new semantic::IncludeFile(this->pos, this->file, this->module_alias));
}
