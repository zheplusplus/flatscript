#ifndef __STEKIN_FLOWCHECK_LIST_PIPELINE_H__
#define __STEKIN_FLOWCHECK_LIST_PIPELINE_H__

#include <vector>

#include "node-base.h"

namespace flchk {

    struct PipeBase {
        virtual ~PipeBase() {}
        virtual util::sptr<proto::PipeBase const> compile(util::sref<SymbolTable> st) const = 0;
        virtual util::sptr<PipeBase const> fold() const = 0;

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

        util::sptr<proto::PipeBase const> compile(util::sref<SymbolTable> st) const;
        util::sptr<PipeBase const> fold() const;
    };

    struct PipeFilter
        : public PipeBase
    {
        explicit PipeFilter(util::sptr<Expression const> expr)
            : PipeBase(std::move(expr))
        {}

        util::sptr<proto::PipeBase const> compile(util::sref<SymbolTable> st) const;
        util::sptr<PipeBase const> fold() const;
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

        util::sptr<proto::Expression const> compile(util::sref<SymbolTable> st) const;
        std::string typeName() const;
        util::sptr<Expression const> fold() const;

        util::sptr<Expression const> const list;
        std::vector<util::sptr<PipeBase const>> const pipeline;
    };

}

#endif /* __STEKIN_FLOWCHECK_LIST_PIPELINE_H__ */
