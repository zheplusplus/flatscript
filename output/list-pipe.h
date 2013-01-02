#ifndef __STEKIN_OUTPUT_LIST_PIPELINE_H__
#define __STEKIN_OUTPUT_LIST_PIPELINE_H__

#include <misc/const.h>

#include "node-base.h"

namespace output {

    struct Pipeline
        : Expression
    {
        Pipeline(misc::position const& pos
               , util::sptr<Expression const> ls
               , util::sptr<Expression const> sec
               , cons::PipelineType tp)
            : Expression(pos)
            , list(std::move(ls))
            , section(std::move(sec))
            , pipe_type(tp)
        {}

        std::string str() const;

        util::sptr<Expression const> const list;
        util::sptr<Expression const> const section;
        cons::PipelineType pipe_type;
    };

    struct AsyncPipeResult
        : Expression
    {
        explicit AsyncPipeResult(misc::position const& pos)
            : Expression(pos)
        {}

        std::string str() const;
    };

    struct AsyncPipeBody
        : Statement
    {
        AsyncPipeBody(util::sptr<Expression const> e, cons::PipelineType tp)
            : expr(std::move(e))
            , pipe_type(tp)
        {}

        void write(std::ostream& os) const;

        util::sptr<Expression const> const expr;
        cons::PipelineType pipe_type;
    };

    struct AsyncPipe
        : Expression
    {
        AsyncPipe(misc::position const& pos
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

}

#endif /* __STEKIN_OUTPUT_LIST_PIPELINE_H__ */
