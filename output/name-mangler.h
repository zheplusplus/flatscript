#ifndef __STEKIN_OUTPUT_NAME_MANGLER_H__
#define __STEKIN_OUTPUT_NAME_MANGLER_H__

#include <vector>
#include <string>

#include <util/pointer.h>

namespace output {

    std::string formName(std::string const& name);
    std::vector<std::string> formNames(std::vector<std::string> const& names);

    std::string formTransientParam(std::string const& param);
    std::vector<std::string> formTransientParams(std::vector<std::string> const& params);

    std::string formAsyncRef(util::id const& id);
    std::string formAnonymousFunc(util::id const& id);

    namespace term {

        std::string const& regularAsyncCallback();

    }

}

#endif /* __STEKIN_OUTPUT_NAME_MANGLER_H__ */
