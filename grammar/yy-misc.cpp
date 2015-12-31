#include <stdexcept>
#include <globals.h>

#include "yy-misc.h"
#include "clauses.h"

using namespace grammar;

extern FILE* yyin;
int yyparse();

int yywrap(void)
{
    return 1;
}

ClauseBuilder grammar::builder;
int grammar::last_indent = 0;
int grammar::lineno = 1;

misc::position grammar::here()
{
    return misc::position(grammar::lineno);
}

void grammar::parse()
{
    if (!flats::Globals::g.input_file.empty()) {
        yyin = fopen(flats::Globals::g.input_file.data(), "r");
        if (yyin == nullptr) {
            throw std::invalid_argument("No such file " + flats::Globals::g.input_file);
        }
    }
    ::yyparse();
}

TokenSequence::TokenSequence(Token* token)
{
    _list.push_back(util::mkptr(token));
}

TokenSequence* TokenSequence::add(Token* token)
{
    _list.push_back(util::mkptr(token));
    return this;
}

std::vector<util::sptr<Token>> TokenSequence::deliver()
{
    std::vector<util::sptr<Token>> list(std::move(_list));
    delete this;
    return std::move(list);
}
