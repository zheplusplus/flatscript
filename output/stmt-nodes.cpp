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

void AsyncCallResultDef::write(std::ostream& os) const
{
    if (this->need_decl) {
        os << "var " << formAsyncRef(async_result.id()) << "=";
    }
    os << async_result->str() << ";" << std::endl;
}

static std::vector<std::string> add_export_root(std::vector<std::string> export_point)
{
    std::vector<std::string> r;
    r.reserve(1 + export_point.size());
    r.push_back("$export");
    for (std::string& e: export_point) {
        r.push_back(std::move(e));
    }
    return std::move(r);
}

Export::Export(std::vector<std::string> e, util::sptr<Expression const> v)
    : export_point(::add_export_root(std::move(e)))
    , value(std::move(v))
{}

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

void ExceptionStall::write(std::ostream& os) const
{
    if (0 == try_block->count()) {
        return;
    }
    if (!try_block->mayThrow()) {
        try_block->write(os);
        return;
    }
    os << "try{" << std::endl;
    try_block->write(os);
    os << "}catch($exception){" << std::endl;
    catch_block->write(os);
    os << "}" << std::endl;
}

int ExceptionStall::count() const
{
    return std::max(try_block->count(), 1);
}

void ExprScheme::write(std::ostream& os) const
{
    os << method->scheme(expr) << std::endl;
}
