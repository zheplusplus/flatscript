#ifndef __FLSC_GRAMMAR_SYNTAX_H__
#define __FLSC_GRAMMAR_SYNTAX_H__

#include "node-base.h"

namespace grammar {

    util::sptr<semantic::Statement const> build(std::string const& file, std::string const& source);

}

#endif /* __FLSC_GRAMMAR_SYNTAX_H__ */
