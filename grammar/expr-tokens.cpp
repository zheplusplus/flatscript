#include "expr-tokens.h"
#include "expr-automations.h"
#include "function.h"

using namespace grammar;

void OpToken::act(AutomationStack& stack)
{
    stack.top()->pushOp(stack, *this);
}

void FactorToken::act(AutomationStack& stack)
{
    stack.top()->pushFactor(stack, std::move(factor), image);
}

void OpenParenToken::act(AutomationStack& stack)
{
    stack.top()->pushOpenParen(stack, pos);
}

void OpenBracketToken::act(AutomationStack& stack)
{
    stack.top()->pushOpenBracket(stack, pos);
}

void OpenBraceToken::act(AutomationStack& stack)
{
    stack.top()->pushOpenBrace(stack, pos);
}

void CloserToken::act(AutomationStack& stack)
{
    stack.top()->matchClosing(stack, *this);
}

void ColonToken::act(AutomationStack& stack)
{
    stack.top()->pushColon(stack, pos);
}

void CommaToken::act(AutomationStack& stack)
{
    stack.top()->pushComma(stack, pos);
}
