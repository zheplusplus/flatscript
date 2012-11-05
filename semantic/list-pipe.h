#ifndef __STEKIN_SEMANTIC_LIST_PIPELINE_H__
#define __STEKIN_SEMANTIC_LIST_PIPELINE_H__

#include <vector>

#include "node-base.h"

namespace semantic {

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

        util::sptr<output::Expression const> compile(util::sref<SymbolTable> st) const;

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

        util::sptr<output::Expression const> compile(util::sref<SymbolTable> st) const;

        util::sptr<Expression const> const list;
        util::sptr<Expression const> const filter;
    };

}

#endif /* __STEKIN_SEMANTIC_LIST_PIPELINE_H__ */
