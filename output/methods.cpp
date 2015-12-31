#include <functional>
#include <util/string.h>

#include "methods.h"
#include "name-mangler.h"

using namespace output;
typedef output::method::_Method MethodBase;

namespace {

    struct MethodImpl
        : MethodBase
    {
        typedef std::function<std::string(std::string const&)> F;

        MethodImpl(F g, bool t)
            : f(std::move(g))
            , spontaneous_throwing(t)
        {}

        F f;
        bool const spontaneous_throwing;

        std::string scheme(std::string const& e) const
        {
            return this->f(e);
        }

        bool mayThrow(util::sptr<Expression const> const& e) const
        {
            return this->spontaneous_throwing || e->mayThrow();
        }
    };

    template <typename F>
    Method make_method(F f, bool spontaneous_throwing)
    {
        return util::mkptr(new MethodImpl(std::move(f), spontaneous_throwing));
    }

}

Method method::place()
{
    return make_method(
        [](std::string const& e)
        {
            return e + ";";
        }, false);
}

Method method::throwExc()
{
    return make_method(
        [](std::string const& exception)
        {
            return "throw " + exception + ";";
        }, true);
}

Method method::callbackExc()
{
    return make_method(
        [](std::string const& exception)
        {
            return "return " + TERM_REGULAR_ASYNC_CALLBACK + "(" + exception + ");";
        }, false);
}

Method method::asyncCatcher(std::string catcher_func_name)
{
    return make_method(
        [=](std::string const& exception)
        {
            return "return " + catcher_func_name + "(" + exception + ");";
        }, false);
}

Method method::ret()
{
    return make_method(
        [](std::string const& r)
        {
            return "return " + r + ";";
        }, false);
}

Method method::asyncRet()
{
    return make_method(
        [](std::string const& r)
        {
            return "return " + TERM_REGULAR_ASYNC_CALLBACK + "(null," + r + ");";
        }, false);
}

Method method::syncPipeRet(util::uid pipe_id)
{
    return make_method(
        [=](std::string const& r)
        {
            return
                util::replace_all(
                util::replace_all(
                    "{"
                        "$retf#PIPE_ID = true;"
                        "return $ret#PIPE_ID = #RESULT;"
                    "}"
                        , "#RESULT", r)
                        , "#PIPE_ID", pipe_id.str())
                ;
        }, false);
}

Method method::syncBreak()
{
    return make_method(
        [](std::string const& r)
        {
            return "return " + r + "=true;";
        }, false);
}

Method method::callNext()
{
    return make_method(
        [](std::string const& r)
        {
            return "return " + r + "();";
        }, false);
}
