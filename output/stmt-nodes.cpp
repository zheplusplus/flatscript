#include <util/string.h>

#include "stmt-nodes.h"
#include "name-mangler.h"

using namespace output;

void Branch::write(std::ostream& os) const
{
    if (0 == consequence->count() && 0 == alternative->count()) {
        os << predicate->str() << ";" << std::endl;
        return;
    }
    os << "if (" << predicate->str() << ")" << std::endl;
    if (consequence->count() != 0) {
        os << "{" << std::endl;
        consequence->write(os);
        os << "}" << std::endl;
    } else {
        os << ";" << std::endl;
    }
    if (alternative->count() != 0) {
        os << "else {" << std::endl;
        alternative->write(os);
        os << "}" << std::endl;
    }
}

void Arithmetics::write(std::ostream& os) const
{
    os << expr->str() << ";" << std::endl;
}

void AsyncCallResultDef::write(std::ostream& os) const
{
    os << "var " << formAsyncRef(async_result.id()) << "=" << async_result->str() << ";"
       << std::endl;
}

void Return::write(std::ostream& os) const
{
    os << "return " << ret_val->str() << ";" << std::endl;
}

void Export::write(std::ostream& os) const
{
    for (size_t i = 2; i < export_point.size(); ++i) {
        std::string point(util::join(".", std::vector<std::string>(export_point.begin()
                                                                 , export_point.begin() + i)));
        os << point << "=" << point << " || {};" << std::endl;
    }
    os << util::join(".", export_point) << "=" << value->str() << ";" << std::endl;
}

int Export::count() const
{
    return export_point.size() - 1;
}

void ThisDeclaration::write(std::ostream& os) const
{
    os << "var $this = this;" << std::endl;
}
