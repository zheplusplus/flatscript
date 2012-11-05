#ifndef __STEKIN_OUTPUT_NAME_MANGLER_H__
#define __STEKIN_OUTPUT_NAME_MANGLER_H__

#include <string>

namespace output {

    std::string formName(std::string const& name);
    std::vector<std::string> formNames(std::vector<std::string> const& names);

}

#endif /* __STEKIN_OUTPUT_NAME_MANGLER_H__ */
