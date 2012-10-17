#include <iostream>
#include <algorithm>

#include <util/string.h>

#include "func-writer.h"
#include "name-mangler.h"

using namespace output;

static std::string const FUNC_BEGIN(
    "function $FUNC_NAME($PARAMETERS) {\n"
);

static std::string const FUNC_END(
    "}\n"
);

static std::string formParams(std::vector<std::string> const& params)
{
    return std::accumulate(params.begin()
                         , params.end()
                         , std::string()
                         , [&](std::string const& acc, std::string const& param)
                           {
                               return acc + formVarName(true, param) + ",";
                           });
}

void output::writeFuncBegin(std::string const& name, std::vector<std::string> const& params)
{
    std::cout <<
        util::replace_all(
        util::replace_all(
            FUNC_BEGIN
                , "$FUNC_NAME", formVarName(true, name))
                , "$PARAMETERS", formParams(params) + "undefined")
    ;
}

void output::writeFuncEnd()
{
    std::cout << FUNC_END;
}

void output::writeArgsBegin()
{
    std::cout << "(";
}

void output::writeArgsEnd()
{
    std::cout << ")";
}

void output::writeArgSeparator()
{
    std::cout << ",";
}

static std::string const MAIN_BEGIN(
    "var lc_write = console.log;\n"
    "(function(window, undefined) {\n"
);

static std::string const MAIN_END(
    "}(this))\n"
);

void output::writeMainBegin()
{
    std::cout << MAIN_BEGIN;
}

void output::writeMainEnd()
{
    std::cout << MAIN_END;
}

static std::string const PIPE_MAP_BEGIN(
"(function (iterlist) {"
"    var r = [];\n"
"    for (var iterindex = 0; iterindex < iterlist.length; ++iterindex) {\n"
"        var iterelement = iterlist[iterindex];\n"
"        r.push(\n"
);

static std::string const PIPE_MAP_END(
"               );\n"
"    }\n"
"    return r;\n"
"}("
);

void output::pipeMapBegin()
{
    std::cout << PIPE_MAP_BEGIN;
}

void output::pipeMapEnd()
{
    std::cout << PIPE_MAP_END;
}

static std::string const PIPE_FILTER_BEGIN(
"(function (iterlist) {"
"    var r = [];\n"
"    for (var iterindex = 0; iterindex < iterlist.length; ++iterindex) {\n"
"        var iterelement = iterlist[iterindex];\n"
"        if (\n"
);

static std::string const PIPE_FILTER_END(
"            ) {\n"
"            r.push(iterelement);\n"
"        }\n"
"    }\n"
"    return r;\n"
"}("
);

void output::pipeFilterBegin()
{
    std::cout << PIPE_FILTER_BEGIN;
}

void output::pipeFilterEnd()
{
    std::cout << PIPE_FILTER_END;
}

static std::string const PIPE_FINISHED(
"))"
);

void output::pipeFinished()
{
    std::cout << PIPE_FINISHED;
}

void output::pipeElement()
{
    std::cout << "iterelement";
}

void output::pipeIndex()
{
    std::cout << "iterindex";
}
