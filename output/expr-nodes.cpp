#include <algorithm>
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

std::string Undefined::str() const
{
    return "undefined";
}

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

std::string FunctionInvocation::str() const
{
    return func->mangledName() + "(" + util::join(",", strList(args)) + ")";
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
"        end = (end === undefined) ? list.length : round(end);\n"
"        for (; begin < end; begin += step) {\n"
"            r.push(list[begin]);\n"
"        }\n"
"        return r;\n"
"    }\n"
"    begin = (begin === undefined) ? list.length - 1 : round(begin);\n"
"    end = (end === undefined) ? -1 : round(end)\n"
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

std::string Assignment::str() const
{
    return "(" + lhs->str() + "=" + rhs->str() + ")";
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
    std::string copy_decls_str;
    auto form_func(formNames);
    if (copy_decls) {
        form_func = formTransientParams;
        std::for_each(param_names.begin()
                    , param_names.end()
                    , [&](std::string const& p)
                      {
                          copy_decls_str += (formName(p) + "=" + formTransientParam(p) + ";");
                      });
    }
    return
        util::replace_all(
        util::replace_all(
        util::replace_all(
            "(function(#PARAMETERS) { #COPY_DECLS#BODY })"
                , "#PARAMETERS", util::join(",", form_func(param_names)))
                , "#COPY_DECLS", copy_decls_str)
                , "#BODY", body_os.str())
        ;
}

std::string RegularAsyncLambda::str() const
{
    std::ostringstream body_os;
    body->write(body_os);
    std::vector<std::string> params(param_names);
    params.insert(params.begin() + async_param_index, term::regularAsyncCallback());
    return
        util::replace_all(
        util::replace_all(
            "(function(#PARAMETERS) { #BODY })"
                , "#PARAMETERS", util::join(",", formNames(params)))
                , "#BODY", body_os.str())
        ;
}

std::string AsyncReference::str() const
{
    return formAsyncRef(ref_id);
}

std::string RegularAsyncCallbackArg::str() const
{
    std::ostringstream body_os;
    body->write(body_os);
    return
        util::replace_all(
        util::replace_all(
        util::replace_all(
            "(function($cb_err, #CALLBACK_RESULT) {\n"
            "    if ($cb_err) #RAISE_EXC\n"
            "#BODY\n"
            "})"
                , "#CALLBACK_RESULT", formAsyncRef(util::id(this)))
                , "#RAISE_EXC", raiser("$cb_err"))
                , "#BODY", body_os.str())
        ;
}

std::string This::str() const
{
    return "$this";
}

std::string Conditional::str() const
{
    return "(" + predicate->str() + "?" + consequence->str() + ":" + alternative->str() + ")";
}
