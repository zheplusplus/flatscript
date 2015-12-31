#include <sstream>

#include <util/string.h>

#include "list-pipe.h"

using namespace output;

static std::string const ASYNC_PIPE(
"(function ($list) {"
    "if (!Array.isArray($list) && $list.constructor !== String) #RAISE_EXC"
    "var $pke#PIPE_ID = null;"
    "function $brk#PIPE_ID() { $np#PIPE_ID($list.length) }"
    "function $np#PIPE_ID($pix#PIPE_ID, $prs#PIPE_ID) {"
        "function $nx#PIPE_ID() { $np#PIPE_ID($pix#PIPE_ID + 1, $prs#PIPE_ID) }"
        "if ($pix#PIPE_ID === $list.length) {"
            "#SUCCESSIVE_STATEMENTS"
        "} else {"
            "var $pel#PIPE_ID = $list[$pix#PIPE_ID];"
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
    "($listpipe(#LIST, function($pix#PIPE_ID, $pke#PIPE_ID,"
                               "$pel#PIPE_ID, $prs#PIPE_ID)"
                      "{#SECTION}))");

std::string SyncPipeline::str() const
{
    std::ostringstream sec_os;
    section->write(sec_os);
    return
        util::replace_all(
        util::replace_all(
        util::replace_all(
            SYNC_PIPE
                , "#SECTION", sec_os.str())
                , "#LIST", this->list->str())
                , "#PIPE_ID", this->pipe_id.str())
        ;
}

static std::string const ROOT_SYNC_PIPE_TEMPLATE(
"{"
    "var $pix#PIPE_ID=0, $brk#PIPE_ID=false, $ret#PIPE_ID, $retf#PIPE_ID=false,"
        "$list=#LIST, $prs#PIPE_ID=[], $nx#PIPE_ID;"
    "for (var $k in $list) {"
        "if ($list.hasOwnProperty($k)) {"
            "(function ($pke#PIPE_ID, $pel#PIPE_ID) {"
                "#SECTION"
            "})($k, $list[$k]);"
            "##RETURN"
            "##BREAK"
            "++$pix#PIPE_ID;"
        "}"
    "}"
"}"
);

static std::string const SYNC_PIPE_TEMPLATE_RET("if ($retf#PIPE_ID) {#EXT_RETURN}");
static std::string const SYNC_PIPE_TEMPLATE_BREAK("if ($brk#PIPE_ID) {break}");

static std::string iteration_template(std::string const& templ, bool has_ret, bool has_break)
{
    return
        util::replace_all(
        util::replace_all(
            templ
                , "##RETURN", has_ret ? SYNC_PIPE_TEMPLATE_RET : "")
                , "##BREAK", has_break ? SYNC_PIPE_TEMPLATE_BREAK : "")
        ;
}

std::string RootSyncPipeline::str() const
{
    std::ostringstream sec_os;
    section->write(sec_os);
    return
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
            ::iteration_template(ROOT_SYNC_PIPE_TEMPLATE, this->has_ret, this->has_break)
                , "#SECTION", sec_os.str())
                , "#LIST", this->list->str())
                , "#EXT_RETURN", this->ext_return->scheme("$ret" + this->pipe_id.str()))
                , "#PIPE_ID", this->pipe_id.str())
        ;
}

static std::string const SYNC_RANGE_ITERATION_TEMPLATE(
"{"
    "var $idx#PIPE_ID, $brk#PIPE_ID=false, $ret#PIPE_ID, $retf#PIPE_ID=false,"
        "$nx#PIPE_ID;"
    "for ($idx#PIPE_ID = #BEGIN; $idx#PIPE_ID #CMP #END; $idx#PIPE_ID += #STEP) {"
        "(function (#REFERENCE) {"
            "#SECTION"
        "})($idx#PIPE_ID);"
        "##RETURN"
        "##BREAK"
    "}"
"}"
);

void SyncRangeIteration::write(std::ostream& os) const
{
    std::ostringstream sec_os;
    this->section->write(sec_os);
    os <<
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
            ::iteration_template(SYNC_RANGE_ITERATION_TEMPLATE, this->has_ret, this->has_break)
                , "#REFERENCE", this->reference->str())
                , "#BEGIN", this->begin->str())
                , "#END", this->end->str())
                , "#STEP", util::str(this->step))
                , "#CMP", this->step > 0 ? "<" : ">")
                , "#SECTION", sec_os.str())
                , "#EXT_RETURN", this->ext_return->scheme("$ret" + this->iter_id.str()))
                , "#PIPE_ID", this->iter_id.str())
        ;
}

static std::string const ASYNC_RANGE_ITERATION_TEMPLATE(
"{"
    "function $brk#PIPE_ID() { $np#PIPE_ID(#END) }"
    "function $np#PIPE_ID(#REFERENCE) {"
        "function $nx#PIPE_ID() { $np#PIPE_ID(#REFERENCE + #STEP) }"
        "if (#REFERENCE #CMP #END) {"
            "#LOOP"
        "} else {"
            "#SUCCESSIVE_STATEMENTS"
        "}"
    "}"
    "$np#PIPE_ID(#BEGIN);"
"}"
);

void AsyncRangeIteration::write(std::ostream& os) const
{
    std::ostringstream sec_os;
    this->section->write(sec_os);
    std::ostringstream suc_os;
    this->succession->write(suc_os);
    os <<
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
            ::ASYNC_RANGE_ITERATION_TEMPLATE
                , "#REFERENCE", this->reference->str())
                , "#BEGIN", this->begin->str())
                , "#END", this->end->str())
                , "#STEP", util::str(this->step))
                , "#CMP", this->step > 0 ? "<" : ">")
                , "#LOOP", sec_os.str())
                , "#SUCCESSIVE_STATEMENTS", suc_os.str())
                , "#PIPE_ID", this->iter_id.str())
        ;
}

std::string PipeElement::str() const
{
    return "$pel" + this->pipe_id.str();
}

std::string PipeIndex::str() const
{
    return "$pix" + this->pipe_id.str();
}

std::string PipeKey::str() const
{
    return "$pke" + this->pipe_id.str();
}

std::string PipeResult::str() const
{
    return "$prs" + this->pipe_id.str();
}

std::string PipeBreak::str() const
{
    return "$brk" + this->pipe_id.str();
}

std::string PipeContinue::str() const
{
    return "$nx" + this->pipe_id.str();
}
