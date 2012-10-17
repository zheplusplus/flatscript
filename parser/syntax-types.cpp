#include <algorithm>

#include <grammar/list-pipe.h>

#include "syntax-types.h"

using namespace parser;

Strings* Strings::append(std::string const& value)
{
    _value += value;
    return this;
}

std::string Strings::deliver()
{
    return std::move(_value);
}

ParamNames* ParamNames::add(std::string const& name)
{
    _names.push_back(name);
    return this;
}

std::vector<std::string> ParamNames::get() const
{
    return _names;
}

ArgList* ArgList::add(grammar::Expression const* expr)
{
    _params.push_back(util::mkptr(expr));
    return this;
}

std::vector<util::sptr<grammar::Expression const>> ArgList::deliver()
{
    return std::move(_params);
}

util::sptr<grammar::PipeBase const> Pipeline::PipeMap::deliverCompile()
{
    return util::mkptr(new grammar::PipeMap(std::move(_expr)));
}

util::sptr<grammar::PipeBase const> Pipeline::PipeFilter::deliverCompile()
{
    return util::mkptr(new grammar::PipeFilter(std::move(_expr)));
}

Pipeline* Pipeline::add(util::sptr<PipeBase> pipe)
{
    _pipeline.push_back(std::move(pipe));
    return this;
}

std::vector<util::sptr<grammar::PipeBase const>> Pipeline::deliverCompile() const
{
    std::vector<util::sptr<grammar::PipeBase const>> pipeline;
    std::for_each(_pipeline.begin()
                , _pipeline.end()
                , [&](util::sptr<PipeBase> const& pipe)
                  {
                      pipeline.push_back(pipe->deliverCompile());
                  });
    return std::move(pipeline);
}
