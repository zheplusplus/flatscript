#include <functional>
#include <limits>
#include <set>
#include <util/pointer.h>
#include <report/errors.h>

#include "tokenize.h"

using namespace grammar;

namespace {

    typedef TokenizeResult::Iterator it_type;
    typedef unsigned char byte;

    std::string const DIGITS("0123456789");
    std::string const DIGITS_UNDERSCORE(DIGITS + "_");
    std::string const LETTERS("qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM");
    std::string const LETTERS_UNDERSCORE(LETTERS + "_");

    struct State {
        static size_t const STATE_SIZE = std::numeric_limits<byte>::max() + 1;

        State()
            : ignored(false)
            , stop(false)
            , accept(false)
        {}

        State(State const&) = delete;
        ~State() = default;

        struct StateRef
            : util::sref<State>
        {
            StateRef()
                : util::sref<State>(nullptr)
            {}

            StateRef(util::sref<State> const& rhs)
                : util::sref<State>(rhs)
            {}

            StateRef(StateRef const& rhs)
                : util::sref<State>(rhs)
            {}
        };

        std::array<StateRef, STATE_SIZE> next;
        std::string type;
        bool ignored;
        bool stop;
        bool accept;

        util::sref<State> nextOrNul(byte b)
        {
            return this->next[b];
        }

        void set(byte b, util::sref<State> n)
        {
            this->next[b] = n;
        }

        void setm(std::string const& s, util::sref<State> n)
        {
            for (byte b: s) {
                this->set(b, n);
            }
        }

        void setAnythingBut(util::sref<State> n, std::string const& e)
        {
            std::set<byte> except(e.begin(), e.end());
            for (size_t b = 0; b < STATE_SIZE; ++b) {
                if (except.find(b) == except.end()) {
                    this->set(b, n);
                }
            }
        }

        void acceptAs(std::string type)
        {
            this->type = std::move(type);
            this->accept = true;
        }
    };

    struct Tokenizer {
        Tokenizer()
            : _entry(nullptr)
            , _current(nullptr)
        {
            this->_current = this->_entry = this->makeState();
            this->_initState();
        }

        util::sref<State> makeState()
        {
            util::sptr<State> next(new State);
            util::sref<State> next_ref(*next);
            this->_storage.push_back(std::move(next));
            return next_ref;
        }

        util::sref<State> currentState()
        {
            return this->_current;
        }

        Tokenizer* ignoreAll(std::string const& n)
        {
            util::sref<State> ignore(this->makeState());
            ignore->ignored = true;
            for (char ch: n) {
                this->_entry->set(ch, ignore);
                ignore->set(ch, ignore);
            }
            return this;
        }

        Tokenizer* simpleSymbols(std::string const& n, std::string type)
        {
            for (char ch: n) {
                this->fixed(std::string(1, ch), type);
            }
            return this;
        }

        Tokenizer* startWith(std::string const& n)
        {
            util::sref<State> next(this->makeState());
            this->_current = next;
            for (char ch: n) {
                this->_entry->set(ch, next);
            }
            return this;
        }

        Tokenizer* nextChar(std::string const& n)
        {
            util::sref<State> next(this->makeState());
            for (char ch: n) {
                this->currentState()->set(ch, next);
            }
            this->_current = next;
            return this;
        }

        Tokenizer* loop(std::string const& n)
        {
            for (char ch: n) {
                this->currentState()->set(ch, this->currentState());
            }
            return this;
        }

        Tokenizer* loopAnythingBut(std::string const& n)
        {
            this->currentState()->setAnythingBut(this->currentState(), n);
            return this;
        }

        Tokenizer* accept(std::string type)
        {
            this->currentState()->acceptAs(std::move(type));
            return this;
        }

        Tokenizer* acceptClear(std::string type)
        {
            this->accept(std::move(type));
            this->_current = this->_entry;
            return this;
        }

        Tokenizer* ignore()
        {
            this->currentState()->ignored = true;
            return this;
        }

        Tokenizer* ignoreClear()
        {
            this->ignore();
            this->_current = this->_entry;
            return this;
        }

        Tokenizer* stop()
        {
            this->currentState()->stop = true;
            return this;
        }

        Tokenizer* fixed(std::string const& image, std::string type) {
            if (image.empty()) {
                return this;
            }
            this->setPath(image);
            this->currentState()->acceptAs(std::move(type));
            return this;
        }

