#include <algorithm>
#include <map>

#include <report/errors.h>
#include <util/string.h>
#include <util/str-comprehension.h>

#include "expr-nodes.h"
#include "name-mangler.h"
#include "function.h"
#include "block.h"

using namespace proto;

template <typename T>
static std::string stringifyPrimitive(T const& t)
{
    return "(" + util::str(t) + ")";
}

std::string BoolLiteral::stringify(bool) const
{
    return stringifyPrimitive(value);
}

std::string IntLiteral::stringify(bool) const
{
    return stringifyPrimitive(value);
}

std::string FloatLiteral::stringify(bool) const
{
    return stringifyPrimitive(value);
}

std::string StringLiteral::stringify(bool) const
{
    return util::cstr_repr(value.c_str(), value.size());
}

static std::vector<std::string> stringifyList(std::vector<util::sptr<Expression const>> const& list
                                            , bool in_pipe)
{
    std::vector<std::string> result;
    std::for_each(list.begin()
                , list.end()
                , [&](util::sptr<Expression const> const& expr)
                  {
                      result.push_back(expr->stringify(in_pipe));
                  });
    return std::move(result);
}

std::string ListLiteral::stringify(bool in_pipe) const
{
    return "[" + util::join(",", stringifyList(value, in_pipe)) + "]";
}

std::string ListElement::stringify(bool in_pipe) const
{
    if (!in_pipe) {
        error::pipeReferenceNotInListContext(pos);
    }
    return "iterelement";
}

std::string ListIndex::stringify(bool in_pipe) const
{
    if (!in_pipe) {
        error::pipeReferenceNotInListContext(pos);
    }
    return "iterindex";
}

std::string Reference::stringify(bool) const
{
    return formName(name);
}

std::string Call::stringify(bool in_pipe) const
{
    return callee->stringify(in_pipe) + "(" + util::join(",", stringifyList(args, in_pipe)) + ")";
}

std::string MemberAccess::stringify(bool in_pipe) const
{
    return referee->stringify(in_pipe) + "." + member;
}

std::string Lookup::stringify(bool in_pipe) const
{
    return collection->stringify(in_pipe) + "[" + key->stringify(in_pipe) + "]";
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

std::string ListSlice::stringify(bool in_pipe) const
{
    return
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
            LIST_SLICE
                , "$LIST", list->stringify(in_pipe))
                , "$BEGIN", begin->stringify(in_pipe))
                , "$END", end->stringify(in_pipe))
                , "$STEP", step->stringify(in_pipe))
        ;
}

std::string ListSlice::Default::stringify(bool) const
{
    return "null";
}

std::string Dictionary::stringify(bool in_pipe) const
{
    std::vector<std::string> item_strings;
    std::for_each(items.begin()
                , items.end()
                , [&](ItemType const& item)
                  {
                      item_strings.push_back(
                          item.first->stringify(in_pipe) + ":" + item.second->stringify(in_pipe));
                  });
    return "({" + util::join(",", item_strings) + "})";
}

std::string ListAppend::stringify(bool in_pipe) const
{
    return lhs->stringify(in_pipe) + ".concat(" + rhs->stringify(in_pipe) + ")";
}

static std::map<std::string, std::string> genOpMap()
{
    std::map<std::string, std::string> m;
    m.insert(std::make_pair("=", "==="));
    m.insert(std::make_pair("!=", "!=="));
    return m;
}

static std::map<std::string, std::string> const OP_MAP(genOpMap());

static std::string stringifyOperator(std::string const& op_img)
{
    auto r = OP_MAP.find(op_img);
    if (OP_MAP.end() == r) {
        return op_img;
    }
    return r->second;
}

static std::string stringifyBinary(util::sptr<Expression const> const& lhs
                                 , std::string const& op
                                 , util::sptr<Expression const> const& rhs
                                 , bool in_pipe)
{
    return "(" + lhs->stringify(in_pipe) + stringifyOperator(op) + rhs->stringify(in_pipe) + ")";
}

static std::string stringifyPreUnary(std::string const& op
                                   , util::sptr<Expression const> const& rhs
                                   , bool in_pipe)
{
    return "(" + stringifyOperator(op) + rhs->stringify(in_pipe) + ")";
}

std::string BinaryOp::stringify(bool in_pipe) const
{
    return stringifyBinary(lhs, op, rhs, in_pipe);
}

std::string PreUnaryOp::stringify(bool in_pipe) const
{
    return stringifyPreUnary(op, rhs, in_pipe);
}

std::string Conjunction::stringify(bool in_pipe) const
{
    return stringifyBinary(lhs, "&&", rhs, in_pipe);
}

std::string Disjunction::stringify(bool in_pipe) const
{
    return stringifyBinary(lhs, "||", rhs, in_pipe);
}

std::string Negation::stringify(bool in_pipe) const
{
    return stringifyPreUnary("!", rhs, in_pipe);
}
