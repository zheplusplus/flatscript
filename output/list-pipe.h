#ifndef __STEKIN_OUTPUT_LIST_PIPELINE_H__
#define __STEKIN_OUTPUT_LIST_PIPELINE_H__

#include "node-base.h"
#include "methods.h"

namespace output {

    struct PipelineBase
        : Expression
    {
        PipelineBase(misc::position const& pos, util::sptr<Expression const> lst, util::uid id)
            : Expression(pos)
            , list(std::move(lst))
            , pipe_id(id)
        {}

        bool mayThrow() const { return true; }

        util::sptr<Expression const> const list;
        util::uid const pipe_id;
    };

    struct AsyncPipeline
        : PipelineBase
    {
        AsyncPipeline(misc::position const& pos
                    , util::sptr<Expression const> list
                    , util::sptr<Statement const> recur
                    , util::sptr<Statement const> suc
                    , util::uid pipe_id
                    , Method t)
            : PipelineBase(pos, std::move(list), pipe_id)
            , recursion(std::move(recur))
            , succession(std::move(suc))
            , thrower(std::move(t))
        {}

        std::string str() const;

        util::sptr<Statement const> const recursion;
        util::sptr<Statement const> const succession;
        Method const thrower;
    };

    struct SyncPipeline
        : PipelineBase
    {
        SyncPipeline(misc::position const& pos
                   , util::sptr<Expression const> list
                   , util::sptr<Statement const> sec
                   , util::uid pipe_id)
            : PipelineBase(pos, std::move(list), pipe_id)
            , section(std::move(sec))
        {}

        std::string str() const;

        util::sptr<Statement const> const section;
    };

    struct RootSyncPipeline
        : PipelineBase
    {
        RootSyncPipeline(misc::position const& pos
                       , util::sptr<Expression const> list
                       , util::sptr<Statement const> sec
                       , Method ext_ret
                       , util::uid pipe_id
                       , bool ret
                       , bool brk)
            : PipelineBase(pos, std::move(list), pipe_id)
            , section(std::move(sec))
            , ext_return(std::move(ext_ret))
            , has_ret(ret)
            , has_break(brk)
        {}

        std::string str() const;

        util::sptr<Statement const> const section;
        Method const ext_return;
        bool const has_ret;
        bool const has_break;
    };

    struct RangeIterationBase
        : Statement
    {
        RangeIterationBase(util::sptr<Expression const> r
                         , util::sptr<Expression const> b
                         , util::sptr<Expression const> e
                         , double s
                         , util::sptr<Statement const> sec
                         , util::uid id)
            : reference(std::move(r))
            , begin(std::move(b))
            , end(std::move(e))
            , step(s)
            , section(std::move(sec))
            , iter_id(id)
        {}

        bool mayThrow() const { return true; }

        util::sptr<Expression const> const reference;
        util::sptr<Expression const> const begin;
        util::sptr<Expression const> const end;
        double const step;
        util::sptr<Statement const> section;
        util::uid iter_id;
    };

    struct SyncRangeIteration
        : RangeIterationBase
    {
        SyncRangeIteration(util::sptr<Expression const> ref
                         , util::sptr<Expression const> begin
                         , util::sptr<Expression const> end
                         , double step
                         , util::sptr<Statement const> sec
                         , Method ext_ret
                         , util::uid iter_id
                         , bool ret
                         , bool brk)
            : RangeIterationBase(std::move(ref), std::move(begin), std::move(end)
                               , step, std::move(sec), iter_id)
            , ext_return(std::move(ext_ret))
            , has_ret(ret)
            , has_break(brk)
        {}

        void write(std::ostream& os) const;

        Method const ext_return;
        bool const has_ret;
        bool const has_break;
    };

    struct AsyncRangeIteration
        : RangeIterationBase
    {
        AsyncRangeIteration(util::sptr<Expression const> ref
                          , util::sptr<Expression const> begin
                          , util::sptr<Expression const> end
                          , double step
                          , util::sptr<Statement const> sec
                          , util::sptr<Statement const> suc
                          , util::uid iter_id)
            : RangeIterationBase(std::move(ref), std::move(begin), std::move(end)
                               , step, std::move(sec), iter_id)
            , succession(std::move(suc))
        {}

        void write(std::ostream& os) const;

        util::sptr<Statement const> const succession;
    };

    struct PipeRefExpr
        : Expression
    {
        PipeRefExpr(misc::position const& pos, util::uid id)
            : Expression(pos)
            , pipe_id(id)
        {}

        bool mayThrow() const { return false; }

        util::uid const pipe_id;
    };

    struct PipeElement
        : PipeRefExpr
    {
        PipeElement(misc::position const& pos, util::uid id)
            : PipeRefExpr(pos, id)
        {}

        std::string str() const;
    };

    struct PipeIndex
        : PipeRefExpr
    {
        PipeIndex(misc::position const& pos, util::uid id)
            : PipeRefExpr(pos, id)
        {}

        std::string str() const;
    };

    struct PipeKey
        : PipeRefExpr
    {
        PipeKey(misc::position const& pos, util::uid id)
            : PipeRefExpr(pos, id)
        {}

        std::string str() const;
    };

    struct PipeResult
        : PipeRefExpr
    {
        PipeResult(misc::position const& pos, util::uid id)
            : PipeRefExpr(pos, id)
        {}

        std::string str() const;
    };

    struct PipeBreak
        : PipeRefExpr
    {
        PipeBreak(misc::position const& pos, util::uid id)
            : PipeRefExpr(pos, id)
        {}

        std::string str() const;
    };

    struct PipeContinue
        : PipeRefExpr
    {
        PipeContinue(misc::position const& pos, util::uid id)
            : PipeRefExpr(pos, id)
        {}

        std::string str() const;
    };

}

#endif /* __STEKIN_OUTPUT_LIST_PIPELINE_H__ */
