#ifndef __STEKIN_OUTPUT_METHODS_H__
#define __STEKIN_OUTPUT_METHODS_H__

#include <string>
#include <functional>

namespace output {

    typedef std::function<std::string (std::string const&)> Method;

    namespace method {

        Method throwExc();
        Method callbackExc();
        Method asyncCatcher(std::string const& catcher_func_name);

    }
}

#endif /* __STEKIN_OUTPUT_METHODS_H__ */
