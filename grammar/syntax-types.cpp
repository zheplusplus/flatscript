#include "syntax-types.h"
#include "node-base.h"

using namespace grammar;

std::string Ident::deliver()
{
    std::string id(std::move(_id));
    delete this;
    return std::move(id);
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

TokenSequence::TokenSequence(Token* token)
{
    _list.push_back(util::mkptr(token));
}

TokenSequence* TokenSequence::add(Token* token)
{
    _list.push_back(util::mkptr(token));
    return this;
}

std::vector<util::sptr<Token>> TokenSequence::deliver()
{
    std::vector<util::sptr<Token>> list(std::move(_list));
    delete this;
    return std::move(list);
}
