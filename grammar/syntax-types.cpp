#include <algorithm>

#include <flowcheck/list-pipe.h>

#include "syntax-types.h"

using namespace grammar;

std::string OpImage::deliver()
{
    std::string img(std::move(_img));
    delete this;
    return std::move(img);
}

std::string Identifier::deliver()
{
    std::string id(std::move(_id));
    delete this;
    return std::move(id);
}

Strings* Strings::append(std::string const& value)
{
    _value += value;
    return this;
}

std::string Strings::deliver()
{
    std::string value(std::move(_value));
    delete this;
    return std::move(value);
}

NameList* NameList::add(std::string const& name)
{
    _names.push_back(name);
    return this;
}

std::vector<std::string> NameList::deliver()
{
    std::vector<std::string> names(std::move(_names));
    delete this;
    return std::move(names);
}

ArgList* ArgList::add(flchk::Expression const* expr)
{
    _args.push_back(util::mkptr(expr));
    return this;
}

std::vector<util::sptr<flchk::Expression const>> ArgList::deliver()
{
    std::vector<util::sptr<flchk::Expression const>> args(std::move(_args));
    delete this;
    return std::move(args);
}

DictContent* DictContent::add(flchk::Expression const* key, flchk::Expression const* value)
{
    _items.push_back(std::make_pair(util::mkptr(key), util::mkptr(value)));
    return this;
}

std::vector<DictContent::ItemType> DictContent::deliver()
{
    std::vector<ItemType> items(std::move(_items));
    delete this;
    return std::move(items);
}

util::sptr<flchk::PipeBase const> Pipeline::PipeMap::deliverCompile()
{
    return util::mkptr(new flchk::PipeMap(std::move(_expr)));
}

util::sptr<flchk::PipeBase const> Pipeline::PipeFilter::deliverCompile()
{
    return util::mkptr(new flchk::PipeFilter(std::move(_expr)));
}

Pipeline* Pipeline::add(util::sptr<PipeBase> pipe)
{
    _pipeline.push_back(std::move(pipe));
    return this;
}

std::vector<util::sptr<flchk::PipeBase const>> Pipeline::deliverCompile() const
{
    std::vector<util::sptr<flchk::PipeBase const>> pipeline;
    std::for_each(_pipeline.begin()
                , _pipeline.end()
                , [&](util::sptr<PipeBase> const& pipe)
                  {
                      pipeline.push_back(pipe->deliverCompile());
                  });
    delete this;
    return std::move(pipeline);
}
