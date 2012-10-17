#include "platform.h"

using namespace platform;

std::string const& platform::c_long_long::type_name()
{
    static std::string const N("long long");
    return N;
}

std::string const& platform::c_long::type_name()
{
    static std::string const N("long");
    return N;
}

std::string const& platform::c_int::type_name()
{
    static std::string const N("int");
    return N;
}

std::string const& platform::c_short::type_name()
{
    static std::string const N("short");
    return N;
}

std::string const& platform::c_double::type_name()
{
    static std::string const N("double");
    return N;
}

std::string const& platform::c_char::type_name()
{
    static std::string const N("char");
    return N;
}
