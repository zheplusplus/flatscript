#include <report/errors.h>

#include "reducing-env.h"

using namespace grammar;

void ArgReducingEnv::setAsync(
            misc::position const& pos, int index, std::vector<std::string> const& ap)
{
    if (isAsync()) {
        return error::moreThanOneAsyncPlaceholder(pos);
    }
    _async_index = index;
    _async_params = ap;
}

void ArgReducingEnv::setRegularAsync(misc::position const& pos, int index)
{
    setAsync(pos, index, std::vector<std::string>());
    _regular_async = true;
}

bool ArgReducingEnv::isAsync() const
{
    return _async_index != -1;
}

bool ArgReducingEnv::isRegularAsync() const
{
    return _regular_async;
}

int ArgReducingEnv::asyncIndex() const
{
    return _async_index;
}

std::vector<std::string> const& ArgReducingEnv::asyncParams() const
{
    return _async_params;
}

void ParamReducingEnv::addParam(std::string const& param)
{
    _params.push_back(param);
}

void ParamReducingEnv::setAsync(misc::position const& pos, int index)
{
    if (_async_index != -1) {
        return error::moreThanOneAsyncPlaceholder(pos);
    }
    _async_index = index;
}

int ParamReducingEnv::asyncIndex() const
{
    return _async_index;
}

std::vector<std::string> const& ParamReducingEnv::params() const
{
    return _params;
}
