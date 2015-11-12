#ifndef __STEKIN_GRAMMAR_CLAUSE_H__
#define __STEKIN_GRAMMAR_CLAUSE_H__

#include <util/pointer.h>
#include <misc/pos-type.h>

#include "node-base.h"
#include "automation-base.h"
#include "function.h"
#include "class.h"
#include "tokens.h"

namespace grammar {

    struct ClauseBase {
        explicit ClauseBase(int ind)
            : indent(ind)
            , _member_indent(-1)
        {}

        ClauseBase(ClauseBase const&) = delete;
        virtual ~ClauseBase() {}

        int const indent;

        void acceptFunc(util::sptr<Function const> func);
        virtual void acceptClass(util::sptr<Class> cls);
        virtual void acceptCtor(misc::position const& ct_pos
                              , std::vector<std::string> ct_params
                              , Block ct_bl, bool super_init
                              , std::vector<util::sptr<Expression const>> super_ctor_args);
        virtual void acceptStmt(util::sptr<Statement> stmt);
        void acceptElse(misc::position const& else_pos, Block block);
        void acceptCatch(misc::position const& catch_pos, Block block);

        virtual void acceptExpr(util::sptr<Expression const>) {}
        virtual void deliver() = 0;
        virtual bool shrinkOn(int level) const;

        void nextToken(util::sptr<Token> const& token);
        bool tryFinish(misc::position const& pos, std::vector<util::sptr<ClauseBase>>& clauses);
        void prepareArith();
        void prepareExport(std::vector<std::string> const& names);

        void setMemberIndent(int level, misc::position const& pos);
    protected:
        AutomationStack _stack;
        int _member_indent;
        Block _block;
    };

    struct IfClause
        : ClauseBase
    {
        void deliver();

        IfClause(int indent_len, util::sptr<Expression const> pred, util::sref<ClauseBase> parent)
            : ClauseBase(indent_len)
            , _predicate(std::move(pred))
            , _parent(parent)
        {}
    protected:
        util::sptr<Expression const> _predicate;
        util::sref<ClauseBase> const _parent;
    };

    struct ElseClause
        : ClauseBase
    {
        ElseClause(int indent_len, misc::position const& pos, util::sref<ClauseBase> parent)
            : ClauseBase(indent_len)
            , else_pos(pos)
            , _parent(parent)
        {}

        void deliver();

        misc::position const else_pos;
    private:
        util::sref<ClauseBase> const _parent;
    };

    struct IfnotClause
        : IfClause
    {
        void deliver();

        IfnotClause(int indent_len
                  , util::sptr<Expression const> pred
                  , util::sref<ClauseBase> parent)
            : IfClause(indent_len, std::move(pred), parent)
        {}
    };

    struct FunctionClause
        : ClauseBase
    {
        void deliver();

        FunctionClause(int indent_level
                     , misc::position const& ps
                     , std::string const& func_name
                     , std::vector<std::string> const& params
                     , int async_param_idx
                     , util::sref<ClauseBase> parent)
            : ClauseBase(indent_level)
            , pos(ps)
            , name(func_name)
            , param_names(params)
            , async_param_index(async_param_idx)
            , _parent(parent)
        {}

        misc::position const pos;
        std::string const name;
        std::vector<std::string> const param_names;
        int const async_param_index;
    private:
        util::sref<ClauseBase> const _parent;
    };

    struct ClassClause
        : ClauseBase
    {
        ClassClause(int indent_len, misc::position const& ps, std::string cls_name
                  , util::sptr<Expression const> base_cls, util::sref<ClauseBase> parent)
            : ClauseBase(indent_len)
            , pos(ps)
            , _class_name(std::move(cls_name))
            , _base_class(std::move(base_cls))
            , _parent(parent)
        {}

        void deliver();
        void acceptClass(util::sptr<Class> cls);
        void acceptStmt(util::sptr<Statement> stmt);
        void acceptCtor(misc::position const& ct_pos
                      , std::vector<std::string> ct_params
                      , Block ct_bl, bool super_init
                      , std::vector<util::sptr<Expression const>> super_ctor_args);

        misc::position const pos;
    private:
        std::string _class_name;
        util::sptr<Expression const> _base_class;
        util::sref<ClauseBase> const _parent;
    };

    struct CtorClause
        : ClauseBase
    {
        CtorClause(int indent_len, misc::position const& ps, std::vector<std::string> params
                 , bool super_init, std::vector<util::sptr<Expression const>> super_ctor_args
                 , util::sref<ClauseBase> parent)
            : ClauseBase(indent_len)
            , pos(ps)
            , _params(std::move(params))
            , _super_init(super_init)
            , _super_ctor_args(std::move(super_ctor_args))
            , _parent(parent)
        {}

        void deliver();

        misc::position const pos;
    private:
        std::vector<std::string> _params;
        bool _super_init;
        std::vector<util::sptr<Expression const>> _super_ctor_args;
        util::sref<ClauseBase> const _parent;
    };

    struct TryClause
        : ClauseBase
    {
        TryClause(int indent_len, misc::position const& pos, util::sref<ClauseBase> parent)
            : ClauseBase(indent_len)
            , _try_pos(pos)
            , _parent(parent)
        {}

        void deliver();
    private:
        misc::position const _try_pos;
        util::sref<ClauseBase> const _parent;
    };

    struct CatchClause
        : ClauseBase
    {
        CatchClause(int indent_len, misc::position const& pos, util::sref<ClauseBase> parent)
            : ClauseBase(indent_len)
            , catch_pos(pos)
            , _parent(parent)
        {}

        void deliver();

        misc::position const catch_pos;
    private:
        util::sref<ClauseBase> const _parent;
    };

    struct BlockReceiverClause
        : ClauseBase
    {
        BlockReceiverClause(int level
                          , AutomationStack& stack
                          , misc::position const& pos
                          , AutomationBase* blockRecr)
            : ClauseBase(level)
            , _stack(stack)
            , _pos(pos)
            , _blockRecr(blockRecr)
        {}

        void deliver();
        bool shrinkOn(int level) const;
    private:
        AutomationStack& _stack;
        misc::position const _pos;
        AutomationBase* const _blockRecr;
    };

}

#endif /* __STEKIN_GRAMMAR_CLAUSE_H__ */
