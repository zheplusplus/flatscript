#ifndef __FLSC_UTILITY_IO_H__
#define __FLSC_UTILITY_IO_H__

#include <string>
#include <stdexcept>

namespace util {

    class IOError
        : public std::runtime_error
    {
    public:
        int const errcode;

        explicit IOError(std::string what);
    };

    std::string read_file(std::string const& filename);
    std::string read_stdin();

}

#endif /* __FLSC_UTILITY_IO_H__ */
