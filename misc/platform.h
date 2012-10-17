#ifndef __STEKIN_MISCELLANY_PLATFORM_H__
#define __STEKIN_MISCELLANY_PLATFORM_H__

#include <string>

namespace platform {

    template <typename _EntryTraits, int _Size>
    struct type_find {
        template <bool _SizeMatched, typename _Traits, int _Sz> struct type_find_branch;

        template <typename _Traits, int _Sz>
        struct type_find_branch<true, _Traits, _Sz> {
            typedef _Traits traits;
        };

        template <typename _Traits, int _Sz>
        struct type_find_branch<false, _Traits, _Sz> {
            typedef typename type_find<typename _EntryTraits::candidate_traits, _Sz>::traits traits;
        };

        typedef typename type_find_branch<_Size == sizeof(typename _EntryTraits::type)
                                        , _EntryTraits
                                        , _Size>::traits traits;
        typedef typename traits::type type;
    };

    struct c_long_long {
        static std::string const& type_name();
        typedef long long type;
    };

    struct c_long {
        static std::string const& type_name();
        typedef long type;
        typedef c_long_long candidate_traits;
    };

    struct c_int {
        static std::string const& type_name();
        typedef int type;
        typedef c_long candidate_traits;
    };

    struct c_short {
        static std::string const& type_name();
        typedef short type;
        typedef c_int candidate_traits;
    };

    struct c_double {
        static std::string const& type_name();
        typedef double type;
    };

    struct c_char {
        static std::string const& type_name();
        typedef char type;
    };

    class vft {
        virtual ~vft() = 0;
    };

    int const WORD_LENGTH_INBYTE = sizeof(void*);
    int const BOOL_SIZE = 1;
    int const INT_SIZE = 8;
    int const FLOAT_SIZE = 8;
    int const VIRTUAL_FUNC_TABLE_SIZE = sizeof(vft);

    typedef type_find<c_short, INT_SIZE>::traits int_traits;
    typedef type_find<c_double, FLOAT_SIZE>::traits float_traits;
    typedef type_find<c_char, BOOL_SIZE>::traits bool_traits;

    typedef type_find<c_short, INT_SIZE>::type int_type;
    typedef type_find<c_double, FLOAT_SIZE>::type float_type;
    typedef type_find<c_char, BOOL_SIZE>::type bool_type;

}

#endif /* __STEKIN_MISCELLANY_PLATFORM_H__ */
