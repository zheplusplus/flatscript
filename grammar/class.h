#ifndef __STEKIN_GRAMMAR_CLASS_H__
#define __STEKIN_GRAMMAR_CLASS_H__

#include <semantic/class.h>

#include "block.h"

namespace grammar {

    struct Constructor {
        Constructor(misc::position const& ps, std::vector<std::string> params, Block b
                  , bool si, std::vector<util::sptr<Expression const>> sa)
            : pos(ps)
            , param_names(std::move(params))
            , body(std::move(b))
            , super_init(si)
            , super_ctor_args(std::move(sa))
        {}

        misc::position const pos;
        std::vector<std::string> const param_names;
        Block const body;
        bool const super_init;
        std::vector<util::sptr<Expression const>> const super_ctor_args;
    };

    struct Class {
        Class(misc::position const& ps, std::string n, std::string base_n, Block b)
            : pos(ps)
            , name(std::move(n))
            , base_class_name(std::move(base_n))
            , body(std::move(b))
        {}

        util::sptr<semantic::Class const> compile() const;

        misc::position const pos;
        std::string const name;
        std::string const base_class_name;
        Block const body;
    };

}

#endif /* __STEKIN_GRAMMAR_CLASS_H__ */
