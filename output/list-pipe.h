#ifndef __STEKIN_OUTPUT_LIST_PIPELINE_H__
#define __STEKIN_OUTPUT_LIST_PIPELINE_H__

#include <vector>

#include "node-base.h"

namespace output {

    struct ListPipeMapper
        : Expression
    {
        ListPipeMapper(misc::position const& pos
                     , util::sptr<Expression const> l
                     , util::sptr<Expression const> m)
            : Expression(pos)
            , list(std::move(l))
            , mapper(std::move(m))
        {}

        std::string str(bool in_pipe) const;

        util::sptr<Expression const> const list;
        util::sptr<Expression const> const mapper;
    };

    struct ListPipeFilter
        : Expression
    {
        ListPipeFilter(misc::position const& pos
                     , util::sptr<Expression const> l
                     , util::sptr<Expression const> f)
            : Expression(pos)
            , list(std::move(l))
            , filter(std::move(f))
        {}

        std::string str(bool in_pipe) const;

        util::sptr<Expression const> const list;
        util::sptr<Expression const> const filter;
    };

}

#endif /* __STEKIN_OUTPUT_LIST_PIPELINE_H__ */
