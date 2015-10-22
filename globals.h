#ifndef __STEKIN_GLOBALS_H__
#define __STEKIN_GLOBALS_H__

#include <set>
#include <string>

namespace flats {

    class Globals {
        Globals()
            : use_class_ext(false)
            , use_list_slice(false)
        {}
    public:
        static Globals g;
    public:
        std::set<std::string> external_syms;
        bool use_class_ext;
        bool use_list_slice;
    };

}

#endif /* __STEKIN_GLOBALS_H__ */
