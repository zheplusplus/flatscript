#include <semantic/node-base.h>
#include <semantic/function.h>
#include <report/errors.h>

#include "block.h"
#include "node-base.h"
#include "function.h"
#include "class.h"

using namespace grammar;

namespace {

    struct PlaceholderStatement
        : Statement
    {
        PlaceholderStatement()
            : Statement(misc::position())
        {}

        util::sptr<semantic::Statement const> compile() const
        {
            return util::sptr<semantic::Statement const>(nullptr);
        }
    };

}

Block::Block()
    : _ctor(nullptr)
{
    _stmts.append(util::mkptr(new PlaceholderStatement));
}

void Block::addStmt(util::sptr<Statement> stmt)
{
    _stmts.append(std::move(stmt));
}

void Block::addFunc(util::sptr<Function const> func)
{
    _funcs.append(std::move(func));
}

void Block::addClass(util::sptr<Class const> cls)
{
    _classes.append(std::move(cls));
}

void Block::setCtor(misc::position const& pos, std::vector<std::string> params, Block body)
{
    if (this->_ctor.not_nul()) {
        return error::duplicateCtor(this->_ctor->pos, pos);
    }
    this->_ctor = util::mkptr(new Constructor(pos, std::move(params), std::move(body)));
}

void Block::acceptElse(misc::position const& else_pos, Block block)
{
    _stmts.back()->acceptElse(else_pos, std::move(block));
}

void Block::acceptCatch(misc::position const& catch_pos, Block block)
{
    _stmts.back()->acceptCatch(catch_pos, std::move(block));
}

semantic::Block Block::compile() const
{
    semantic::Block block;
    _classes.iter([&](util::sptr<Class const> const& cls, int)
                  {
                      block.addClass(cls->compile());
                  });
    _funcs.iter([&](util::sptr<Function const> const& func, int)
                {
                    block.addFunc(func->compile());
                });
    _stmts.iter([&](util::sptr<Statement> const& stmt, int)
                {
                    block.addStmt(stmt->compile());
                }, 1);
    return std::move(block);
}
