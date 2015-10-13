#ifndef __STEKIN_SEMANTIC_CLASS_H__
#define __STEKIN_SEMANTIC_CLASS_H__

#include <output/class.h>

#include "block.h"

namespace semantic {

    struct Class {
        struct Constructor {
            Constructor(misc::position const& ps, std::vector<std::string> params, Block b)
                : pos(ps)
                , param_names(std::move(params))
                , body(std::move(b))
            {}

            misc::position const pos;
            std::vector<std::string> const param_names;
            Block const body;
        };

        Class(misc::position const& ps, std::string n, std::string base_n
            , Block b, util::sptr<Constructor const> ct)
                : pos(ps)
                , name(std::move(n))
                , base_class_name(std::move(base_n))
                , body(std::move(b))
                , ctor_or_nul(std::move(ct))
        {}

        util::sptr<output::Class const> compile(util::sref<SymbolTable> st) const;

        misc::position const pos;
        std::string const name;
        std::string const base_class_name;
        Block const body;
        util::sptr<Constructor const> ctor_or_nul;
    };

}

#endif /* __STEKIN_SEMANTIC_CLASS_H__ */
