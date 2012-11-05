#ifndef __STEKIN_GRAMMAR_FUNCTION_H__
#define __STEKIN_GRAMMAR_FUNCTION_H__

#include <string>
#include <vector>

#include "block.h"

namespace grammar {

    struct Function {
        Function(misc::position const& ps
               , std::string const& n
               , std::vector<std::string> const& params
               , Block b)
            : pos(ps)
            , name(n)
            , param_names(params)
            , body(std::move(b))
        {}

        void compile(util::sref<semantic::Filter> filter) const;

        misc::position const pos;
        std::string const name;
        std::vector<std::string> const param_names;
        Block const body;
    };

}

#endif /* __STEKIN_GRAMMAR_FUNCTION_H__ */
