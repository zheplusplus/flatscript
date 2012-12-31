#ifndef __STEKIN_GRAMMAR_AUTOMATION_BASE_H__
#define __STEKIN_GRAMMAR_AUTOMATION_BASE_H__

#include <vector>

#include <util/pointer.h>
#include <misc/pos-type.h>

#include "fwd-decl.h"

namespace grammar {

    struct AutomationStack;
    struct ClauseStackWrapper;

    struct Token {
        misc::position const pos;
        std::string const image;

        Token(misc::position const& ps, std::string const& img)
            : pos(ps)
            , image(img)
        {}

        virtual void act(AutomationStack& stack) = 0;

        Token(Token const&) = delete;
    };

    struct AutomationBase {
        virtual ~AutomationBase() {}

        virtual void activated(AutomationStack&) {};
        virtual void resumed(AutomationStack&) {};

        virtual void pushOp(AutomationStack& stack, Token const& token);
        virtual void pushFactor(AutomationStack& stack
                              , util::sptr<Expression const> factor
                              , std::string const& image);
        virtual void pushOpenParen(AutomationStack& stack, misc::position const& pos);
        virtual void pushOpenBracket(AutomationStack& stack, misc::position const& pos);
        virtual void pushOpenBrace(AutomationStack& stack, misc::position const& pos);
        virtual void matchClosing(AutomationStack& stack, Token const& closer);
        virtual void pushColon(AutomationStack& stack, misc::position const& pos);
        virtual void pushPropertySeparator(AutomationStack& stack, misc::position const& pos);
        virtual void pushComma(AutomationStack& stack, misc::position const& pos);

        virtual void accepted(AutomationStack&, util::sptr<Expression const> expr) = 0;
        virtual void accepted(AutomationStack&, std::vector<util::sptr<Expression const>>) {}
        virtual void accepted(AutomationStack&, misc::position const&, Block&&) {}
        virtual bool finishOnBreak(bool sub_empty) const = 0;
        virtual void finish(ClauseStackWrapper& clauses
                          , AutomationStack& stack
                          , misc::position const& pos) = 0;

        AutomationBase()
            : _previous(nullptr)
        {}

        AutomationBase(AutomationBase const&) = delete;
    protected:
        util::sref<AutomationBase const> _previous;
    public:
        void setPrevious(util::sref<AutomationBase const> previous);
    };

    struct AutomationStack {
        void push(util::sptr<AutomationBase> automation);
        util::sref<AutomationBase> top() const;
        void reduced(util::sptr<Expression const> expr);
        void reduced(std::vector<util::sptr<Expression const>> list);
        void pop();
        bool empty() const;
    private:
        std::vector<util::sptr<AutomationBase>> _stack;
    };

    struct ClauseBase {
        explicit ClauseBase(int ind)
            : indent(ind)
            , _member_indent(-1)
        {}

        ClauseBase(ClauseBase const&) = delete;
        virtual ~ClauseBase() {}

        int const indent;

        virtual void acceptFunc(util::sptr<Function const> func) = 0;
        virtual void acceptStmt(util::sptr<Statement> stmt) = 0;
        virtual void acceptExpr(util::sptr<Expression const>) {}
        virtual void acceptElse(misc::position const& else_pos);
        virtual void deliver() = 0;
        virtual bool shrinkOn(int level) const;

        void nextToken(util::sptr<Token> const& token);
        bool tryFinish(misc::position const& pos, std::vector<util::sptr<ClauseBase>>& clauses);
        void prepareArith();
        void prepareReturn();
        void prepareExport(std::vector<std::string> const& names);

        void setMemberIndent(int level, misc::position const& pos);
    protected:
        AutomationStack _stack;
        int _member_indent;
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
    private:
        int const _last_indent;
        AutomationStack& _stack;
        misc::position const _pos;
        std::vector<util::sptr<ClauseBase>>& _clauses;
    };

}

#endif /* __STEKIN_GRAMMAR_AUTOMATION_BASE_H__ */
