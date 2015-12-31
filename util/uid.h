#ifndef __FLSC_UTILITY_UID_H__
#define __FLSC_UTILITY_UID_H__

#include <string>

#include "typetraits.h"

namespace util {

    struct uid {
        typedef tp::UInt<8>::type id_type;

        id_type const id;

        uid(uid const& rhs)
            : id(rhs.id)
        {}

        std::string str() const;

        bool operator==(uid const& rhs) const;
        bool operator!=(uid const& rhs) const;
    private:
        uid();
    public:
        static uid next_id()
        {
            return uid();
        }
    };

}

#endif /* __FLSC_UTILITY_UID_H__ */
