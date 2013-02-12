#include <sstream>

#include <util/string.h>

#include "list-pipe.h"

using namespace output;

static std::string const ASYNC_MAPPER_CONTINUE(
"return $next($index + 1, $result);\n"
);

void PipelineContinue::write(std::ostream& os) const
{
    os << ASYNC_MAPPER_CONTINUE;
}

static std::string const ASYNC_PIPE(
"(function ($list) {\n"
"    if (!($list) || $list.length === undefined) #RAISE_EXC\n"
"    function $next($index, $result) {\n"
"        var $key = null;\n"
"        if ($index === $list.length) {\n"
"            #SUCCESSIVE_STATEMENTS\n"
"        } else {\n"
"            var $element = $list[$index];\n"
"            #NEXT\n"
"        }\n"
"    }\n"
"    $next(0, []);\n"
"})(#LIST)\n"
);

std::string AsyncPipeline::str() const
{
    std::ostringstream suc_os;
    succession->write(suc_os);
    std::ostringstream rec_os;
    recursion->write(rec_os);
    return std::move(
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
            ASYNC_PIPE
                , "#RAISE_EXC", raiser("'not iterable'"))
                , "#SUCCESSIVE_STATEMENTS", suc_os.str())
                , "#NEXT", rec_os.str())
                , "#LIST", list->str())
        );
}

static std::string const SYNC_PIPE(
"(function ($list) {\n"
"    if (!($list)) return;\n"
"    var $result = [];\n"
"    var $ind = 0;\n"
"    var $next = function() {};\n"
"    for (var $k in $list) {\n"
"        if ($ind === $list.length) {\n"
"            break;\n"
"        }\n"
"        (function ($index, $key, $element) {\n"
"           #SECTION\n"
"        })($ind, $k, $list[$k]);\n"
"        ++$ind;\n"
"    }\n"
"    return $result;\n"
"})(#LIST)"
);

std::string SyncPipeline::str() const
{
    std::ostringstream sec_os;
    section->write(sec_os);
    return std::move(
        util::replace_all(
        util::replace_all(
            SYNC_PIPE
                , "#SECTION", sec_os.str())
                , "#LIST", list->str())
        );
}

std::string PipeElement::str() const
{
    return "$element";
}

std::string PipeIndex::str() const
{
    return "$index";
}

std::string PipeKey::str() const
{
    return "$key";
}

std::string PipeResult::str() const
{
    return "$result";
}
