#ifndef __STEKIN_GRAMMAR_AUTOMATION_BASE_H__
#define __STEKIN_GRAMMAR_AUTOMATION_BASE_H__

#include <set>
#include <map>
#include <functional>

#include <util/arrays.h>
#include <misc/pos-type.h>

#include "tokens.h"

namespace grammar {

    struct ClauseStackWrapper;

    typedef std::function<void (AutomationStack&, TypedToken const&)> TokenAction;
    typedef std::function<util::sptr<AutomationBase> (TypedToken const&)> AutomationCreator;

    struct AutomationBase {
        AutomationBase(AutomationBase const&) = delete;
        AutomationBase();
        virtual ~AutomationBase() {}

        virtual void activated(AutomationStack&) {};
        virtual void resumed(AutomationStack&) {};

        void nextToken(AutomationStack& stack, TypedToken const& token);
        static void discardToken(AutomationStack&, TypedToken const& token);

        virtual void pushFactor(AutomationStack& stack
                              , util::sptr<Expression const> factor
                              , std::string const& image);

        virtual void accepted(AutomationStack&, util::sptr<Expression const> expr) = 0;
        virtual void accepted(AutomationStack&, std::vector<util::sptr<Expression const>>) {}
        virtual void accepted(AutomationStack&, misc::position const&, Block&&) {}
        virtual bool finishOnBreak(bool sub_empty) const = 0;
        virtual void finish(ClauseStackWrapper& clauses
                          , AutomationStack& stack
                          , misc::position const& pos) = 0;
    protected:
        util::sref<AutomationBase const> _previous;
        TokenAction _actions[TOKEN_TYPE_COUNT];

        void _setFollowings(std::set<TokenType> types);
        void _setShifts(std::map<TokenType, std::pair<AutomationCreator, bool>> types);

        virtual bool _reduce(AutomationStack&, Token const&) { return false; }
    public:
        void setPrevious(util::sref<AutomationBase const> previous);
    };

    struct AutomationStack {
        void push(util::sptr<AutomationBase> automation);
        void replace(util::sptr<AutomationBase> automation);
        util::sref<AutomationBase> top() const;
        void reduced(util::sptr<Expression const> expr);
        void reduced(std::vector<util::sptr<Expression const>> list);
        void pop();
        bool empty() const;
    private:
        std::vector<util::sptr<AutomationBase>> _stack;
    };

    struct ClauseStackWrapper {
        ClauseStackWrapper(int indent
                         , AutomationStack& stack
                         , misc::position const pos
                         , std::vector<util::sptr<ClauseBase>>& clauses)
            : _last_indent(indent)
            , _stack(stack)
            , _pos(pos)
            , _clauses(clauses)
        {}

        void pushBlockReceiver(util::sref<AutomationBase> blockRecr);
        void pushIfClause(util::sptr<Expression const> predicate);
        void pushElseClause(misc::position const& else_pos);
        void pushFuncClause(misc::position const& pos
                          , std::string name
                          , std::vector<std::string> const& params
                          , int async_param_index);
    private:
        int const _last_indent;
        AutomationStack& _stack;
        misc::position const _pos;
        std::vector<util::sptr<ClauseBase>>& _clauses;
    };

}

#endif /* __STEKIN_GRAMMAR_AUTOMATION_BASE_H__ */
