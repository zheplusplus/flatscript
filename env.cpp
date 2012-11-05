#include <unistd.h>

#include "env.h"

static std::set<std::string> pre_imported;

void stekin::initEnv(int argc, char* argv[])
{
    int ch;
    opterr = 0;
    while ((ch = getopt(argc, argv, "i:")) != EOF) {
        if ('i' == ch) {
            pre_imported.insert(optarg);
        }
    }
}

std::set<std::string> const& stekin::preImported()
{
    return pre_imported;
}
