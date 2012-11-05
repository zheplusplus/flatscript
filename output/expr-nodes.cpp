#include <algorithm>
#include <sstream>
#include <map>

#include <report/errors.h>
#include <util/string.h>
#include <util/str-comprehension.h>

#include "expr-nodes.h"
#include "name-mangler.h"
#include "function.h"
#include "block.h"

using namespace output;

template <typename T>
static std::string strPrimitive(T const& t)
{
    return "(" + util::str(t) + ")";
}

std::string BoolLiteral::str(bool) const
{
    return strPrimitive(value);
}

std::string IntLiteral::str(bool) const
{
    return strPrimitive(value);
}

std::string FloatLiteral::str(bool) const
{
    return strPrimitive(value);
}

std::string StringLiteral::str(bool) const
{
    return util::cstr_repr(value.c_str(), value.size());
}

static std::vector<std::string> strList(
                    std::vector<util::sptr<Expression const>> const& list, bool in_pipe)
{
    std::vector<std::string> result;
    std::for_each(list.begin()
                , list.end()
                , [&](util::sptr<Expression const> const& expr)
                  {
                      result.push_back(expr->str(in_pipe));
                  });
    return std::move(result);
}

std::string ListLiteral::str(bool in_pipe) const
{
    return "[" + util::join(",", strList(value, in_pipe)) + "]";
}

std::string ListElement::str(bool in_pipe) const
{
    if (!in_pipe) {
        error::pipeReferenceNotInListContext(pos);
    }
    return "iterelement";
}

std::string ListIndex::str(bool in_pipe) const
{
    if (!in_pipe) {
        error::pipeReferenceNotInListContext(pos);
    }
    return "iterindex";
}

std::string Reference::str(bool) const
{
    return formName(name);
}

std::string ImportedName::str(bool) const
{
    return name;
}

std::string Call::str(bool in_pipe) const
{
    return callee->str(in_pipe) + "(" + util::join(",", strList(args, in_pipe)) + ")";
}

std::string MemberAccess::str(bool in_pipe) const
{
    return referee->str(in_pipe) + "." + member;
}

std::string Lookup::str(bool in_pipe) const
{
    return collection->str(in_pipe) + "[" + key->str(in_pipe) + "]";
}

static std::string const LIST_SLICE(
"(function(list, begin, end, step) {\n"
"    function round(x) {\n"
"        if (x > list.length) return list.length;\n"
"        if (x < 0) return x % list.length + list.length;\n"
"        return x;\n"
"    }\n"
"    var r = [];\n"
"    step = step || 1;\n"
"    if (step > 0) {\n"
"        begin = round(begin || 0);\n"
"        end = (end === null) ? list.length : round(end);\n"
"        for (; begin < end; begin += step) {\n"
"            r.push(list[begin]);\n"
"        }\n"
"        return r;\n"
"    }\n"
"    begin = (begin === null) ? list.length - 1 : round(begin);\n"
"    end = (end === null) ? -1 : round(end)\n"
"    for (; begin > end; begin += step) {\n"
"        r.push(list[begin]);\n"
"    }\n"
"    return r;\n"
"})($LIST, $BEGIN, $END, $STEP)\n"
);

std::string ListSlice::str(bool in_pipe) const
{
    return
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
            LIST_SLICE
                , "$LIST", list->str(in_pipe))
                , "$BEGIN", begin->str(in_pipe))
                , "$END", end->str(in_pipe))
                , "$STEP", step->str(in_pipe))
        ;
}

std::string ListSlice::Default::str(bool) const
{
    return "null";
}

std::string Dictionary::str(bool in_pipe) const
{
    std::vector<std::string> item_strings;
    std::for_each(items.begin()
                , items.end()
                , [&](ItemType const& item)
                  {
                      item_strings.push_back(
                          item.first->str(in_pipe) + ":" + item.second->str(in_pipe));
                  });
    return "({" + util::join(",", item_strings) + "})";
}

std::string ListAppend::str(bool in_pipe) const
{
    return lhs->str(in_pipe) + ".concat(" + rhs->str(in_pipe) + ")";
}

static std::map<std::string, std::string> genOpMap()
{
    std::map<std::string, std::string> m;
    m.insert(std::make_pair("=", "==="));
    m.insert(std::make_pair("!=", "!=="));
    return m;
}

static std::map<std::string, std::string> const OP_MAP(genOpMap());

static std::string strOperator(std::string const& op_img)
{
    auto r = OP_MAP.find(op_img);
    if (OP_MAP.end() == r) {
        return op_img;
    }
    return r->second;
}

static std::string strBinary(util::sptr<Expression const> const& lhs
                           , std::string const& op
                           , util::sptr<Expression const> const& rhs
                           , bool in_pipe)
{
    return "(" + lhs->str(in_pipe) + strOperator(op) + rhs->str(in_pipe) + ")";
}

static std::string strPreUnary(std::string const& op
                             , util::sptr<Expression const> const& rhs
                             , bool in_pipe)
{
    return "(" + strOperator(op) + rhs->str(in_pipe) + ")";
}

std::string BinaryOp::str(bool in_pipe) const
{
    return strBinary(lhs, op, rhs, in_pipe);
}

std::string PreUnaryOp::str(bool in_pipe) const
{
    return strPreUnary(op, rhs, in_pipe);
}

std::string Lambda::str(bool) const
{
    std::ostringstream body_os;
    body->write(body_os);
    return
        util::replace_all(
        util::replace_all(
            "function($PARAMETERS) { $BODY }"
                , "$PARAMETERS", util::join(",", formNames(param_names)))
                , "$BODY", body_os.str())
        ;
}
