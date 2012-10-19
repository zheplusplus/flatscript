#include <algorithm>
#include <iostream>

#include "stmt-nodes.h"
#include "name-mangler.h"

using namespace proto;

void Branch::write() const
{
    std::cout << "if (" << predicate->stringify(false) << ")" << std::endl;
    consequence->write();
    std::cout << "else" << std::endl;
    alternative->write();
}

void Arithmetics::write() const
{
    std::cout << expr->stringify(false) << ";" << std::endl;
}

void NameDef::write() const
{
    std::cout << "const " << formName(name) << "=" << init->stringify(false) << ";" << std::endl;
}

void Return::write() const
{
    std::cout << "return " << ret_val->stringify(false) << ";" << std::endl;
}

void ReturnNothing::write() const
{
    std::cout << "return;" << std::endl;
}

void Import::write() const
{
    std::for_each(names.begin()
                , names.end()
                , [&](std::string const& name)
                  {
                      std::cout << "const " << formName(name) << "=" << name << ";" << std::endl;
                  });
}

void AttrSet::write() const
{
    std::cout << set_point->stringify(false) << "=" << value->stringify(false) << ";" << std::endl;
}
