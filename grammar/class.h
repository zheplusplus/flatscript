#ifndef __STEKIN_GRAMMAR_CLASS_H__
#define __STEKIN_GRAMMAR_CLASS_H__

#include <semantic/class.h>

#include "block.h"

namespace grammar {

    struct Constructor {
        Constructor(misc::position const& ps
                  , std::vector<std::string> params
                  , util::sptr<Block const> b
                  , bool si
                  , std::vector<util::sptr<Expression const>> sa)
            : pos(ps)
            , param_names(std::move(params))
            , body(std::move(b))
            , super_init(si)
            , super_ctor_args(std::move(sa))
        {}

        misc::position const pos;
        std::vector<std::string> const param_names;
        util::sptr<Block const> const body;
        bool const super_init;
        std::vector<util::sptr<Expression const>> const super_ctor_args;
    };

    struct Class
        : Statement
    {
        Class(misc::position const& ps, std::string n
            , util::sptr<Expression const> base, util::sptr<Block const> b)
                : Statement(ps)
                , name(std::move(n))
                , base_class(std::move(base))
                , body(std::move(b))
        {}

        util::sptr<semantic::Statement const> compile() const;

        std::string const name;
        util::sptr<Expression const> const base_class;
        util::sptr<Block const> const body;
    };

}

#endif /* __STEKIN_GRAMMAR_CLASS_H__ */
