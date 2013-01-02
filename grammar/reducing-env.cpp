#include <report/errors.h>

#include "reducing-env.h"

using namespace grammar;

bool ArgReducingEnv::inPipe() const
{
    return _external_env.inPipe();
}

void ArgReducingEnv::setAsync(
            misc::position const& pos, int index, std::vector<std::string> const& ap)
{
    if (isAsync()) {
        error::moreThanOneAsyncPlaceholder(pos);
    } else {
        _async_index = index;
        _async_params = ap;
    }
}

bool ArgReducingEnv::isAsync() const
{
    return _async_index != -1;
}

int ArgReducingEnv::asyncIndex() const
{
    return _async_index;
}

std::vector<std::string> const& ArgReducingEnv::asyncParams() const
{
    return _async_params;
}
