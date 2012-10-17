#ifndef __STEKIN_UTILITY_SERIAL_NUMBER_H__
#define __STEKIN_UTILITY_SERIAL_NUMBER_H__

namespace util {

    struct serial_num {
        static serial_num next();
    public:
        int const n;

        serial_num(serial_num const& rhs)
            : n(rhs.n)
        {}
    private:
        explicit serial_num(int nn)
            : n(nn)
        {}
    };

}

#endif /* __STEKIN_UTILITY_SERIAL_NUMBER_H__ */
