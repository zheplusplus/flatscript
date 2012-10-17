struct _stk_type_void {
    _stk_type_void& operator=(_stk_type_void const&)
    {
        return *this;
    }
};

struct _stk_type_bool {
    _stk_type_1_byte boolean;

    _stk_type_bool& operator=(_stk_type_bool const& rhs)
    {
        boolean = rhs.boolean;
        return *this;
    }

    operator bool() const
    {
        return boolean;
    }

    _stk_type_bool(bool b)
        : boolean(b)
    {}

    _stk_type_bool()
        : boolean(false)
    {}
};

template <typename _T>
void push(void* mem, int offset, _T const& value)
{
    *(_T*)(offset + (_stk_type_1_byte*)(mem)) = value;
}

struct _stk_res_entry {
    virtual ~_stk_res_entry() {}
};

struct _stk_type_string
    : _stk_res_entry
{
    char* value;
    _stk_type_int length;

    _stk_type_string(_stk_type_string const& rhs)
        : value(new char[rhs.length + 1])
        , length(rhs.length)
    {
        std::copy(rhs.value, rhs.value + length, value);
        value[length] = 0;
    }

    _stk_type_string(_stk_type_string const& a, _stk_type_string const& b)
        : value(new char[a.length + b.length + 1])
        , length(a.length + b.length)
    {
        std::copy(a.value, a.value + a.length, value);
        std::copy(b.value, b.value + b.length, value + a.length);
        value[length] = 0;
    }

    _stk_type_string(char const* v, int len)
        : value(new char[len + 1])
        , length(len)
    {
        std::copy(v, v + length, value);
        value[length] = 0;
    }

    ~_stk_type_string()
    {
        delete[] value;
    }
};

void push(void* mem, int offset, _stk_type_string const& str)
{
    new(offset + (_stk_type_1_byte*)(mem))_stk_type_string(str);
}

_stk_type_string _stk_to_string(_stk_type_void)
{
    return _stk_type_string("", 0);
}

_stk_type_string _stk_to_string(_stk_type_bool v)
{
    return _stk_type_string(v ? "true" : "false", v ? 4 : 5);
}

_stk_type_string _stk_to_string(_stk_type_int v)
{
    char buffer[32];
    return _stk_type_string(buffer, sprintf(buffer, "%lld", v));
}

_stk_type_string _stk_to_string(_stk_type_float v)
{
    char buffer[32];
    return _stk_type_string(buffer, sprintf(buffer, "%lf", v));
}

_stk_type_string operator+(_stk_type_string const& a, _stk_type_string const& b)
{
    return _stk_type_string(a, b);
}

template <typename _T>
_stk_type_string operator+(_stk_type_string const& a, _T const& b)
{
    return _stk_type_string(a, _stk_to_string(b));
}

template <typename _T>
_stk_type_string operator+(_T const& a, _stk_type_string const& b)
{
    return _stk_type_string(_stk_to_string(a), b);
}

template <typename _MemberType>
struct _stk_list
    : _stk_res_entry
{
    _stk_type_int _size;
    _MemberType* _members;

    explicit _stk_list(int reserved)
        : _size(0)
        , _members(new _MemberType[reserved])
    {}

    _stk_list()
        : _size(0)
        , _members(NULL)
    {}

    _stk_list(_stk_list<_MemberType> const& rhs)
        : _size(0)
        , _members(NULL)
    {
        copy_members(rhs);
    }

    _stk_list const& operator=(_stk_list<_MemberType> const& rhs)
    {
        copy_members(rhs);
    }

    void copy_members(_stk_list<_MemberType> const& rhs)
    {
        _stk_list copy;
        copy._size = rhs._size;
        copy._members = new _MemberType[copy._size];
        for (_stk_type_int i = 0; i < rhs._size; ++i) {
            copy._members[i] = rhs._members[i];
        }

        delete[] _members;
        _size = copy._size;
        _members = copy._members;

        copy._size = 0;
        copy._members = NULL;
    }

    ~_stk_list()
    {
        delete[] _members;
    }

    _stk_type_bool empty() const
    {
        return _size == 0;
    }

    _stk_type_int size() const
    {
        return _size;
    }

    _MemberType first() const
    {
        return _members[0];
    }

    _stk_list push_back(_MemberType const& value) const
    {
        _stk_list result;
        result._size = _size + 1;
        result._members = new _MemberType[result._size];
        for (_stk_type_int i = 0; i < _size; ++i) {
            result._members[i] = _members[i];
        }
        result._members[_size] = value;
        return result;
    }
};

template <int _Size, typename _MemberType>
struct _stk_list_builder {
    mutable _stk_list<_MemberType> list;
    mutable _stk_type_int cursor;

    _stk_list_builder()
        : cursor(0)
    {
        list._size = _Size;
        list._members = new _MemberType[_Size];
    }

    _stk_list_builder const& push(_MemberType const& m) const
    {
        list._members[cursor++] = m;
        return *this;
    }

    _stk_list<_MemberType> build() const
    {
        return list;
    }
};

struct _stk_empty_list_type {
    template <typename _MemberType>
    operator _stk_list<_MemberType>()
    {
        return _stk_list<_MemberType>();
    }

    _stk_type_bool empty() const
    {
        return true;
    }

    _stk_type_int size() const
    {
        return 0;
    }

