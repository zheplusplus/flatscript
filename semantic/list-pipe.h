#ifndef __STEKIN_SEMANTIC_LIST_PIPELINE_H__
#define __STEKIN_SEMANTIC_LIST_PIPELINE_H__

#include <misc/const.h>

#include "node-base.h"

namespace semantic {

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

        util::sptr<output::Expression const> compile(CompilingSpace& space) const;

        util::sptr<Expression const> const list;
        util::sptr<Expression const> const section;
        cons::PipelineType pipe_type;
    private:
        util::sptr<output::Expression const> _compileSync(CompilingSpace& space) const;
        util::sptr<output::Expression const> _compileAsync(CompilingSpace& space) const;
    };

}

#endif /* __STEKIN_SEMANTIC_LIST_PIPELINE_H__ */
