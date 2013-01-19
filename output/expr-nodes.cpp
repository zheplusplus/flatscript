#include <sstream>
#include <map>
#include <set>

#include <util/string.h>
#include <util/str-comprehension.h>
#include <report/errors.h>

#include "expr-nodes.h"
#include "name-mangler.h"
#include "function.h"
#include "block.h"

using namespace output;

static bool isReserved(std::string const& name)
{
    static std::set<std::string> const RESERVED_WORDS({
        "break", "case", "catch", "continue", "debugger", "default", "delete", "do", "else",
        "finally", "for", "function", "if", "in", "instanceof", "new", "return", "switch", "this",
        "throw", "try", "typeof", "var", "void", "while", "with",
    });
    return RESERVED_WORDS.find(name) != RESERVED_WORDS.end();
}

std::string PropertyNameExpr::strAsProp() const
{
    return str();
}

template <typename T>
static std::string strPrimitive(T const& t)
{
    return util::str(t);
}

std::string BoolLiteral::str() const
{
    return strPrimitive(value);
}

std::string IntLiteral::str() const
{
    return strPrimitive(value);
}

std::string FloatLiteral::str() const
{
    return strPrimitive(value);
}

std::string StringLiteral::str() const
{
    return util::cstr_repr(value.c_str(), value.size());
}

static std::vector<std::string> strList(util::ptrarr<Expression const> const& list)
{
    return list.mapv([&](util::sptr<Expression const> const& expr, int)
                     {
                         return expr->str();
                     });
}

std::string ListLiteral::str() const
{
    return "[" + util::join(",", strList(value)) + "]";
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

std::string Reference::str() const
{
    return formName(name);
}

std::string ImportedName::str() const
{
    if (isReserved(name)) {
        error::importReservedWord(pos, name);
    }
    return name;
}

std::string Call::str() const
{
    return callee->str() + "(" + util::join(",", strList(args)) + ")";
}

std::string MemberAccess::str() const
{
    if (isReserved(member)) {
        return referee->str() + "[\"" + member + "\"]";
    }
    return referee->str() + "." + member;
}

std::string Lookup::str() const
{
    return collection->str() + "[" + key->str() + "]";
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

std::string ListSlice::str() const
{
    return
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
            LIST_SLICE
                , "$LIST", list->str())
                , "$BEGIN", begin->str())
                , "$END", end->str())
                , "$STEP", step->str())
        ;
}

std::string ListSlice::Default::str() const
{
    return "null";
}

std::string Dictionary::str() const
{
    return "({"
         + util::join(",", items.mapv([&](util::ptrkv<Expression const> const& kv, int)
                                      {
                                          return kv.key->str() + ":" + kv.value->str();
                                      }))
         + "})";
}

std::string ListAppend::str() const
{
    return lhs->str() + ".concat(" + rhs->str() + ")";
}

static std::string strBinaryOperator(std::string const& op_img)
{
    static std::map<std::string, std::string> const map{
        { "=", "===" },
        { "!=", "!==" },
    };
    auto r = map.find(op_img);
    if (map.end() == r) {
        return op_img;
    }
    return r->second;
}

std::string BinaryOp::str() const
{
    return "(" + lhs->str() + strBinaryOperator(op) + rhs->str() + ")";
}

static std::string strPreUnaryOperator(std::string const& op_img)
{
    static std::map<std::string, std::string> const map{
        { "*", "new " },
    };
    auto r = map.find(op_img);
    if (map.end() == r) {
        return op_img;
    }
    return r->second;
}

std::string PreUnaryOp::str() const
{
    return "(" + strPreUnaryOperator(op) + rhs->str() + ")";
}

std::string Lambda::str() const
{
    std::ostringstream body_os;
    body->write(body_os);
    return
        util::replace_all(
        util::replace_all(
            "(function($PARAMETERS) { $BODY })"
                , "$PARAMETERS", util::join(",", formNames(param_names)))
                , "$BODY", body_os.str())
        ;
}

std::string AsyncReference::str() const
{
    return formAsyncRef(ref_id);
}

std::string This::str() const
{
    return "$this";
}
