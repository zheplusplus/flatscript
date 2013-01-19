#include <sstream>

#include <util/string.h>

#include "list-pipe.h"

using namespace output;

std::string AsyncPipeResult::str() const
{
    return "$result";
}

static std::string const ASYNC_MAPPER_NEXT(
"return $next($index + 1, $result);\n"
);

static std::string const ASYNC_MAPPER_RESULT(
"$result.push(#EXPRESSION);\n"
+ ASYNC_MAPPER_NEXT
);

void PipelineResult::write(std::ostream& os) const
{
    os << util::replace_all(
            ASYNC_MAPPER_RESULT
                , "#EXPRESSION", expr->str());
}

void PipelineNext::write(std::ostream& os) const
{
    os << ASYNC_MAPPER_NEXT;
}

static std::string const ASYNC_PIPE(
"(function ($list) {\n"
"    if (!($list) || $list.length === undefined) throw 'not iterable';\n"
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
            ASYNC_PIPE
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
"    var $next = function() {};"
"    for (var $k in $list) {\n"
"        if ($ind === $list.length) {\n"
"            break;\n"
"        }\n"
"        (function ($index, $key, $element) {\n"
"           #SECTION;\n"
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
