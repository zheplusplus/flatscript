#include <algorithm>

#include <util/string.h>

#include "list-pipe.h"

using namespace proto;

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

std::string PipeMap::stringify(std::string const& list_repr) const
{
    return std::move(
        util::replace_all(
        util::replace_all(
            PIPE_MAP
                , "$VALUE", expr->stringify(true))
                , "$LIST", list_repr)
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

std::string PipeFilter::stringify(std::string const& list_repr) const
{
    return std::move(
        util::replace_all(
        util::replace_all(
            PIPE_FILTER
                , "$PREDICATE", expr->stringify(true))
                , "$LIST", list_repr)
        );
}

std::string ListPipeline::stringify(bool in_pipe) const
{
    std::string result(list->stringify(in_pipe));
    std::for_each(pipeline.begin()
                , pipeline.end()
                , [&](util::sptr<PipeBase const> const& p)
                  {
                      result = std::move(p->stringify(result));
                  });
    return std::move(result);
}
