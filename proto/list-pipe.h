#ifndef __STEKIN_PROTO_LIST_PIPELINE_H__
#define __STEKIN_PROTO_LIST_PIPELINE_H__

#include <vector>

#include "node-base.h"

namespace proto {

    struct PipeBase {
        virtual ~PipeBase() {}

        explicit PipeBase(util::sptr<Expression const> e)
            : expr(std::move(e))
        {}

        virtual void writeBegin() const = 0;
        virtual void writeEnd() const = 0;

        util::sptr<Expression const> expr;
    };

    struct PipeMap
        : public PipeBase
    {
        explicit PipeMap(util::sptr<Expression const> expr)
            : PipeBase(std::move(expr))
        {}

        void writeBegin() const;
        void writeEnd() const;
    };

    struct PipeFilter
        : public PipeBase
    {
        explicit PipeFilter(util::sptr<Expression const> expr)
            : PipeBase(std::move(expr))
        {}

        void writeBegin() const;
        void writeEnd() const;
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

        void write() const;
        void writeAsPipe() const;

        util::sptr<Expression const> const list;
        std::vector<util::sptr<PipeBase const>> const pipeline;
    };

}

#endif /* __STEKIN_PROTO_LIST_PIPELINE_H__ */
