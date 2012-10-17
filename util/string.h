#ifndef __STEKIN_UTILITY_STRING_H__
#define __STEKIN_UTILITY_STRING_H__

#include <string>
#include <gmpxx.h>

namespace util {

    std::string replace_all(std::string src
                          , std::string const& origin_text
                          , std::string const& replacement);

    std::string str(int i);
    std::string str(long i);
    std::string str(long long i);
    std::string str(double d);
    std::string str(bool b);
    std::string str(void const* p);
    std::string str(mpz_class const& z);
    std::string str(mpf_class const& f);

}

#endif /* __STEKIN_UTILITY_STRING_H__ */
