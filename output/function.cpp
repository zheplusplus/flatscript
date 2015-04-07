#include <util/string.h>

#include "function.h"
#include "expr-nodes.h"
#include "name-mangler.h"

using namespace output;

namespace {

    struct MangledReference
        : Expression
    {
        MangledReference(misc::position const& pos, std::string const& n)
            : Expression(pos)
            , name(n)
        {}

        std::string str() const
        {
            return name;
        }

        std::string const name;
    };

}

void Function::write(std::ostream& os) const
{
    os << "function " << mangledName() << "(" << util::join(",", parameters()) << "){" << std::endl;
    body()->write(os);
    os << "}" << std::endl;
}

util::sptr<Expression const> Function::callMe(
        misc::position const& pos, util::ptrarr<Expression const> args) const
{
    return util::mkptr(new output::Call(
                pos, util::mkptr(new MangledReference(pos, mangledName())),
                std::move(args)));
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
    return formNames(params);
}

std::vector<std::string> RegularAsyncFunction::parameters() const
{
    std::vector<std::string> p(formNames(params));
    p.insert(p.begin() + async_param_index, TERM_REGULAR_ASYNC_CALLBACK);
    return p;
}

std::string RegularAsyncReturnCall::str() const
{
    return TERM_REGULAR_ASYNC_CALLBACK + "(null," + val->str() + ")";
}

util::sref<Statement const> AnonymousCallback::body() const
{
    return *_body;
}

std::string AnonymousCallback::mangledName() const
{
    return formAnonymousFunc(util::id(this));
}

util::sref<Block> AnonymousCallback::bodyFlow()
{
    return *_body;
}

std::vector<std::string> ConditionalCallback::parameters() const
{
    return std::vector<std::string>({ TERM_CONDITIONAL_CALLBACK_PARAMETER });
}

std::vector<std::string> NoParamCallback::parameters() const
{
    return std::vector<std::string>();
}

std::vector<std::string> AsyncCatcher::parameters() const
{
    return std::vector<std::string>({ TERM_EXCEPTION });
}
