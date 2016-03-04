#ifndef __STEKIN_GRAMMAR_CLAUSE_H__
#define __STEKIN_GRAMMAR_CLAUSE_H__

#include <util/pointer.h>

#include "automation-base.h"
#include "class.h"

namespace grammar {

    struct ClauseBase {
        explicit ClauseBase(int ind)
            : indent(ind)
            , _member_indent(-1)
            , _block(new Block)
        {}

        ClauseBase(ClauseBase const&) = delete;
        virtual ~ClauseBase() {}

        int const indent;

        void acceptFunc(util::sptr<Function const> func);
        virtual void acceptClass(util::sptr<Class> cls);
        virtual void acceptCtor(misc::position const& ct_pos
                              , std::vector<std::string> ct_params
                              , util::sptr<Block const> ct_bl, bool super_init
                              , std::vector<util::sptr<Expression const>> super_ctor_args);
        virtual void acceptStmt(util::sptr<Statement> stmt);
        void acceptElse(misc::position const& else_pos, util::sptr<Statement const> block);
        void acceptCatch(misc::position const& catch_pos, util::sptr<Statement const> block,
                         std::string except_name);

        virtual void deliver() = 0;
        virtual bool shrinkOn(int level) const;

        void nextToken(util::sptr<Token> const& token);
        bool tryFinish(misc::position const& pos, std::vector<util::sptr<ClauseBase>>& clauses);
        void prepareArith();

        void setMemberIndent(int level, misc::position const& pos);
    protected:
        AutomationStack _stack;
        int _member_indent;
        util::sptr<Block> _block;
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

    struct ForClause
        : ClauseBase
    {
        void deliver();

        ForClause(int indent_len
                , std::string ref
                , util::sptr<Expression const> begin
                , util::sptr<Expression const> end
                , util::sptr<Expression const> step
                , util::sref<ClauseBase> parent)
            : ClauseBase(indent_len)
            , _ref(std::move(ref))
            , _begin(std::move(begin))
            , _end(std::move(end))
            , _step(std::move(step))
            , _parent(parent)
        {}
    protected:
        std::string _ref;
        util::sptr<Expression const> _begin;
        util::sptr<Expression const> _end;
        util::sptr<Expression const> _step;
        util::sref<ClauseBase> const _parent;
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
                     , bool exprt
                     , util::sref<ClauseBase> parent)
            : ClauseBase(indent_level)
            , pos(ps)
            , name(func_name)
            , param_names(params)
            , async_param_index(async_param_idx)
            , _parent(parent)
            , _export(exprt)
        {}

        misc::position const pos;
        std::string const name;
        std::vector<std::string> const param_names;
        int const async_param_index;
    private:
        util::sref<ClauseBase> const _parent;
        bool const _export;
    };

    struct ClassClause
        : ClauseBase
    {
        ClassClause(int indent_len, misc::position const& ps, std::string cls_name
                  , util::sptr<Expression const> base_cls, bool exprt
                  , util::sref<ClauseBase> parent)
            : ClauseBase(indent_len)
            , pos(ps)
            , _class_name(std::move(cls_name))
            , _base_class(std::move(base_cls))
            , _parent(parent)
            , _export(exprt)
        {}

        void deliver();
        void acceptClass(util::sptr<Class> cls);
        void acceptStmt(util::sptr<Statement> stmt);
        void acceptCtor(misc::position const& ct_pos
                      , std::vector<std::string> ct_params
                      , util::sptr<Block const> ct_bl, bool super_init
                      , std::vector<util::sptr<Expression const>> super_ctor_args);

        misc::position const pos;
    private:
        std::string _class_name;
        util::sptr<Expression const> _base_class;
        util::sref<ClauseBase> const _parent;
        bool const _export;
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
        CatchClause(int indent_len, misc::position const& pos, util::sref<ClauseBase> parent,
                    std::string excn)
            : ClauseBase(indent_len)
            , catch_pos(pos)
            , _parent(parent)
            , _except_name(std::move(excn))
        {}

        void deliver();

        misc::position const catch_pos;
    private:
        util::sref<ClauseBase> const _parent;
        std::string _except_name;
    };

    struct BlockReceiverClause
        : ClauseBase
    {
        BlockReceiverClause(int level
                          , AutomationStack& stack
                          , misc::position const& pos
                          , AutomationBase* blockRecr)
            : ClauseBase(level)
            , _parent_stack(stack)
            , _pos(pos)
            , _blockRecr(blockRecr)
        {}

        void deliver();
        bool shrinkOn(int level) const;
    private:
        AutomationStack& _parent_stack;
        misc::position const _pos;
        AutomationBase* const _blockRecr;
    };

}

#endif /* __STEKIN_GRAMMAR_CLAUSE_H__ */
