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
#include "class.h"
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

template <typename T>
static std::string strPrimitive(T const& t)
{
    return util::str(t);
}

template <typename T>
static std::string strPrimitive(T const& t, bool)
{
    return '(' + strPrimitive(t) + ')';
}

std::string BoolLiteral::str() const
{
    return strPrimitive(value);
}

std::string IntLiteral::str() const
{
    return strPrimitive(value, true);
}

std::string FloatLiteral::str() const
{
    return strPrimitive(value, true);
}

std::string StringLiteral::str() const
{
    return util::cstr_repr(value.c_str(), value.size());
}

std::string RegEx::str() const
{
    return this->value;
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

bool ListLiteral::mayThrow() const
{
    return value.any([](util::sptr<Expression const> const& e, int)
                     {
                         return e->mayThrow();
                     });
}

std::string Reference::str() const
{
    return formName(name);
}

std::string SubReference::str() const
{
    return formSubName(name, space_id);
}

std::string TransientParamReference::str() const
{
    return formTransientParam(name);
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

std::string ListSlice::str() const
{
    return
        util::replace_all(
        util::replace_all(
        util::replace_all(
        util::replace_all(
            "$listslice($LIST, $BEGIN, $END, $STEP)"
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
    auto form_func(mangle_as_param ? formTransientParams : formNames);
    return
        util::replace_all(
        util::replace_all(
            "(function(#PARAMETERS) { #BODY })"
                , "#PARAMETERS", util::join(",", form_func(param_names)))
                , "#BODY", body_os.str())
        ;
}

std::string RegularAsyncLambda::str() const
{
    std::ostringstream body_os;
    body->write(body_os);
    std::vector<std::string> params(formNames(param_names));
    params.insert(params.begin() + async_param_index, TERM_REGULAR_ASYNC_CALLBACK);
    return
        util::replace_all(
        util::replace_all(
            "(function(#PARAMETERS) { #BODY })"
                , "#PARAMETERS", util::join(",", params))
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
                , "#RAISE_EXC", thrower->scheme("$cb_err"))
                , "#BODY", body_os.str())
        ;
}

std::string This::str() const
{
    return "$this";
}

std::string SuperFunc::str() const
{
    return "$super." + this->property + ".call";
}

std::string Conditional::str() const
{
    return "(" + predicate->str() + "?" + consequence->str() + ":" + alternative->str() + ")";
}

std::string ExceptionObj::str() const
{
    return TERM_EXCEPTION;
}

std::string ConditionalCallbackParameter::str() const
{
    return TERM_CONDITIONAL_CALLBACK_PARAMETER;
}

std::string SuperConstructorCall::str() const
{
    return this->class_name + ".$super.constructor.apply($this,["
         + util::join(",", ::strList(this->args)) + "]);";
}
