#ifndef __STEKIN_OUTPUT_CLASS_H__
#define __STEKIN_OUTPUT_CLASS_H__

#include <map>
#include <string>

#include "fwd-decl.h"
#include "expr-nodes.h"

namespace output {

    struct Class {
        struct Constructor {
            Constructor(std::vector<std::string> params, util::sptr<Statement const> b)
                : param_names(std::move(params))
                , body(std::move(b))
            {}

            std::vector<std::string> const param_names;
            util::sptr<Statement const> const body;
        };

        Class(std::string n, util::sptr<Expression const> base
            , std::map<std::string, util::sptr<Lambda const>> memfns
            , util::sptr<Constructor const> ct)
                : name(std::move(n))
                , base_class_or_nul(std::move(base))
                , member_funcs(std::move(memfns))
                , ctor_or_nul(std::move(ct))
        {}

        void write(std::ostream& os) const;

        std::string const name;
        util::sptr<Expression const> const base_class_or_nul;
        std::map<std::string, util::sptr<Lambda const>> const member_funcs;
        util::sptr<Constructor const> ctor_or_nul;
    };

}

#endif /* __STEKIN_OUTPUT_CLASS_H__ */
