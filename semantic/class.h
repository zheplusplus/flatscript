#ifndef __STEKIN_SEMANTIC_CLASS_H__
#define __STEKIN_SEMANTIC_CLASS_H__

#include <output/class.h>

#include "block.h"

namespace semantic {

    struct Constructor {
        Constructor(misc::position const& ps, std::vector<std::string> params, Block b
                  , std::string const& class_name, bool si
                  , util::ptrarr<Expression const> super_ctor_args);

        util::sptr<output::Constructor const> compile(
                util::sref<SymbolTable> st, bool has_base_class) const;

        bool superInit() const
        {
            return this->super_init.not_nul();
        }

        misc::position const pos;
        std::vector<std::string> const param_names;
        util::sptr<Statement const> const super_init;
        Block const body;
    };

    struct Class {
        Class(misc::position const& ps, std::string n, std::string base_n
            , Block b, util::sptr<Constructor const> ct)
                : pos(ps)
                , name(std::move(n))
                , base_class_name(std::move(base_n))
                , body(std::move(b))
                , ctor_or_nul(std::move(ct))
        {}

        util::sptr<output::Class const> compile(util::sref<SymbolTable> st) const;

        bool hasBaseClass() const
        {
            return !this->base_class_name.empty();
        }

        misc::position const pos;
        std::string const name;
        std::string const base_class_name;
        Block const body;
        util::sptr<Constructor const> const ctor_or_nul;
    };

}

#endif /* __STEKIN_SEMANTIC_CLASS_H__ */
