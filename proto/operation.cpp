#include <map>

#include "operation.h"

std::string proto::mapUnaryOp(std::string const& op)
{
    return op;
}

std::string proto::mapBinaryOp(std::string const& op)
{
    return "=" == op ? "==" : op;
}
