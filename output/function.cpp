#include <util/string.h>

#include "function.h"
#include "node-base.h"
#include "name-mangler.h"

using namespace output;

void Function::write(std::ostream& os) const
{
    os << "function " << mangledName() << "(" << util::join(",", formNames(parameters())) << ") {"
       << std::endl;
    body()->write(os);
    os << "}" << std::endl;
}

util::sref<Statement const> RegularFunction::body() const
{
    return *body_stmt;
}

std::string RegularFunction::mangledName() const
{
    return formName(name);
}

std::vector<std::string> RegularFunction::parameters() const
{
    return params;
}

std::vector<std::string> RegularAsyncFunction::parameters() const
{
    std::vector<std::string> p(params);
    p.insert(p.begin() + async_param_index, term::regularAsyncCallback());
    return p;
}

std::string RegularAsyncReturnCall::str() const
{
    return formName(term::regularAsyncCallback()) + "(null," + val->str() + ")";
}

util::sref<Statement const> ConditionalCallback::body() const
{
    return *_body;
}

std::string ConditionalCallback::mangledName() const
{
    return formAnonymousFunc(util::id(this));
}

std::vector<std::string> ConditionalCallback::parameters() const
{
    return std::vector<std::string>({ "$ccp" });
}

util::sref<Block> ConditionalCallback::bodyFlow()
{
    return *_body;
}
