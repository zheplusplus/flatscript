#ifndef __STEKIN_MISCELLANY_POSITION_TYPE_H__
#define __STEKIN_MISCELLANY_POSITION_TYPE_H__

#include <string>

namespace misc {

    struct position {
        std::string file;
        int line;

        position(position const&) = default;

        position(position&& rhs)
            : file(std::move(rhs.file))
            , line(rhs.line)
        {}

        explicit position(int ln)
            : line(ln)
        {}

        position(std::string f, int ln)
            : file(std::move(f))
            , line(ln)
        {}

        position()
            : line(0)
        {}

        std::string str() const;
        std::string as_line() const;
        bool operator==(position const& rhs) const;
        position& operator=(position const& rhs);
    };

}

#endif /* __STEKIN_MISCELLANY_POSITION_TYPE_H__ */
