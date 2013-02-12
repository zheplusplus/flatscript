#ifndef __STEKIN_SEMANTIC_LIST_PIPELINE_H__
#define __STEKIN_SEMANTIC_LIST_PIPELINE_H__

#include "block.h"

namespace semantic {

    struct Pipeline
        : Expression
    {
        Pipeline(misc::position const& pos, util::sptr<Expression const> ls, Block sec)
            : Expression(pos)
            , list(std::move(ls))
            , section(std::move(sec))
        {}

        util::sptr<output::Expression const> compile(BaseCompilingSpace& space) const;
        bool isAsync() const;

        util::sptr<Expression const> const list;
        Block const section;
    private:
        util::sptr<output::Expression const> _compileSync(BaseCompilingSpace& space) const;
        util::sptr<output::Expression const> _compileAsync(BaseCompilingSpace& space) const;
    public:
        static util::sptr<Expression const> createMapper(misc::position const& pos
                                                       , util::sptr<Expression const> ls
                                                       , util::sptr<Expression const> sec);
        static util::sptr<Expression const> createFilter(misc::position const& pos
                                                       , util::sptr<Expression const> ls
                                                       , util::sptr<Expression const> sec);
    };

    struct PipeElement
        : Expression
    {
        explicit PipeElement(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<output::Expression const> compile(BaseCompilingSpace& space) const;
    };

    struct PipeIndex
        : Expression
    {
        explicit PipeIndex(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<output::Expression const> compile(BaseCompilingSpace& space) const;
    };

    struct PipeKey
        : Expression
    {
        explicit PipeKey(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<output::Expression const> compile(BaseCompilingSpace& space) const;
    };

    struct PipeResult
        : Expression
    {
        explicit PipeResult(misc::position const& pos)
            : Expression(pos)
        {}

        util::sptr<output::Expression const> compile(BaseCompilingSpace& space) const;
    };

}

#endif /* __STEKIN_SEMANTIC_LIST_PIPELINE_H__ */
