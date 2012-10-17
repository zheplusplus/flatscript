#ifndef __STEKIN_OUTPUT_NAME_MANGLER_H__
#define __STEKIN_OUTPUT_NAME_MANGLER_H__

#include <string>
#include <vector>

#include <util/sn.h>

namespace output {

    std::string formVarName(bool local, std::string const& name);
    std::string formFuncName(std::string const& name, int param_count);

}

#endif /* __STEKIN_OUTPUT_NAME_MANGLER_H__ */
