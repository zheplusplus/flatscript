#ifndef __STEKIN_MISCELLANY_POSITION_TYPE_H__
#define __STEKIN_MISCELLANY_POSITION_TYPE_H__

#include <string>
#include <list>

namespace misc {

    struct position {
        int line;

        explicit position(int ln)
            : line(ln)
        {}

        position()
            : line(0)
        {}

        std::string str() const;

        bool operator==(position const& rhs) const;

        position& operator=(position const& rhs);
    };

}

#endif /* __STEKIN_MISCELLANY_POSITION_TYPE_H__ */
