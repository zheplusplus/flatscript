#include <unistd.h>

#include "env.h"
#include "globals.h"

#ifndef EOF
#define EOF (-1)
#endif

static std::set<std::string> const pre_imported({
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
            Globals::g.pre_imported.insert(optarg);
        }
    }
    for (std::string const& i: pre_imported) {
        Globals::g.pre_imported.insert(i);
    }
}
