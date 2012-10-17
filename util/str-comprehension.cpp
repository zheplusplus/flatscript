#include <cstring>
#include <map>
#include <algorithm>

#include <report/errors.h>

#include "str-comprehension.h"

namespace {

    template <typename K, typename V>
    void push(std::map<K, V>& map, K const& k, V const& v)
    {
        map.insert(std::make_pair(k, v));
    }

    struct escaper {
        escaper()
        {
            push(map, '\\', '\\');
            push(map, '"', '"');
            push(map, '\'', '\'');
            push(map, 't', '\t');
            push(map, 'n', '\n');
        }

        char get(char ch, int* index)
        {
            auto r = map.find(ch);
            if (r == map.end()) {
                return ch;
            }
            ++*index;
            return r->second;
        }
    private:
        std::map<char, char> map;
    };
    escaper esc;

}

std::string util::comprehend(char const* input, int begin, int end)
{
    size_t len = strlen(input);
    if (end < 0) {
        end = len + end;
    }
    std::string result;
    result.reserve(len);
    for (int i = begin; i < end; ) {
        if (input[i] == '\\') {
            ++i;
            result.push_back(esc.get(input[i], &i));
        } else {
            result.push_back(input[i]);
            ++i;
        }
    }
    return result;
}

namespace {

    struct encoder {
        encoder()
        {
            push(map, '\\', std::string("\\\\"));
            push(map, '"', std::string("\\\""));
            push(map, '\t', std::string("\\t"));
            push(map, '\n', std::string("\\n"));
        }

        std::string get(char ch)
        {
            auto r = map.find(ch);
            if (r == map.end()) {
                return std::string(1, ch);
            }
            return r->second;
        }
    private:
        std::map<char, std::string> map;
    };
    encoder enc;

}

std::string util::cstr_repr(char const* input, int length)
{
    std::string result;
    result.reserve(length * 2 + 2);
    result += '"';
    std::for_each(input
                , input + length
                , [&](char ch)
                  {
                      result += enc.get(ch);
                  });
    result += '"';
    return result;
}
