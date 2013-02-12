#include "node-base.h"
#include "tokens.h"
#include "automation-base.h"

using namespace grammar;

void TypedToken::act(AutomationStack& stack)
{
    stack.top()->nextToken(stack, *this);
}

void FactorToken::act(AutomationStack& stack)
{
    stack.top()->pushFactor(stack, std::move(factor), image);
}
