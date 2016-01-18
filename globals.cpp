#include "globals.h"

using namespace flats;

Globals Globals::g;
std::string const Globals::version("0.6.1-2016-01-29-Chika");

static std::set<std::string> const RESERVED_WORDS({
    "break", "case", "catch", "continue", "debugger", "default", "delete", "do", "else",
    "finally", "for", "function", "if", "in", "instanceof", "new", "return", "switch", "this",
    "throw", "try", "typeof", "var", "void", "while", "with",
});

bool flats::isReserved(std::string const& name)
{
    return RESERVED_WORDS.find(name) != RESERVED_WORDS.end();
}
