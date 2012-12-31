#include <algorithm>

#include <util/string.h>

#include "list-pipe.h"

using namespace output;

static std::string const PIPE_MAP(
"(function ($list) {\n"
"    var r = [];\n"
"    var $ind = 0;\n"
"    for (var $k in $list) {\n"
"        if ($ind === $list.length) {\n"
"            break;\n"
"        }\n"
"        r.push((function ($index, $key, $element) {\n"
"           return #VALUE;\n"
"        })($ind, $k, $list[$k]));\n"
"        ++$ind;\n"
"    }\n"
"    return r;\n"
"})(#LIST)"
);

std::string ListPipeMapper::str() const
{
    return std::move(
        util::replace_all(
        util::replace_all(
            PIPE_MAP
                , "#VALUE", mapper->str())
                , "#LIST", list->str())
        );
}

static std::string const PIPE_FILTER(
"(function ($list) {\n"
"    var r = [];\n"
"    var $index = 0;\n"
"    for (var $key in $list) {\n"
"        if ($index === $list.length) {\n"
"            break;\n"
"        }\n"
"        var $element = $list[$key];\n"
"        if (#PREDICATE) {\n"
"            r.push($element);\n"
"        }\n"
"        ++$index;\n"
"    }\n"
"    return r;\n"
"})(#LIST)"
);

std::string ListPipeFilter::str() const
{
    return std::move(
        util::replace_all(
        util::replace_all(
            PIPE_FILTER
                , "#PREDICATE", filter->str())
                , "#LIST", list->str())
        );
}
