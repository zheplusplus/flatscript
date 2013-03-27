#include <unistd.h>

#include "env.h"

#ifndef EOF
#define EOF (-1)
#endif

static std::set<std::string> pre_imported({
    "console", "setTimeout", "setInterval", "clearTimeout", "parseInt", "parseFloat", "Number",
    "Date", "Math", "Object", "Function", "escape", "unescape", "encodeURI", "encodeURIComponent",
    "decodeURI", "decodeURIComponent", "JSON", "NaN", "null", "undefined", "isFinite", "isNaN",
    "RegExp",
});

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
