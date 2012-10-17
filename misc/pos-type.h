#ifndef __STEKIN_MISCELLANY_POSITION_TYPE_H__
#define __STEKIN_MISCELLANY_POSITION_TYPE_H__

#include <string>
#include <list>

namespace misc {

    struct position {
        int const line;

        explicit position(int l)
            : line(l)
        {}

        position()
            : line(0)
        {}

        std::string str() const;

        bool operator==(position const& rhs) const;
    };

    struct trace {
        trace& add(position const& pos);
        position top() const;
        std::string str(std::string const& message) const;

        bool operator==(trace const& rhs) const;
    private:
        std::list<position> _trace;
    };

}

#endif /* __STEKIN_MISCELLANY_POSITION_TYPE_H__ */
