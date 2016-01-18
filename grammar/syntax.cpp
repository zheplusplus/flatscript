#include <map>
#include <set>
#include <functional>
#include <util/string.h>
#include <util/str-comprehension.h>
#include <report/errors.h>

#include "syntax.h"
#include "tokenize.h"
#include "tokens.h"
#include "expr-nodes.h"
#include "clause-builder.h"
#include "clauses.h"

using namespace grammar;

typedef std::string::const_iterator Iterator;
typedef std::function<util::sptr<Token>(RoughToken&)> MakeToken;

static int count_indent(Iterator& begin, Iterator end, std::string const& file, int& lineno)
{
    int indent = 0;
    for (; begin != end; ++begin) {
        switch (*begin) {
        case ' ':
            ++indent;
            break;
        case '\t':
            error::tabAsIndent(misc::position(file, lineno));
            break;
        case '\n':
            indent = 0;
            ++lineno;
            break;
        default:
            return indent;
        }
    }
    return 0;
}

static std::map<std::string, TokenType> const KEYWORDS_MAP({
    {"if", IF},
    {"else", ELSE},
    {"ifnot", IFNOT},
    {"for", FOR},
    {"break", BREAK},
    {"continue", CONTINUE},
    {"try", TRY},
    {"catch", CATCH},
    {"throw", THROW},
    {"func", FUNC},
    {"return", RETURN},
    {"class", CLASS},
    {"super", SUPER},
    {"ctor", CONSTRUCTOR},
    {"extern", EXTERN},
    {"export", EXPORT},
    {"typeof", OPERATOR},
    {"enum", ENUM},
    {"include", INCLUDE},
});

static std::set<std::string> const RESERVED({
    "from", "delete", "elif", "while", "gen", "yield", "with", "finally", "switch", "case",
});

static std::map<std::string, MakeToken> const FACTOR_KEYWORD_MAP({
    {"true", [](RoughToken& t)
        {
            return util::mkptr(new FactorToken(t.pos, util::mkptr(new BoolLiteral(t.pos, true)),
                                               std::move(t.image)));
        }},
    {"false", [](RoughToken& t)
        {
            return util::mkptr(new FactorToken(t.pos, util::mkptr(new BoolLiteral(t.pos, false)),
                                               std::move(t.image)));
        }},
    {"this", [](RoughToken& t)
        {
            return util::mkptr(new FactorToken(t.pos, util::mkptr(new This(t.pos)),
                                               std::move(t.image)));
        }},
});

static MakeToken typed_token_maker(TokenType type)
{
    return [=](RoughToken& t)
    {
        return util::mkptr(new TypedToken(t.pos, std::move(t.image), type));
    };
}

template <typename Function>
static MakeToken factor_token_maker(Function f)
{
    return [=](RoughToken& t)
    {
        return util::mkptr(new grammar::FactorToken(t.pos, f(t.pos, t.image), t.image));
    };
}

static MakeToken identifier_maker()
{
    return [](RoughToken& t) -> util::sptr<Token>
    {
        auto fkw(KEYWORDS_MAP.find(t.image));
        if (fkw != KEYWORDS_MAP.end()) {
            return util::mkptr(new TypedToken(t.pos, std::move(t.image), fkw->second));
        }
        auto fkwf(FACTOR_KEYWORD_MAP.find(t.image));
        if (fkwf != FACTOR_KEYWORD_MAP.end()) {
            return fkwf->second(t);
        }
        if (RESERVED.find(t.image) != RESERVED.end()) {
            error::reservedWord(t.pos, t.image);
        }
        return util::mkptr(new grammar::FactorToken(t.pos, util::mkptr(
                new Identifier(t.pos, t.image)), t.image));
    };
}

static std::map<std::string, MakeToken> const TOKEN_MAKE_MAP({
    {"comma", ::typed_token_maker(COMMA)},
    {"operator", ::typed_token_maker(OPERATOR)},
    {"open_paren", ::typed_token_maker(OPEN_PAREN)},
    {"close_paren", ::typed_token_maker(CLOSE_PAREN)},
    {"open_bracket", ::typed_token_maker(OPEN_BRACKET)},
    {"close_bracket", ::typed_token_maker(CLOSE_BRACKET)},
    {"open_brace", ::typed_token_maker(OPEN_BRACE)},
    {"close_brace", ::typed_token_maker(CLOSE_BRACE)},
    {"pipe_sep", ::typed_token_maker(PIPE_SEP)},
    {"colon", ::typed_token_maker(COLON)},
    {"pipe_element", ::factor_token_maker(
        [](misc::position const& p, std::string const&)
        {
            return util::mkptr(new grammar::PipeElement(p));
        })},
    {"pipe_index", ::factor_token_maker(
        [](misc::position const& p, std::string const&)
        {
            return util::mkptr(new grammar::PipeIndex(p));
        })},
    {"pipe_key", ::factor_token_maker(
        [](misc::position const& p, std::string const&)
        {
            return util::mkptr(new grammar::PipeKey(p));
        })},
    {"pipe_result", ::factor_token_maker(
        [](misc::position const& p, std::string const&)
        {
            return util::mkptr(new grammar::PipeResult(p));
        })},
    {"exception_obj", ::factor_token_maker(
        [](misc::position const& p, std::string const&)
        {
            return util::mkptr(new grammar::ExceptionObj(p));
        })},
    {"regular_async_param", ::factor_token_maker(
        [](misc::position const& p, std::string const&)
        {
            return util::mkptr(new grammar::RegularAsyncParam(p));
        })},
    {"identifier", ::identifier_maker()},

    {"integer", ::factor_token_maker(
        [](misc::position const& p, std::string const& image)
        {
            return util::mkptr(new grammar::IntLiteral(p, util::replace_all(image, "_", "")));
        })},
    {"float", ::factor_token_maker(
        [](misc::position const& p, std::string const& image)
        {
            return util::mkptr(new grammar::FloatLiteral(p, util::replace_all(image, "_", "")));
        })},
    {"string", ::factor_token_maker(
        [](misc::position const& p, std::string const& image)
        {
            return util::mkptr(new grammar::StringLiteral(
                    p, util::comprehend(image.c_str(), 1, -1)));
        })},
    {"triple_quoted_string", ::factor_token_maker(
        [](misc::position const& p, std::string const& image)
        {
            return util::mkptr(new grammar::StringLiteral(
                    p, util::comprehend(image.c_str(), 3, -3)));
        })},
    {"regex", [](RoughToken& t)
        {
            return util::mkptr(new grammar::FactorToken(
                    t.pos, grammar::makeRegEx(t.pos, t.image), t.image));
        }},
});

static std::vector<util::sptr<Token>> convertTokens(std::vector<RoughToken>& tokens)
{
    std::vector<util::sptr<Token>> result;
    for (auto& token: tokens) {
        result.push_back(TOKEN_MAKE_MAP.find(token.type)->second(token));
    }
    return std::move(result);
}

util::sptr<semantic::Statement const> grammar::build(
            std::string const& file, std::string const& source)
{
    ClauseBuilder builder;
    int lineno = 1;
    Iterator iter = source.begin();
    Iterator end = source.end();
    while (iter != end) {
        int indent = count_indent(iter, end, file, lineno);
        TokenizeResult result(tokenize(iter, end, file, lineno));
        if (!result.tokens.empty()) {
            builder.addTokens(indent, misc::position(file, lineno), convertTokens(result.tokens));
        }
        lineno += result.lineno_inc;
        iter = result.iterator;
    }
    return builder.buildAndClear(misc::position(file, lineno));
}