    template <typename _MemberType>
    _stk_list<_MemberType> push_back(_MemberType const& value) const
    {
        _stk_list<_MemberType> result;
        result._size = 1;
        result._members = new _MemberType[1];
        result._members[0] = value;
        return result;
    }
};

template <typename _T>
_stk_list<_T> _stk_list_append(_stk_list<_T> const& lhs, _stk_list<_T> const& rhs)
{
    _stk_list<_T> result;
    result._size = lhs._size + rhs._size;
    result._members = new _T[result._size];

    for (_stk_type_int i = 0; i < lhs._size; ++i) {
        result._members[i] = lhs._members[i];
    }
    for (_stk_type_int i = 0; i < rhs._size ; ++i) {
        result._members[lhs._size + i] = rhs._members[i];
    }

    return result;
}

template <typename _T>
_stk_list<_T> _stk_list_append(_stk_empty_list_type lhs, _stk_list<_T> const& rhs)
{
    return rhs;
}

template <typename _T>
_stk_list<_T> _stk_list_append(_stk_list<_T> const& lhs, _stk_empty_list_type rhs)
{
    return lhs;
}

_stk_empty_list_type _stk_list_append(_stk_empty_list_type lhs, _stk_empty_list_type rhs)
{
    return _stk_empty_list_type();
}

template <typename _MemberType>
void push(void* mem, int offset, _stk_list<_MemberType> const& list)
{
    new(offset + (_stk_type_1_byte*)(mem))_stk_list<_MemberType>(list);
}

template <int _Size>
struct _stk_res_entries {
    int entries[_Size];
    int cursor;

    _stk_res_entries()
        : cursor(0)
    {}

    _stk_res_entries(_stk_res_entries const& rhs)
        : cursor(rhs.cursor)
    {
        for (int i = 0; i < cursor; ++i) {
            entries[i] = rhs.entries[i];
        }
    }

    void add(int offset)
    {
        entries[cursor] = offset;
        ++cursor;
    }

    void dtor(void* mem)
    {
        for (int i = 0; i < cursor; ++i) {
            ((_stk_res_entry*)(entries[i] + (char*)(mem)))->~_stk_res_entry();
        }
    }
};

template <>
struct _stk_res_entries<0> {
    _stk_res_entries() {}
    _stk_res_entries(_stk_res_entries const&) {}
    void dtor(void*) {}
};

template <int _Size>
struct _stk_entries_builder {
    mutable _stk_res_entries<_Size> res_entries;
    mutable int cursor;

    _stk_entries_builder()
        : cursor(0)
    {}

    _stk_entries_builder const& push(int entry) const
    {
        res_entries.set(cursor++, entry);
        return *this;
    }

    _stk_res_entries<_Size> build() const
    {
        return res_entries;
    }
};

template <int _Size>
struct _stk_composite {
    _stk_type_1_byte mem[_Size];

    _stk_composite() {}

    _stk_composite(_stk_composite const& rhs)
    {
        operator=(rhs);
    }

    _stk_composite const& operator=(_stk_composite const& rhs)
    {
        std::copy(rhs.mem, rhs.mem + _Size, mem);
        return *this;
    }

    template <typename _T>
    _stk_composite push(int offset, _T const& value)
    {
        ::push(mem, offset, value);
        return *this;
    }

    ~_stk_composite() {}
};

template <>
struct _stk_composite<0> {
    _stk_composite() {}
    _stk_composite(_stk_composite const&) {}

    _stk_composite const& operator=(_stk_composite const&)
    {
        return *this;
    }
};

std::ostream& operator<<(std::ostream& os, _stk_type_bool const& b)
{
    return os << (0 == b.boolean ? "false" : "true");
}

std::ostream& operator<<(std::ostream& os, _stk_type_void)
{
    return os;
}

std::ostream& operator<<(std::ostream& os, _stk_type_string const& s)
{
    return os << s.value;
}

template <typename _MemberType>
std::ostream& operator<<(std::ostream& os, _stk_list<_MemberType> const& list)
{
    os << "[ ";
    for (_stk_type_int i = 0; i < list._size; ++i) {
        os << list._members[i] << ' ';
    }
    return os << ']';
}

std::ostream& operator<<(std::ostream& os, _stk_empty_list_type)
{
    return os << "[ ]";
}

template <int _Size>
std::ostream& operator<<(std::ostream& os, _stk_composite<_Size> const& composite)
{
    return os << "Stekin Composite (" << _Size << ')';
}

template <int _Level>
struct _stk_frame_bases {
    template <int _ExtLevel>
    _stk_frame_bases(_stk_frame_bases<_ExtLevel> const& ext_bases, void* this_base)
    {
        std::copy(ext_bases._stk_ext_bases, ext_bases._stk_ext_bases + _Level, _stk_ext_bases);
        _stk_ext_bases[_Level] = this_base;
    }

    _stk_frame_bases(_stk_frame_bases const& cp_bases)
    {
        std::copy(cp_bases._stk_ext_bases, cp_bases._stk_ext_bases + _Level + 1, _stk_ext_bases);
    }

    explicit _stk_frame_bases(void* this_base)
    {
        _stk_ext_bases[_Level] = this_base;
    }

    template <typename _T>
    void push(int offset, _T const& value)
    {
        ::push(_stk_ext_bases[_Level], offset, value);
    }

    void push(int, _stk_type_void) {}

    void* this_base() const
    {
        return _stk_ext_bases[_Level];
    }

    void* _stk_ext_bases[_Level + 1];
};
