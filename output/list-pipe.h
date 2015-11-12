#ifndef __STEKIN_OUTPUT_LIST_PIPELINE_H__
#define __STEKIN_OUTPUT_LIST_PIPELINE_H__

#include "node-base.h"
#include "methods.h"

namespace output {

    struct PipelineContinue
        : Statement
    {
        void write(std::ostream& os) const;
        bool mayThrow() const { return false; }
    };

    struct AsyncPipeline
        : Expression
    {
        AsyncPipeline(misc::position const& pos
                    , util::sptr<Expression const> ls
                    , util::sptr<Statement const> recur
                    , util::sptr<Statement const> suc
                    , Method t)
            : Expression(pos)
            , list(std::move(ls))
            , recursion(std::move(recur))
            , succession(std::move(suc))
            , thrower(std::move(t))
        {}

        std::string str() const;
        bool mayThrow() const { return true; }

        util::sptr<Expression const> const list;
        util::sptr<Statement const> const recursion;
        util::sptr<Statement const> const succession;
        Method const thrower;
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
        bool mayThrow() const { return true; }

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
        bool mayThrow() const { return false; }
    };

    struct PipeIndex
        : Expression
    {
        explicit PipeIndex(misc::position const& pos)
            : Expression(pos)
        {}

        std::string str() const;
        bool mayThrow() const { return false; }
    };

    struct PipeKey
        : Expression
    {
        explicit PipeKey(misc::position const& pos)
            : Expression(pos)
        {}

        std::string str() const;
        bool mayThrow() const { return false; }
    };

    struct PipeResult
        : Expression
    {
        explicit PipeResult(misc::position const& pos)
            : Expression(pos)
        {}

        std::string str() const;
        bool mayThrow() const { return false; }
    };

}

#endif /* __STEKIN_OUTPUT_LIST_PIPELINE_H__ */
