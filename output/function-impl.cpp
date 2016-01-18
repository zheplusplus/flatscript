#include <util/string.h>

#include "function-impl.h"
#include "expr-nodes.h"
#include "name-mangler.h"

using namespace output;

namespace {

    struct MangledReference
        : Expression
    {
        explicit MangledReference(std::string const& n)
            : name(n)
        {}

        std::string str() const { return name; }
        bool mayThrow() const { return false; }

        std::string const name;
    };

}

void Function::write(std::ostream& os) const
{
    os << "function " << mangledName() << "(" << util::join(",", parameters()) << "){" << std::endl;
    body()->write(os);
    os << "}" << std::endl;
}

util::sptr<Expression const> Function::callMe(util::ptrarr<Expression const> args) const
{
    return util::mkptr(new Call(util::mkptr(new MangledReference(mangledName())), std::move(args)));
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

util::sref<Statement const> AnonymousCallback::body() const
{
    return *_body;
}

std::string AnonymousCallback::mangledName() const
{
    return formAnonymousFunc(this->id);
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

std::vector<std::string> AsyncCatchFunc::parameters() const
{
    return std::vector<std::string>({ formSubName(this->except_name, this->catch_id) });
}

std::vector<std::string> AsyncCatcherDeprecated::parameters() const
{
    return std::vector<std::string>({ TERM_EXCEPTION });
}

std::string ModuleInitFunc::mangledName() const
{
    return formModuleFuncName(this->module_id);
}

std::vector<std::string> ModuleInitFunc::parameters() const
{
    return {TERM_EXPORT};
}

util::sptr<Expression const> ModuleInitFunc::exportArg() const
{
    return util::mkptr(new Assignment(util::mkptr(new InitTarget(this->module_id)),
                                      util::mkptr(new Dictionary)));
}

std::string ModuleInitFunc::InitTarget::str() const
{
    return formModuleExportName(this->module_id);
}
