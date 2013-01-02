#include <sstream>

#include <util/string.h>

#include "list-pipe.h"

using namespace output;

static std::string const PIPE_MAP(
"(function ($list) {\n"
"    if (!($list)) return;\n"
"    var $result = [];\n"
"    var $ind = 0;\n"
"    for (var $k in $list) {\n"
"        if ($ind === $list.length) {\n"
"            break;\n"
"        }\n"
"        $result.push((function ($index, $key, $element) {\n"
"           return #SECTION;\n"
"        })($ind, $k, $list[$k]));\n"
"        ++$ind;\n"
"    }\n"
"    return $result;\n"
"})(#LIST)"
);

static std::string const PIPE_FILTER(
"(function ($list) {\n"
"    if (!($list)) return;\n"
"    var $result = [];\n"
"    var $index = 0;\n"
"    for (var $key in $list) {\n"
"        if ($index === $list.length) {\n"
"            break;\n"
"        }\n"
"        var $element = $list[$key];\n"
"        if (#SECTION) {\n"
"            $result.push($element);\n"
"        }\n"
"        ++$index;\n"
"    }\n"
"    return $result;\n"
"})(#LIST)"
);

std::string Pipeline::str() const
{
    return std::move(
        util::replace_all(
        util::replace_all(
            cons::MAP == pipe_type ? PIPE_MAP : PIPE_FILTER
                , "#SECTION", section->str())
                , "#LIST", list->str())
        );
}

std::string AsyncPipeResult::str() const
{
    return "$result";
}

static std::string const ASYNC_MAPPER_BODY(
"$result.push(#EXPRESSION);\n"
"$next($index + 1, $result);\n"
);

static std::string const ASYNC_FILTER_BODY(
"if (#EXPRESSION) $result.push($element);\n"
"$next($index + 1, $result);\n"
);

void AsyncPipeBody::write(std::ostream& os) const
{
    os << util::replace_all(
            cons::MAP == pipe_type ? ASYNC_MAPPER_BODY : ASYNC_FILTER_BODY
                , "#EXPRESSION", expr->str());
}

static std::string const ASYNC_PIPE(
"(function ($list) {\n"
"    if (!($list)) throw 'not iterable';\n"
"    function $next($index, $result) {\n"
"        if ($index === $list.length) {\n"
"            #SUCCESSIVE_STATEMENTS\n"
"        } else {\n"
"            var $element = $list[$index];\n"
"            #NEXT\n"
"        }\n"
"    }\n"
"    $next(0, []);\n"
"})(#LIST);\n"
);

std::string AsyncPipe::str() const
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
