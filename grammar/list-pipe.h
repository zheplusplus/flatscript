#ifndef __STEKIN_GRAMMAR_LIST_PIPELINE_H__
#define __STEKIN_GRAMMAR_LIST_PIPELINE_H__

#include <vector>

#include "node-base.h"

namespace grammar {

    struct PipeBase {
        virtual ~PipeBase() {}
        virtual util::sptr<flchk::PipeBase const> compile() const = 0;

        explicit PipeBase(util::sptr<Expression const> e)
            : expr(std::move(e))
        {}

        util::sptr<Expression const> expr;
    };

    struct PipeMap
        : public PipeBase
    {
        explicit PipeMap(util::sptr<Expression const> expr)
            : PipeBase(std::move(expr))
        {}

        util::sptr<flchk::PipeBase const> compile() const;
    };

    struct PipeFilter
        : public PipeBase
    {
        explicit PipeFilter(util::sptr<Expression const> expr)
            : PipeBase(std::move(expr))
        {}

        util::sptr<flchk::PipeBase const> compile() const;
    };

    struct ListPipeline
        : public Expression
    {
        ListPipeline(misc::position const& pos
                   , util::sptr<Expression const> l
                   , std::vector<util::sptr<PipeBase const>> p)
            : Expression(pos)
            , list(std::move(l))
            , pipeline(std::move(p))
        {}

        util::sptr<flchk::Expression const> compile() const;

        util::sptr<Expression const> const list;
        std::vector<util::sptr<PipeBase const>> const pipeline;
    };

}

#endif /* __STEKIN_GRAMMAR_LIST_PIPELINE_H__ */