        Tokenizer* setPath(std::string const& n)
        {
            util::sref<State> state = this->_entry;
            for (char ch: n) {
                util::sref<State> next = state->nextOrNul(ch);
                if (next.nul()) {
                    next = this->makeState();
                    state->set(ch, next);
                }
                state = next;
            }
            this->_current = state;
            return this;
        }

        util::sref<State> getPath(std::string const& n) const
        {
            util::sref<State> state = this->_entry;
            for (char ch: n) {
                state = state->nextOrNul(ch);
                if (state.nul()) {
                    return state;
                }
            }
            return state;
        }

        TokenizeResult tokenize(it_type begin, it_type end, std::string const& file, int lineno)
        {
            int count_lf = 0;
            int current_line = lineno;
            std::vector<RoughToken> tokens;
            util::sref<State> state(this->_entry);
            std::function<void(byte)> next_char(nullptr);
            std::function<void(byte)> reset_consume(nullptr);
            std::string last_image;

            next_char = [&](byte b)
            {
                auto next_s(state->nextOrNul(b));
                if (next_s.not_nul()) {
                    state = next_s;
                    if (!state->ignored) {
                        last_image += b;
                    }
                    return;
                }
                if (state->accept) {
                    tokens.push_back(RoughToken(std::move(last_image), state->type,
                                                file, current_line));
                    current_line = lineno + count_lf;
                    return reset_consume(b);
                }
                if (state->ignored) {
                    return reset_consume(b);
                }
                error::invalidChar(misc::position(file, current_line), b);
            };

            reset_consume = [&](byte b)
            {
                last_image.clear();
                state = this->_entry;
                next_char(b);
            };

            for (; begin != end; ++begin) {
                char ch = *begin;
                next_char(ch);
                if (ch == '\n') {
                    ++count_lf;
                }
                if (state->stop) {
                    return TokenizeResult(std::move(tokens), ++begin, count_lf);
                }
            }
            if (!last_image.empty()) {
                if (state->accept) {
                    tokens.push_back(RoughToken(std::move(last_image), state->type,
                                                file, current_line));
                } else {
                    error::unexpectedEot(misc::position(file, current_line), state->type);
                }
            }
            return std::move(TokenizeResult(std::move(tokens), end, count_lf));
        }
    private:
        util::sref<State> _entry;
        util::sref<State> _current;
        std::vector<util::sptr<State>> _storage;

        void _initState();
    };

}

