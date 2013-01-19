#ifndef __STEKIN_UTILITY_ARRAYS_H__
#define __STEKIN_UTILITY_ARRAYS_H__

#include <vector>

#include "pointer.h"

namespace util {

    template <typename RightRefType, template <class> class FinalType>
    struct rrefarr {
        typedef RightRefType value_type;
        typedef std::vector<value_type> storage_type;

        rrefarr() = default;
        ~rrefarr() = default;
        rrefarr(rrefarr const&) = delete;

        rrefarr(rrefarr&& rhs)
            : _array(std::move(rhs._array))
        {}

        template <typename ConvertableType>
        rrefarr(rrefarr<ConvertableType, FinalType>&& rhs)
        {
            this->append(std::move(rhs));
        }

        explicit rrefarr(int init_size)
        {
            _array.reserve(init_size);
        }

        rrefarr(storage_type&& s)
            : _array(std::move(s))
        {}

        template <class F>
        auto map(F f) const -> FinalType<typename decltype(f(value_type(nullptr), 0))::value_type>
        {
            FinalType<typename decltype(f(value_type(nullptr), 0))::value_type> result(size());
            for (int i = 0; i < int(_array.size()); ++i) {
                result.append(f(_array[i], i));
            }
            return std::move(result);
        }

        template <class F>
        auto mapv(F f) const -> std::vector<decltype(f(value_type(nullptr), 0))>
        {
            std::vector<decltype(f(value_type(nullptr), 0))> result;
            for (int i = 0; i < int(_array.size()); ++i) {
                result.push_back(f(_array[i], i));
            }
            return std::move(result);
        }

        template <class F>
        bool any(F f) const
        {
            for (int i = 0; i < int(_array.size()); ++i) {
                if (f(_array[i], i)) {
                    return true;
                }
            }
            return false;
        }

        template <class F>
        void iter(F f) const
        {
            for (int i = 0; i < int(_array.size()); ++i) {
                f(_array[i], i);
            }
        }

        void append(value_type v)
        {
            _array.push_back(std::move(v));
        }

        template <typename ConvertableType>
        void append(rrefarr<ConvertableType, FinalType>&& following)
        {
            _array.reserve(size() + following.size());
            for (auto i = following.begin(); i != following.end(); ++i) {
                _array.push_back(std::move(*i));
            }
            following.clear();
        }

        value_type const& operator[](int index) const
        {
            return _array[index];
        }

        value_type const& back() const
        {
            return _array.back();
        }

        int size() const
        {
            return int(_array.size());
        }

        bool empty() const
        {
            return _array.empty();
        }

        void clear()
        {
            _array.clear();
        }
    private:
        storage_type _array;
    public:
        typedef typename storage_type::const_iterator const_iterator;
        typedef typename storage_type::iterator iterator;

        const_iterator begin() const
        {
            return _array.begin();
        }

        const_iterator end() const
        {
            return _array.end();
        }

        iterator begin()
        {
            return _array.begin();
        }

        iterator end()
        {
            return _array.end();
        }
    };

    template <typename T>
    struct ptrarr
        : rrefarr<sptr<T>, ptrarr>
    {
        typedef rrefarr<sptr<T>, util::ptrarr> base_type;
        typedef typename base_type::value_type value_type;

        ptrarr() = default;
        ptrarr(ptrarr const&) = delete;

        ptrarr(ptrarr&& rhs)
            : base_type(std::move(rhs))
        {}

        template <typename ConvertableType>
        ptrarr(util::ptrarr<ConvertableType>&& rhs)
            : base_type(std::move(rhs))
        {}

        explicit ptrarr(int init_size)
            : base_type(init_size)
        {}

        ptrarr(typename base_type::storage_type&& s)
            : base_type(std::move(s))
        {}

        ptrarr& append(value_type v)
        {
            base_type::append(std::move(v));
            return *this;
        }

        template <typename ConvertableType>
        ptrarr& append(rrefarr<ConvertableType, util::ptrarr>&& following)
        {
            base_type::append(std::move(following));
            return *this;
        }

        ptrarr deliverRange(int begin, int end)
        {
            ptrarr result(end - begin);
            for (; begin < end; ++begin) {
                result.append(std::move(*(base_type::begin() + begin)));
            }
            return std::move(result);
        }
    };

    template <typename T>
    struct ptrkv {
        typedef sptr<T> storage_type;
        typedef typename storage_type::value_type value_type;

        ptrkv(sptr<T>&& k, sptr<T>&& v)
            : key(std::move(k))
            , value(std::move(v))
        {}

        ptrkv(ptrkv&& rhs)
            : key(std::move(rhs.key))
            , value(std::move(rhs.value))
        {}

        ptrkv(std::nullptr_t)
            : key(nullptr)
            , value(nullptr)
        {}

        ptrkv(ptrkv const&) = delete;

        storage_type key;
        storage_type value;
    };

    template <typename T>
    ptrkv<T> mkkv(sptr<T> k, sptr<T> v)
    {
        return ptrkv<T>(std::move(k), std::move(v));
    }

    template <typename T>
    struct ptrkvarr
        : rrefarr<ptrkv<T>, ptrkvarr>
    {
        typedef rrefarr<ptrkv<T>, util::ptrkvarr> base_type;
        typedef typename base_type::value_type kv_type;

        ptrkvarr() = default;
        ptrkvarr(ptrkvarr const&) = delete;

        ptrkvarr(ptrkvarr&& rhs)
            : base_type(std::move(rhs))
        {}

        template <typename ConvertableType>
        ptrkvarr(util::ptrkvarr<ConvertableType>&& rhs)
            : base_type(std::move(rhs))
        {}

        explicit ptrkvarr(int init_size)
            : base_type(init_size)
        {}

        ptrkvarr& append(sptr<T> k, sptr<T> v)
        {
            base_type::append(mkkv(std::move(k), std::move(v)));
            return *this;
        }

        ptrkvarr& append(ptrkv<T> p)
        {
            base_type::append(std::move(p));
            return *this;
        }
    };

}

#endif /* __STEKIN_UTILITY_ARRAYS_H__ */
