#ifndef __STEKIN_UTILITY_MAP_COMPARE_H__
#define __STEKIN_UTILITY_MAP_COMPARE_H__

#include <map>
#include <algorithm>

namespace util {

    template <typename _MapType>
    bool map_less(_MapType const& lhs, _MapType const& rhs)
    {
        if (lhs.size() != rhs.size()) {
            return lhs.size() < rhs.size();
        }
        auto i = lhs.begin();
        auto j = rhs.begin();
        for (; lhs.end() != i; ++i, ++j) {
            if (i->first != j->first) {
                return i->first < j->first;
            }
            if (i->second != j->second) {
                return i->second < j->second;
            }
        }
        return false;
    }

    struct map_less_t {
        template <typename _MapType>
        bool operator()(_MapType const& lhs, _MapType const& rhs) const
        {
            return map_less(lhs, rhs);
        }
    };

}

#endif /* __STEKIN_UTILITY_MAP_COMPARE_H__ */