void Tokenizer::_initState()
{
    this->simpleSymbols("-+*%!=<>|~&^", "operator")
        ->fixed(":", "colon")
        ->fixed(",", "comma")
        ->fixed("(", "open_paren")
        ->fixed(")", "close_paren")
        ->fixed("[", "open_bracket")
        ->fixed("]", "close_bracket")
        ->fixed("{", "open_brace")
        ->fixed("}", "close_brace")
        ->fixed("++", "operator")
        ->fixed("<<", "operator")
        ->fixed(">>", "operator")
        ->fixed(">>>", "operator")
        ->fixed("<=", "operator")
        ->fixed(">=", "operator")
        ->fixed("!=", "operator")
        ->fixed("||", "operator")
        ->fixed("&&", "operator")
        ->fixed("|:", "pipe_sep")
        ->fixed("|?", "pipe_sep")
        ->fixed("$", "pipe_element")
        ->fixed("$element", "pipe_element")
        ->fixed("$i", "pipe_index")
        ->fixed("$index", "pipe_index")
        ->fixed("$k", "pipe_key")
        ->fixed("$key", "pipe_key")
        ->fixed("$r", "pipe_result")
        ->fixed("$result", "pipe_result")
        ->fixed("$e", "exception_obj")
        ->fixed("%%", "regular_async_param")
        ->fixed("$exception", "exception_obj")
        ->ignoreAll(" \t\r")

        ->startWith(LETTERS_UNDERSCORE)
        ->loop(LETTERS_UNDERSCORE + DIGITS)
        ->acceptClear("identifier")

        ->startWith(DIGITS)
        ->loop(DIGITS_UNDERSCORE)
        ->accept("integer")
        ->nextChar(".")
        ->accept("float")
        ->nextChar(DIGITS)
        ->accept("float")
        ->loop(DIGITS_UNDERSCORE)
        ->acceptClear("float")

        ->startWith(".")
        ->accept("operator")
        ->nextChar(DIGITS)
        ->accept("float")
        ->loop(DIGITS_UNDERSCORE)
        ->acceptClear("float")

        ->startWith("'")
        ->loopAnythingBut("'\\")
        ->nextChar("'")
        ->accept("string")

        ->startWith("\"")
        ->loopAnythingBut("\"\\")
        ->nextChar("\"")
        ->accept("string")

        ->startWith("#")
        ->ignore()
        ->loopAnythingBut("\n")
        ->nextChar("\n")
        ->stop()
        ->ignoreClear()

        ->startWith("\n")
        ->stop()
        ->acceptClear("eol")
        ;

    this->setPath("/");
    util::sref<State> slash(this->currentState());
    slash->acceptAs("operator");
    util::sref<State> regex_body(this->makeState());
    slash->setAnythingBut(regex_body, " \\/\t\n\r");
    regex_body->setAnythingBut(regex_body, "\\/\n");
    util::sref<State> regex_escape(this->makeState());
    regex_body->set('\\', regex_escape);
    slash->set('\\', regex_escape);
    regex_escape->setAnythingBut(regex_body, "");
    util::sref<State> regex(this->makeState());
    slash->set('/', regex);
    regex_body->set('/', regex);
    regex->setm(LETTERS, regex);
    regex->acceptAs(regex_body->type = regex_escape->type = "regex");

    this->setPath("'\\");
    util::sref<State> str_escape = this->currentState();
    str_escape->setAnythingBut(this->getPath("'"), "");
    str_escape->type = this->getPath("'")->type = "string";

    this->setPath("\"\\");
    str_escape = this->currentState();
    str_escape->setAnythingBut(this->getPath("\""), "");
    str_escape->type = this->getPath("\"")->type = "string";

    this->setPath("'''");
    util::sref<State> triple_quoted_str = this->currentState();
    triple_quoted_str->setAnythingBut(triple_quoted_str, "'\\");
    util::sref<State> inner_quotes_one = this->makeState();
    util::sref<State> inner_quotes_two = this->makeState();
    util::sref<State> str_finished = this->makeState();
    str_escape = this->makeState();
    triple_quoted_str->set('\'', inner_quotes_one);
    inner_quotes_one->setAnythingBut(triple_quoted_str, "'\\");
    inner_quotes_one->set('\'', inner_quotes_two);
    inner_quotes_two->setAnythingBut(triple_quoted_str, "'\\");
    inner_quotes_two->set('\'', str_finished);
    str_finished->acceptAs("triple_quoted_string");
    triple_quoted_str->set('\\', str_escape);
    inner_quotes_one->set('\\', str_escape);
    inner_quotes_two->set('\\', str_escape);
    str_escape->setAnythingBut(triple_quoted_str, "");
    str_escape->type = inner_quotes_two->type = inner_quotes_one->type =
        this->getPath("'''")->type = "triple_quoted_string";

    this->setPath("\"\"\"");
    triple_quoted_str = this->currentState();
    triple_quoted_str->setAnythingBut(triple_quoted_str, "\"\\");
    inner_quotes_one = this->makeState();
    inner_quotes_two = this->makeState();
    str_finished = this->makeState();
    str_escape = this->makeState();
    triple_quoted_str->set('"', inner_quotes_one);
    inner_quotes_one->setAnythingBut(triple_quoted_str, "\"\\");
    inner_quotes_one->set('"', inner_quotes_two);
    inner_quotes_two->setAnythingBut(triple_quoted_str, "\"\\");
    inner_quotes_two->set('"', str_finished);
    str_finished->acceptAs("triple_quoted_string");
    triple_quoted_str->set('\\', str_escape);
    inner_quotes_one->set('\\', str_escape);
    inner_quotes_two->set('\\', str_escape);
    str_escape->setAnythingBut(triple_quoted_str, "");
    str_escape->type = inner_quotes_two->type = inner_quotes_one->type =
        this->getPath("\"\"\"")->type = "triple_quoted_string";
}

TokenizeResult grammar::tokenize(it_type begin, it_type end, std::string const& file, int lineno)
{
    Tokenizer t;
    return t.tokenize(begin, end, file, lineno);
}
