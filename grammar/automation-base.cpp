#include <report/errors.h>

#include "node-base.h"
#include "stmt-automations.h"
#include "function.h"

using namespace grammar;

void AutomationBase::setPrevious(util::sref<AutomationBase const> previous)
{
    _previous = previous;
}

void AutomationStack::push(util::sptr<AutomationBase> automation)
{
    if (!empty()) {
        automation->setPrevious(top());
    }
    _stack.push_back(std::move(automation));
    top()->activated(*this);
    top()->resumed(*this);
}

void AutomationStack::replace(util::sptr<AutomationBase> automation)
{
    _stack.pop_back();
    push(std::move(automation));
}

util::sref<AutomationBase> AutomationStack::top() const
{
    return *_stack.back();
}

void AutomationStack::reduced(util::sptr<Expression const> expr)
{
    _stack.pop_back();
    top()->accepted(*this, std::move(expr));
    top()->resumed(*this);
}

void AutomationStack::reduced(std::vector<util::sptr<Expression const>> list)
{
    _stack.pop_back();
    top()->accepted(*this, std::move(list));
    top()->resumed(*this);
}

void AutomationStack::pop()
{
    _stack.pop_back();
    if (!empty()) {
        top()->resumed(*this);
    }
}

bool AutomationStack::empty() const
{
    return _stack.empty();
}

void AutomationBase::pushOp(AutomationStack&, Token const& token)
{
    error::unexpectedToken(token.pos, token.image);
}

void AutomationBase::pushFactor(
                AutomationStack&, util::sptr<Expression const> factor, std::string const& image)
{
    error::unexpectedToken(factor->pos, image);
}

void AutomationBase::pushOpenParen(AutomationStack&, misc::position const& pos)
{
    error::unexpectedToken(pos, "(");
}

void AutomationBase::pushOpenBracket(AutomationStack&, misc::position const& pos)
{
    error::unexpectedToken(pos, "[");
}

void AutomationBase::pushOpenBrace(AutomationStack&, misc::position const& pos)
{
    error::unexpectedToken(pos, "{");
}

void AutomationBase::pushColon(AutomationStack&, misc::position const& pos)
{
    error::unexpectedToken(pos, ":");
}

void AutomationBase::pushPropertySeparator(AutomationStack&, misc::position const& pos)
{
    error::unexpectedToken(pos, "::");
}

void AutomationBase::pushComma(AutomationStack&, misc::position const& pos)
{
    error::unexpectedToken(pos, ",");
}

void AutomationBase::matchClosing(AutomationStack&, Token const& closer)
{
    error::unexpectedToken(closer.pos, closer.image);
}

void ClauseBase::acceptElse(misc::position const& else_pos)
{
    error::elseNotMatchIf(else_pos);
}

bool ClauseBase::shrinkOn(int level) const
{
    return level <= indent;
}

void ClauseBase::nextToken(util::sptr<Token> const& token)
{
    token->act(_stack);
}

bool ClauseBase::tryFinish(misc::position const& pos, std::vector<util::sptr<ClauseBase>>& clauses)
{
    if ((!_stack.empty()) && _stack.top()->finishOnBreak(true)) {
        ClauseStackWrapper wrapper(_member_indent, _stack, pos, clauses);
        _stack.top()->finish(wrapper, _stack, pos);
        return true;
    }
    return _stack.empty();
}

void ClauseBase::prepareArith()
{
    _stack.push(util::mkptr(new grammar::ExprStmtAutomation(util::mkref(*this))));
}

void ClauseBase::prepareReturn()
{
    _stack.push(util::mkptr(new grammar::ReturnAutomation(util::mkref(*this))));
}

void ClauseBase::prepareExport(std::vector<std::string> const& names)
{
    _stack.push(util::mkptr(new grammar::ExportStmtAutomation(util::mkref(*this), names)));
}

void ClauseBase::setMemberIndent(int level, misc::position const& pos)
{
    if (-1 == _member_indent) {
        _member_indent = level;
        return;
    }
    if (level != _member_indent) {
        error::invalidIndent(pos);
    }
}

namespace {

    struct BlockReceiver
        : ClauseBase
    {
        BlockReceiver(int level
                    , AutomationStack& stack
                    , misc::position const& pos
                    , util::sref<AutomationBase> blockRecr)
            : ClauseBase(level)
            , _stack(stack)
            , _pos(pos)
            , _blockRecr(blockRecr)
        {}

        void acceptFunc(util::sptr<Function const> func)
        {
            _block.addFunc(std::move(func));
        }

        void acceptStmt(util::sptr<Statement> stmt)
        {
            _block.addStmt(std::move(stmt));
        }

        void deliver()
        {
            _blockRecr->accepted(_stack, _pos, std::move(_block));
        }

        bool shrinkOn(int level) const
        {
            if (_member_indent == -1) {
                return ClauseBase::shrinkOn(level);
            }
            return level < _member_indent;
        }
    private:
        AutomationStack& _stack;
        misc::position const _pos;
        Block _block;
        util::sref<AutomationBase> _blockRecr;
    };

}

void ClauseStackWrapper::pushBlockReceiver(util::sref<AutomationBase> blockRecr)
{
    _clauses.push_back(util::mkptr(new BlockReceiver(_last_indent, _stack, _pos, blockRecr)));
}
