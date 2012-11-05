#include <algorithm>

#include <util/string.h>

#include "stmt-nodes.h"
#include "name-mangler.h"

using namespace output;

void Branch::write(std::ostream& os) const
{
    os << "if (" << predicate->str(false) << ")" << std::endl;
    consequence->write(os);
    os << "else" << std::endl;
    alternative->write(os);
}

void Arithmetics::write(std::ostream& os) const
{
    os << expr->str(false) << ";" << std::endl;
}

void NameDef::write(std::ostream& os) const
{
    os << "const " << formName(name) << "=" << init->str(false) << ";" << std::endl;
}

void Return::write(std::ostream& os) const
{
    os << "return " << ret_val->str(false) << ";" << std::endl;
}

void ReturnNothing::write(std::ostream& os) const
{
    os << "return;" << std::endl;
}

void Export::write(std::ostream& os) const
{
    os << '{' << std::endl;
    for (size_t i = 2; i < export_point.size(); ++i) {
        std::string point(util::join(".", std::vector<std::string>(export_point.begin()
                                                                 , export_point.begin() + i)));
        os << point << "=" << point << " || {};" << std::endl;
    }
    os << util::join(".", export_point) << "=" << value->str(false) << ";}" << std::endl;
}

void AttrSet::write(std::ostream& os) const
{
    os << set_point->str(false) << "=" << value->str(false) << ";" << std::endl;
}
