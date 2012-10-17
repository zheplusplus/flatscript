#ifndef __STEKIN_UTILITY_STRING_COMPREHENSION_H__
#define __STEKIN_UTILITY_STRING_COMPREHENSION_H__

#include <string>

namespace util {

    std::string comprehend(char const* input, int begin, int end);
    std::string cstr_repr(char const* input, int length);

}

#endif /* __STEKIN_UTILITY_STRING_COMPREHENSION_H__ */
