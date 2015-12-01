#ifndef __STEKIN_GLOBALS_H__
#define __STEKIN_GLOBALS_H__

#include <set>
#include <string>

namespace flats {

    class Globals {
        Globals()
            : use_class_ext(false)
            , use_list_slice(false)
            , use_list_pipe(false)
            , use_export(false)
        {}
    public:
        static std::string const version;
        static Globals g;
    public:
        std::set<std::string> external_syms;
        std::string input_file;
        bool use_class_ext;
        bool use_list_slice;
        bool use_list_pipe;
        bool use_export;
    };

}

#endif /* __STEKIN_GLOBALS_H__ */
