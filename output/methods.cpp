#include "methods.h"
#include "name-mangler.h"

using namespace output;

Method method::throwExc()
{
    return [](std::string const& exception)
           {
               return "throw " + exception + ";";
           };
}

Method method::callbackExc()
{
    return [](std::string const& exception)
           {
               return "return " + formName(term::regularAsyncCallback()) + "(" + exception + ");";
           };
}

Method method::asyncCatcher(std::string const& catcher_func_name)
{
    return [=](std::string const& exception)
           {
               return "return " + catcher_func_name + "(" + exception + ");";
           };
}
