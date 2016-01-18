#include <cstdio>
#include <cerrno>
#include <cstring>

#include "io.h"

using namespace util;

static std::string error_message()
{
    char message[256];
    return std::string(strerror_r(errno, message, 256));
}

IOError::IOError(std::string what)
    : std::runtime_error(what + " " + ::error_message())
    , errcode(errno)
{}

namespace {

    class File {
        FILE* _file;
    public:
        File(FILE* f)
            : _file(f)
        {}

        File(File const&) = delete;

        ~File()
        {
            this->close();
        }

        std::string read()
        {
            return File::read(this->_file);
        }

        static std::string read(FILE* file)
        {
            std::string s;
            static int const SIZE = 4096;
            char buf[SIZE];
            while (true) {
                size_t r = ::fread(buf, 1, SIZE - 1, file);
                if (r > 0) {
                    buf[r] = 0;
                    s += buf;
                } else {
                    if (::feof(file)) {
                        return std::move(s);
                    }
                    throw IOError("Error on read");
                }
            }
        }

        bool closed() const
        {
            return this->_file == nullptr;
        }

        void close()
        {
            if (!this->closed()) {
                ::fclose(this->_file);
                this->_file = nullptr;
            }
        }
    };

}

std::string util::read_file(std::string const& filename)
{
    File f(::fopen(filename.data(), "r"));
    if (f.closed()) {
        throw IOError("Failed to open `" + filename + "':");
    }
    return f.read();
}

std::string util::read_stdin()
{
    return File::read(stdin);
}
