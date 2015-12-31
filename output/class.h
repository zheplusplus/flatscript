#ifndef __STEKIN_OUTPUT_CLASS_H__
#define __STEKIN_OUTPUT_CLASS_H__

#include <map>
#include <vector>
#include <string>
#include <util/pointer.h>

#include "node-base.h"

namespace output {

    struct Constructor {
        Constructor(std::vector<std::string> params, util::sptr<Statement const> b)
            : param_names(std::move(params))
            , body(std::move(b))
        {}

        std::vector<std::string> const param_names;
        util::sptr<Statement const> const body;
    };

    struct ClassInitFunc
        : Statement
    {
        ClassInitFunc(std::string n, bool inh
                    , std::map<std::string, util::sptr<Expression const>> memfns
                    , util::sptr<Constructor const> ct)
            : name(std::move(n))
            , member_funcs(std::move(memfns))
            , ctor_or_nul(std::move(ct))
            , inherit(inh)
        {}

        void write(std::ostream& os) const;
        bool mayThrow() const { return false; }

        std::string const name;
        std::map<std::string, util::sptr<Expression const>> const member_funcs;
        util::sptr<Constructor const> ctor_or_nul;
        bool const inherit;
    };

    struct ClassInitCall
        : Statement
    {
        ClassInitCall(std::string n, util::sptr<Expression const> base)
            : name(std::move(n))
            , base_class_or_nul(std::move(base))
        {}

        void write(std::ostream& os) const;
        bool mayThrow() const { return true; }

        std::string const name;
        util::sptr<Expression const> const base_class_or_nul;
    };

}

#endif /* __STEKIN_OUTPUT_CLASS_H__ */
