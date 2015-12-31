#ifndef __STEKIN_OUTPUT_NAME_MANGLER_H__
#define __STEKIN_OUTPUT_NAME_MANGLER_H__

#include <vector>
#include <string>

#include <util/uid.h>

namespace output {

    std::string formName(std::string const& name);
    std::vector<std::string> formNames(std::vector<std::string> const& names);

    std::string formClassName(std::string const& name);
    std::string formSubName(std::string const& name, util::uid space_id);

    std::string formTransientParam(std::string const& param);
    std::vector<std::string> formTransientParams(std::vector<std::string> const& params);

    std::string formAsyncRef(util::uid const& id);
    std::string formAnonymousFunc(util::uid const& id);

    extern std::string const TERM_REGULAR_ASYNC_CALLBACK;
    extern std::string const TERM_EXCEPTION;
    extern std::string const TERM_CONDITIONAL_CALLBACK_PARAMETER;

}

#endif /* __STEKIN_OUTPUT_NAME_MANGLER_H__ */
