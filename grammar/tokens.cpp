#include <report/errors.h>

#include "node-base.h"
#include "tokens.h"
#include "automation-base.h"

using namespace grammar;

void Token::unexpected() const
{
    error::unexpectedToken(this->pos, this->image);
}

void TypedToken::act(AutomationStack& stack)
{
    stack.top()->nextToken(stack, *this);
}

void FactorToken::act(AutomationStack& stack)
{
    stack.top()->pushFactor(stack, *this);
}
