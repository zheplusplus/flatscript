#ifndef __STEKIN_OUTPUT_LIST_PIPELINE_H__
#define __STEKIN_OUTPUT_LIST_PIPELINE_H__

#include "node-base.h"
#include "methods.h"

namespace output {

    struct PipelineContinue
        : Statement
    {
        void write(std::ostream& os) const;
    };

    struct AsyncPipeline
        : Expression
    {
        AsyncPipeline(misc::position const& pos
                    , util::sptr<Expression const> ls
                    , util::sptr<Statement const> recur
                    , util::sptr<Statement const> suc
                    , Method const& r)
            : Expression(pos)
            , list(std::move(ls))
            , recursion(std::move(recur))
            , succession(std::move(suc))
            , raiser(r)
        {}

        std::string str() const;

        util::sptr<Expression const> const list;
        util::sptr<Statement const> const recursion;
        util::sptr<Statement const> const succession;
        Method const raiser;
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

    struct PipeElement
        : Expression
    {
        explicit PipeElement(misc::position const& pos)
            : Expression(pos)
        {}

        std::string str() const;
    };

    struct PipeIndex
        : Expression
    {
        explicit PipeIndex(misc::position const& pos)
            : Expression(pos)
        {}

        std::string str() const;
    };

    struct PipeKey
        : Expression
    {
        explicit PipeKey(misc::position const& pos)
            : Expression(pos)
        {}

        std::string str() const;
    };

    struct PipeResult
        : Expression
    {
        explicit PipeResult(misc::position const& pos)
            : Expression(pos)
        {}

        std::string str() const;
    };

}

#endif /* __STEKIN_OUTPUT_LIST_PIPELINE_H__ */
