#ifndef __STEKIN_ENVIRONMENT_H__
#define __STEKIN_ENVIRONMENT_H__

#include <set>
#include <string>

namespace stekin {

    void initEnv(int argc, char* argv[]);
    std::set<std::string> const& preImported();

}

#endif /* __STEKIN_ENVIRONMENT_H__ */
