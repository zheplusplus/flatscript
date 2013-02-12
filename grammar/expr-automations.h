#ifndef __STEKIN_GRAMMAR_EXPRESSION_AUTOMATIONS_H__
#define __STEKIN_GRAMMAR_EXPRESSION_AUTOMATIONS_H__

#include "automation-base.h"
#include "expr-nodes.h"

namespace grammar {

    struct PipelineAutomation
        : AutomationBase
    {
        PipelineAutomation();

        void activated(AutomationStack& stack);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        void accepted(AutomationStack& stack, misc::position const& pos, Block&& block);
        bool finishOnBreak(bool sub_empty) const;
        void finish(ClauseStackWrapper&, AutomationStack&, misc::position const&);
    protected:
        bool _reduce(AutomationStack& stack, Token const&);
    private:
        void _pushPipeSep(AutomationStack& stack, Token const& token);
        void _tryReducePipe();
        void _reduce(AutomationStack& stack);
        bool _afterOperator(bool sub_empty) const;

        util::sptr<Expression const> _cache_list;
        util::sptr<Expression const> _cache_section;
        misc::position _cache_op_pos;
        std::string _cache_pipe_op;
    };

    struct ConditionalAutomation
        : AutomationBase
    {
        ConditionalAutomation();

        void activated(AutomationStack& stack);
        void accepted(AutomationStack& stack, util::sptr<Expression const> expr);
        bool finishOnBreak(bool sub_empty) const;
        void finish(ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const& pos);
    protected:
        bool _reduce(AutomationStack& stack, Token const&);
    private:
        void _forceReduce(AutomationStack& stack);

        bool _before_if;
        bool _before_else;
        util::sptr<Expression const> _cache_consq;
        util::sptr<Expression const> _cache_pred;
    };

    struct ArithAutomation
        : AutomationBase
    {
        void pushFactor(AutomationStack& stack
                      , util::sptr<Expression const> factor
                      , std::string const& image);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        void accepted(AutomationStack&, std::vector<util::sptr<Expression const>> list);
        bool finishOnBreak(bool sub_empty) const;
        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);

        ArithAutomation();

        struct Operator;
        typedef std::vector<util::sptr<Operator const>> OpStack;
        typedef std::vector<util::sptr<Expression const>> FactorStack;

        struct Operator {
            virtual ~Operator() {}

            Operator(int p)
                : pri(p)
            {}

            virtual void operate(FactorStack& factors) const = 0;

            int const pri;
        };
    private:
        bool _reduce(AutomationStack& stack, Token const& follower);
        bool _empty() const;
        void _pushOp(AutomationStack& stack, Token const& token);
        void _pushOpenParen(AutomationStack& stack, misc::position const& pos);
        void _pushOpenBracket(AutomationStack& stack, misc::position const& pos);
        void _pushOpenBrace(AutomationStack& stack, misc::position const& pos);
        void _pushFactor(util::sptr<Expression const> factor, Token const& token);
        void _reduceBinaryOrPostfix(int pri);

        bool _need_factor;
        bool _accept_list_for_args;
        std::string _reduce_close_paren;
        FactorStack _factor_stack;
        OpStack _op_stack;
    };

    struct ExprListAutomation
        : AutomationBase
    {
        explicit ExprListAutomation(TokenType closer_type);

        ExprListAutomation()
            : ExprListAutomation(CLOSE_PAREN)
        {}

        void activated(AutomationStack& stack);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        bool finishOnBreak(bool) const { return false; }
        void finish(ClauseStackWrapper&, AutomationStack&, misc::position const&) {}
    protected:
        std::vector<util::sptr<Expression const>> _list;
        void _pushComma(AutomationStack& stack, misc::position const& pos);
        virtual void _matchClose(AutomationStack& stack, misc::position const& pos);
    };

    struct ListLiteralAutomation
        : ExprListAutomation
    {
        ListLiteralAutomation()
            : ExprListAutomation(CLOSE_BRACKET)
        {}
    private:
        void _matchClose(AutomationStack& stack, misc::position const& pos);
    };

    struct NestedOrParamsAutomation
        : ExprListAutomation
    {
        NestedOrParamsAutomation();

        void accepted(AutomationStack& stack, util::sptr<Expression const> expr);
        void accepted(AutomationStack& stack, misc::position const& pos, Block&& block);
        bool finishOnBreak(bool sub_empty) const;
        void finish(ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const& pos);
    private:
        void _pushColon(AutomationStack& stack, misc::position const& pos);
        void _pushComma(AutomationStack& stack, TypedToken const& token);
        void _matchCloser(AutomationStack&, TypedToken const& closer);
        bool _reduce(AutomationStack& stack, Token const& token);
        void _reduceAsNested(AutomationStack& stack, misc::position const& rp);
        void _reduceAsLambda(AutomationStack& stack);
        void _reduceAsLambda(AutomationStack& stack, misc::position const& pos, Block body);
        bool _afterColon() const;

        bool _wait_for_closing;
        bool _wait_for_colon;
        util::sptr<Expression const> _lambda_ret_val;
    };

    struct BracketedExprAutomation
        : ExprListAutomation
    {
        BracketedExprAutomation()
            : ExprListAutomation(CLOSE_BRACKET)
        {}
    private:
        void _matchClose(AutomationStack& stack, misc::position const& pos);
    };

    struct DictAutomation
        : AutomationBase
    {
        DictAutomation();

        void activated(AutomationStack& stack);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        bool finishOnBreak(bool) const { return false; }
        void finish(ClauseStackWrapper&, AutomationStack&, misc::position const&) {}
    private:
        void _pushComma(AutomationStack& stack, misc::position const& pos);
        void _pushColon(AutomationStack& stack, misc::position const& pos);
        void _pushPropertySeparator(AutomationStack& stack, misc::position const& pos);
        bool _pushSeparator(AutomationStack& stack
                          , misc::position const& pos
                          , std::string const& sep);
        void _matchCloseBrace(AutomationStack& stack, Token const& closer);

        bool _wait_for_key;
        bool _wait_for_colon;
        bool _wait_for_comma;
        util::sptr<Expression const> _key_cache;
        util::ptrkvarr<Expression const> _items;
    };

    struct AsyncPlaceholderAutomation
        : AutomationBase
    {
        explicit AsyncPlaceholderAutomation(misc::position const& ps);

        void pushFactor(AutomationStack& stack
                      , util::sptr<Expression const> factor
                      , std::string const& image);
        void accepted(AutomationStack& stack, util::sptr<Expression const> expr);
        void accepted(AutomationStack& stack, std::vector<util::sptr<Expression const>> list);
        bool finishOnBreak(bool) const { return false; }
        void finish(ClauseStackWrapper&, AutomationStack&, misc::position const&) {}

        misc::position const pos;
    private:
        bool _reduce(AutomationStack& stack, Token const&);
    };

    struct ThisPropertyAutomation
        : AutomationBase
    {
        explicit ThisPropertyAutomation(misc::position const& ps);

        void pushFactor(AutomationStack& stack
                      , util::sptr<Expression const> factor
                      , std::string const& image);
        void accepted(AutomationStack&, util::sptr<Expression const>) {}
        bool finishOnBreak(bool sub_empty) const;
        void finish(ClauseStackWrapper& clauses, AutomationStack& stack, misc::position const& pos);

        misc::position const pos;
    protected:
        bool _reduce(AutomationStack& stack, Token const&);
    };

}

#endif /* __STEKIN_GRAMMAR_EXPRESSION_AUTOMATIONS_H__ */
