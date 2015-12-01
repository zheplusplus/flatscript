#include <sstream>

#include <util/string.h>

#include "list-pipe.h"

using namespace output;

static std::string const ASYNC_MAPPER_CONTINUE("return $nx#PIPE_ID();");

void PipelineContinue::write(std::ostream& os) const
{
    os << util::replace_all(ASYNC_MAPPER_CONTINUE, "#PIPE_ID", this->pipe_id.str());
}

static std::string const ASYNC_PIPE(
"(function ($list) {"
    "if (!($list) || !Array.isArray($list)) #RAISE_EXC"
    "function $np#PIPE_ID($index, $result) {"
        "function $nx#PIPE_ID() { $np#PIPE_ID($index + 1, $result) }"
        "var $key = null;"
        "if ($index === $list.length) {"
            "#SUCCESSIVE_STATEMENTS"
        "} else {"
            "var $element = $list[$index];"
            "#NEXT"
        "}"
    "}"
    "$np#PIPE_ID(0, []);"
"})(#LIST)"
);

std::string AsyncPipeline::str() const
{
    std::ostringstream suc_os;
    succession->write(suc_os);
    std::ostringstream rec_os;
    recursion->write(rec_os);
    return
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
            ASYNC_PIPE
                , "#RAISE_EXC", thrower->scheme("'Require array for async iteration'"))
                , "#SUCCESSIVE_STATEMENTS", suc_os.str())
                , "#NEXT", rec_os.str())
                , "#LIST", list->str())
                , "#PIPE_ID", this->pipe_id.str())
        ;
}

static std::string const SYNC_PIPE(
    "($listpipe(#LIST, function($index, $key, $element, $result) {#SECTION}))");

std::string SyncPipeline::str() const
{
    std::ostringstream sec_os;
    section->write(sec_os);
    return
        util::replace_all(
        util::replace_all(
            SYNC_PIPE
                , "#SECTION", sec_os.str())
                , "#LIST", list->str())
        ;
}

static std::string const SYNC_PIPE_ROOT(
"{"
    "var $ind#PIPE_ID=0, $ret#PIPE_ID, $retf#PIPE_ID=false, $list=#LIST, $result=[];"
    "for (var $k in $list) {"
        "if ($list.hasOwnProperty($k)) {"
            "(function ($index, $key, $element) {"
                "#SECTION"
            "})($ind#PIPE_ID, $k, $list[$k]);"
            "if ($retf#PIPE_ID) {"
                "#EXT_RETURN"
            "}"
            "++$ind#PIPE_ID;"
        "}"
    "}"
"}"
);

std::string RootSyncPipeline::str() const
{
    std::ostringstream sec_os;
    section->write(sec_os);
    return
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
            SYNC_PIPE_ROOT
                , "#SECTION", sec_os.str())
                , "#LIST", this->list->str())
                , "#EXT_RETURN", this->ext_return->scheme("$ret" + this->pipe_id.str()))
                , "#PIPE_ID", this->pipe_id.str())
        ;
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
