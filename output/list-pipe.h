#ifndef __STEKIN_OUTPUT_LIST_PIPELINE_H__
#define __STEKIN_OUTPUT_LIST_PIPELINE_H__

#include "node-base.h"

namespace output {

    struct AsyncPipeResult
        : Expression
    {
        explicit AsyncPipeResult(misc::position const& pos)
            : Expression(pos)
        {}

        std::string str() const;
    };

    struct PipelineResult
        : Statement
    {
        explicit PipelineResult(util::sptr<Expression const> e)
            : expr(std::move(e))
        {}

        void write(std::ostream& os) const;

        util::sptr<Expression const> const expr;
    };

    struct PipelineNext
        : Statement
    {
        void write(std::ostream& os) const;
    };

    struct AsyncPipeline
        : Expression
    {
        AsyncPipeline(misc::position const& pos
                    , util::sptr<Expression const> ls
                    , util::sptr<Statement const> r
                    , util::sptr<Statement const> s)
            : Expression(pos)
            , list(std::move(ls))
            , recursion(std::move(r))
            , succession(std::move(s))
        {}

        std::string str() const;

        util::sptr<Expression const> const list;
        util::sptr<Statement const> const recursion;
        util::sptr<Statement const> const succession;
    };

    struct SyncPipeline
        : Expression
    {
        SyncPipeline(misc::position const& pos
                   , util::sptr<Expression const> ls
                   , util::sptr<Statement const> sec)
            : Expression(pos)
            , list(std::move(ls))
            , section(std::move(sec))
        {}

        std::string str() const;

        util::sptr<Expression const> const list;
        util::sptr<Statement const> const section;
    };

}

#endif /* __STEKIN_OUTPUT_LIST_PIPELINE_H__ */
