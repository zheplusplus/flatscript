#include <algorithm>

#include <util/string.h>

#include "list-pipe.h"

using namespace output;

static std::string const PIPE_MAP(
"(function (iterlist) {"
"    var r = [];\n"
"    for (var iterindex = 0; iterindex < iterlist.length; ++iterindex) {\n"
"        var iterelement = iterlist[iterindex];\n"
"        r.push($VALUE);\n"
"    }\n"
"    return r;\n"
"})($LIST)"
);

std::string ListPipeMapper::str(bool in_pipe) const
{
    return std::move(
        util::replace_all(
        util::replace_all(
            PIPE_MAP
                , "$VALUE", mapper->str(true))
                , "$LIST", list->str(in_pipe))
        );
}

static std::string const PIPE_FILTER(
"(function (iterlist) {"
"    var r = [];\n"
"    for (var iterindex = 0; iterindex < iterlist.length; ++iterindex) {\n"
"        var iterelement = iterlist[iterindex];\n"
"        if ($PREDICATE) {\n"
"            r.push(iterelement);\n"
"        }\n"
"    }\n"
"    return r;\n"
"})($LIST)"
);

std::string ListPipeFilter::str(bool in_pipe) const
{
    return std::move(
        util::replace_all(
        util::replace_all(
            PIPE_FILTER
                , "$PREDICATE", filter->str(true))
                , "$LIST", list->str(in_pipe))
        );
}
