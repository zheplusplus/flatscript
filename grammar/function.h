#ifndef __STEKIN_GRAMMAR_FUNCTION_H__
#define __STEKIN_GRAMMAR_FUNCTION_H__

#include "block.h"

namespace grammar {

    struct Function {
        Function(misc::position const& ps
               , std::string const& n
               , std::vector<std::string> const& params
               , int async_param_idx
               , Block b)
            : pos(ps)
            , name(n)
            , param_names(params)
            , async_param_index(async_param_idx)
            , body(std::move(b))
        {}

        void compile(util::sref<semantic::Filter> filter) const;

        misc::position const pos;
        std::string const name;
        std::vector<std::string> const param_names;
        int const async_param_index;
        Block const body;
    };

}

#endif /* __STEKIN_GRAMMAR_FUNCTION_H__ */
