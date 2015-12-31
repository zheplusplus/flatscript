#ifndef __FLSC_SEMANTIC_LOOP_H__
#define __FLSC_SEMANTIC_LOOP_H__

#include <util/uid.h>

#include "node-base.h"

namespace semantic {

    struct RangeIteration
        : Statement
    {
        RangeIteration(misc::position const& pos
                     , std::string r
                     , util::sptr<Expression const> b
                     , util::sptr<Expression const> e
                     , util::sptr<Expression const> s
                     , util::sptr<Statement const> lp)
            : Statement(pos)
            , reference(std::move(r))
            , begin(std::move(b))
            , end(std::move(e))
            , step(std::move(s))
            , loop(std::move(lp))
            , loop_id(util::uid::next_id())
        {}

        bool isAsync() const;
        void compile(util::sref<Scope> scope) const;

        std::string const reference;
        util::sptr<Expression const> const begin;
        util::sptr<Expression const> const end;
        util::sptr<Expression const> const step;
        util::sptr<Statement const> loop;
        util::uid const loop_id;
    };

}

#endif /* __FLSC_SEMANTIC_LOOP_H__ */
