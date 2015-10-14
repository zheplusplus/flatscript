#ifndef __STEKIN_GLOBALS_H__
#define __STEKIN_GLOBALS_H__

#include <set>
#include <string>

namespace stekin {

    class Globals {
        Globals()
            : use_class_ext(false)
            , use_list_slice(false)
        {}
    public:
        static Globals g;
    public:
        std::set<std::string> pre_imported;
        bool use_class_ext;
        bool use_list_slice;
    };

}

#endif /* __STEKIN_GLOBALS_H__ */