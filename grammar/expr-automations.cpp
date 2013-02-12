#include <map>
#include <algorithm>

#include <semantic/function.h>
#include <util/arrays.h>
#include <report/errors.h>

#include "expr-automations.h"
#include "stmt-nodes.h"
#include "expr-nodes.h"
#include "function.h"

using namespace grammar;

static void checkEmptyExpr(util::sref<Expression const> expr)
{
    if (expr->empty()) {
        error::invalidEmptyExpr(expr->pos);
    }
}

PipelineAutomation::PipelineAutomation()
    : _cache_list(nullptr)
    , _cache_section(nullptr)
{
    _setFollowings({ COMMA, COLON, PROP_SEP, CLOSE_PAREN, CLOSE_BRACKET, CLOSE_BRACE });
    _actions[PIPE_SEP] = [&](AutomationStack& stack, Token const& token)
                         {
                             _pushPipeSep(stack, token);
                         };
}

void PipelineAutomation::activated(AutomationStack& stack)
{
    stack.push(util::mkptr(new ConditionalAutomation));
}

void PipelineAutomation::accepted(AutomationStack&, util::sptr<Expression const> expr)
{
    if (_cache_list.nul()) {
        _cache_list = std::move(expr);
    } else {
        _cache_section = std::move(expr);
    }
}

void PipelineAutomation::accepted(AutomationStack& stack, misc::position const& pos, Block&& block)
{
    checkEmptyExpr(*_cache_list);
    stack.reduced(util::mkptr(new BlockPipeline(pos, std::move(_cache_list), std::move(block))));
}

bool PipelineAutomation::finishOnBreak(bool sub_empty) const
{
    if ("|:" == _cache_pipe_op && sub_empty && _cache_section.nul()) {
        return true;
    }
    if (_afterOperator(sub_empty)) {
        return false;
    }
    return _previous->finishOnBreak(sub_empty && _cache_list.nul());
}

void PipelineAutomation::finish(
            ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const& pos)
{
    if ("|:" == _cache_pipe_op && _cache_section.not_nul() && _cache_section->empty()) {
        return wrapper.pushBlockReceiver(util::mkref(*this));
    }
    _reduce(stack);
    stack.top()->finish(wrapper, stack, pos);
}

void PipelineAutomation::_tryReducePipe()
{
    if (_cache_section.nul()) {
        return;
    }
    checkEmptyExpr(*_cache_list);
    checkEmptyExpr(*_cache_section);
    _cache_list.reset(new Pipeline(
                _cache_op_pos, std::move(_cache_list), _cache_pipe_op, std::move(_cache_section)));
}

bool PipelineAutomation::_reduce(AutomationStack& stack, Token const&)
{
    _reduce(stack);
    return true;
}

void PipelineAutomation::_pushPipeSep(AutomationStack& stack, Token const& token)
{
    _tryReducePipe();
    _cache_op_pos = token.pos;
    _cache_pipe_op = token.image;
    stack.push(util::mkptr(new ConditionalAutomation));
}

void PipelineAutomation::_reduce(AutomationStack& stack)
{
    _tryReducePipe();
    stack.reduced(std::move(_cache_list));
}

bool PipelineAutomation::_afterOperator(bool sub_empty) const
{
    return sub_empty && _cache_list.not_nul() && (!_cache_pipe_op.empty()) && _cache_section.nul();
}

ConditionalAutomation::ConditionalAutomation()
    : _before_if(true)
    , _before_else(true)
    , _cache_consq(nullptr)
    , _cache_pred(nullptr)
{
    _setFollowings({ PIPE_SEP, COMMA, COLON, PROP_SEP, CLOSE_PAREN, CLOSE_BRACKET, CLOSE_BRACE });
    _actions[IF] = [&](AutomationStack& stack, TypedToken const& token)
                   {
                       if (!_before_if) {
                           return error::unexpectedToken(token.pos, token.image);
                       }
                       _before_if = false;
                       activated(stack);
                   };
    _actions[ELSE] = [&](AutomationStack& stack, TypedToken const& token)
                     {
                         if (!_before_else) {
                             return error::unexpectedToken(token.pos, token.image);
                         }
                         _before_else = false;
                         activated(stack);
                     };
}

void ConditionalAutomation::activated(AutomationStack& stack)
{
    stack.push(util::mkptr(new ArithAutomation));
}

void ConditionalAutomation::accepted(AutomationStack& stack, util::sptr<Expression const> expr)
{
    if (_cache_consq.nul()) {
        _cache_consq = std::move(expr);
        return;
    }
    if (_cache_pred.nul()) {
        _cache_pred = std::move(expr);
        return;
    }

    checkEmptyExpr(*_cache_consq);
    checkEmptyExpr(*_cache_pred);
    checkEmptyExpr(*expr);
    misc::position pos(_cache_consq->pos);
    stack.reduced(util::mkptr(new grammar::Conditional(
                        pos, std::move(_cache_pred), std::move(_cache_consq), std::move(expr))));
}

bool ConditionalAutomation::finishOnBreak(bool sub_empty) const
{
    if (_before_if) {
        return _previous->finishOnBreak(sub_empty && (_cache_consq.nul() || _cache_consq->empty()));
    }
    if (_before_else) {
        return false;
    }
    return !sub_empty;
}

void ConditionalAutomation::finish(
                    ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const& pos)
{
    _forceReduce(stack);
    stack.top()->finish(wrapper, stack, pos);
}

bool ConditionalAutomation::_reduce(AutomationStack& stack, Token const&)
{
    _forceReduce(stack);
    return true;
}

void ConditionalAutomation::_forceReduce(AutomationStack& stack)
{
    if (!_before_if) {
        error::incompleteConditional(_cache_pred->pos);
    }
    stack.reduced(std::move(_cache_consq));
}

namespace {

    enum {
        UNARY_ADD, BITWISE, ADD, MUL, COMP, OR, AND, NOT, MEMBER, CALL, PLACEHOLDER
    };

    bool reducable(int stack_top, int encounter) {
        static bool const _(false), O(true);
        static bool const REDUCABLE[][PLACEHOLDER + 1] = {
            /*
              U  |  B    !=              C  H
              ~  ^     *  <     A  N     A  O   < STACK TOP
              +  &  +  /  >  O  N  O  .  L  L    \
              - <<  -  %  =  R  D  T     L  D     V ENCOUNTER
            */
            { _, O, O, O, O, O, O, _, O, O, _ }, // +-(u)
            { O, O, O, O, _, _, _, _, O, O, _ }, // |&^~ << >>
            { O, _, O, O, _, _, _, _, O, O, _ }, // +-(b)
            { O, _, _, O, _, _, _, _, O, O, _ }, // */%
            { O, O, O, O, O, _, _, _, O, O, _ }, // <>!=
            { O, O, O, O, O, O, O, O, O, O, _ }, // ||
            { O, O, O, O, O, _, O, O, O, O, _ }, // &&
            { _, O, O, O, O, O, O, O, O, O, _ }, // !
            { _, _, _, _, _, _, _, _, O, O, _ }, // .
            { _, _, _, _, _, _, _, _, O, O, _ }, // a(b)
        };
        return REDUCABLE[encounter][stack_top];
    }

    std::map<std::string, int> const UNARY_PRI_MAPPING{
        { "-", UNARY_ADD },
        { "+", UNARY_ADD },
        { "~", UNARY_ADD },
        { "*", UNARY_ADD },
        { "typeof", UNARY_ADD },
        { "!", NOT },
    };
    std::map<std::string, int> const BINARY_PRI_MAPPING{
        { "^", BITWISE },
        { "|", BITWISE },
        { "&", BITWISE },
        { "<<", BITWISE },
        { ">>", BITWISE },
        { ">>>", BITWISE },
        { "-", ADD },
        { "+", ADD },
        { "++", ADD },
        { "*", MUL },
        { "/", MUL },
        { "%", MUL },
        { "=", COMP },
        { "!=", COMP },
        { "<", COMP },
        { "<=", COMP },
        { ">", COMP },
        { ">=", COMP },
        { "&&", AND },
        { "||", OR },
        { ".", MEMBER },
    };

    struct PlaceholderOp
        : ArithAutomation::Operator
    {
        PlaceholderOp()
            : ArithAutomation::Operator(PLACEHOLDER)
        {}

        void operate(ArithAutomation::FactorStack&) const {}
    };

    struct FuncCall
        : ArithAutomation::Operator
    {
        explicit FuncCall(int args)
            : ArithAutomation::Operator(CALL)
            , args_count(args)
        {}

        int const args_count;

        void operate(ArithAutomation::FactorStack& factors) const
        {
            util::ptrarr<Expression const> args;
            for (int i = 0; i < args_count; ++i) {
                args.append(std::move(factors.back()));
                factors.pop_back();
            }
            util::sptr<Expression const> callee(std::move(factors.back()));
            factors.pop_back();
            factors.push_back(util::mkptr(new Call(std::move(callee), std::move(args))));
        }
    };

    struct LookupOp
        : ArithAutomation::Operator
    {
        LookupOp()
            : ArithAutomation::Operator(CALL)
        {}

        void operate(ArithAutomation::FactorStack& factors) const
        {
            util::sptr<Expression const> key(std::move(factors.back()));
            factors.pop_back();
            util::sptr<Expression const> collection(std::move(factors.back()));
            factors.pop_back();
            factors.push_back(util::mkptr(new Lookup(std::move(collection), std::move(key))));
        }
    };

    struct ListSliceOp
        : ArithAutomation::Operator
    {
        ListSliceOp()
            : ArithAutomation::Operator(CALL)
        {}

        void operate(ArithAutomation::FactorStack& factors) const
        {
            util::sptr<Expression const> begin(std::move(factors.back()));
            factors.pop_back();
            util::sptr<Expression const> end(std::move(factors.back()));
            factors.pop_back();
            util::sptr<Expression const> step(std::move(factors.back()));
            factors.pop_back();
            util::sptr<Expression const> list(std::move(factors.back()));
            factors.pop_back();
            factors.push_back(util::mkptr(new ListSlice(
                            std::move(list), std::move(begin), std::move(end), std::move(step))));
        }
    };

    struct PreUnaryOperator
        : ArithAutomation::Operator
    {
        PreUnaryOperator(misc::position const& ps, std::string const& o)
            : ArithAutomation::Operator(UNARY_PRI_MAPPING.find(o)->second)
            , pos(ps)
            , op(o)
        {}

        misc::position const pos;
        std::string const op;

        void operate(ArithAutomation::FactorStack& factors) const
        {
            util::sptr<Expression const> rhs(std::move(factors.back()));
            factors.pop_back();
            factors.push_back(util::mkptr(new PreUnaryOp(pos, op, std::move(rhs))));
        }
    };

    struct BinaryOperator
        : ArithAutomation::Operator
    {
        BinaryOperator(misc::position const& ps, std::string const& o)
            : ArithAutomation::Operator(BINARY_PRI_MAPPING.find(o)->second)
            , pos(ps)
            , op(o)
        {}

        misc::position const pos;
        std::string const op;

        void operate(ArithAutomation::FactorStack& factors) const
        {
            util::sptr<Expression const> rhs(std::move(factors.back()));
            factors.pop_back();
            util::sptr<Expression const> lhs(std::move(factors.back()));
            factors.pop_back();
            factors.push_back(util::mkptr(new BinaryOp(pos, std::move(lhs), op, std::move(rhs))));
        }
    };

    util::sptr<Expression const> reduce(ArithAutomation::OpStack& ops
                                      , ArithAutomation::FactorStack& factors)
    {
        while (ops.size() > 1) {
            util::sptr<ArithAutomation::Operator const> op(std::move(ops.back()));
            ops.pop_back();
            op->operate(factors);
        }
        return std::move(factors.back());
    }

    void removeLastEmpty(std::vector<util::sptr<Expression const>>& list)
    {
        if ((!list.empty()) && list.back()->empty()) {
            list.pop_back();
        }
    }

}

ArithAutomation::ArithAutomation()
    : _need_factor(true)
    , _accept_list_for_args(false)
{
    _op_stack.push_back(util::mkptr(new PlaceholderOp));
    _setFollowings({
        IF, ELSE, PIPE_SEP, COMMA, COLON, PROP_SEP, CLOSE_PAREN, CLOSE_BRACKET, CLOSE_BRACE
    });
    _actions[THIS] = [&](AutomationStack& stack, TypedToken const& token)
                     {
                         if (!_need_factor) {
                             return error::unexpectedToken(token.pos, token.image);
                         }
                         _need_factor = false;
                         stack.push(util::mkptr(new ThisPropertyAutomation(token.pos)));
                     };
    _actions[OPEN_PAREN] = [&](AutomationStack& stack, TypedToken const& token)
                           {
                               _pushOpenParen(stack, token.pos);
                           };
    _actions[OPEN_BRACKET] = [&](AutomationStack& stack, TypedToken const& token)
                             {
                                 _pushOpenBracket(stack, token.pos);
                             };
    _actions[OPEN_BRACE] = [&](AutomationStack& stack, TypedToken const& token)
                           {
                               _pushOpenBrace(stack, token.pos);
                           };
    _actions[OPERATOR] = [&](AutomationStack& stack, TypedToken const& token)
                         {
                             _pushOp(stack, token);
                         };
}

void ArithAutomation::_reduceBinaryOrPostfix(int pri)
{
    while (reducable(_op_stack.back()->pri, pri)) {
        util::sptr<ArithAutomation::Operator const> op(std::move(_op_stack.back()));
        _op_stack.pop_back();
        op->operate(_factor_stack);
    }
}

void ArithAutomation::pushFactor(
            AutomationStack& stack, util::sptr<Expression const> factor, std::string const& image)
{
    if (_need_factor) {
        _need_factor = false;
        return _factor_stack.push_back(std::move(factor));
    }
    AutomationBase::pushFactor(stack, std::move(factor), image);
}

void ArithAutomation::accepted(AutomationStack&, util::sptr<Expression const> expr)
{
    _factor_stack.push_back(std::move(expr));
}

void ArithAutomation::accepted(AutomationStack&, std::vector<util::sptr<Expression const>> list)
{
    _reduceBinaryOrPostfix(CALL);
    std::for_each(list.rbegin()
                , list.rend()
                , [&](util::sptr<Expression const>& e)
                  {
                      _factor_stack.push_back(std::move(e));
                  });
    if (_accept_list_for_args) {
        return _op_stack.push_back(util::mkptr(new FuncCall(list.size())));
    }
    if (list.size() == 1) {
        _op_stack.push_back(util::mkptr(new LookupOp));
    } else {
        _op_stack.push_back(util::mkptr(new ListSliceOp));
    }
}

bool ArithAutomation::finishOnBreak(bool sub_empty) const
{
    if (_need_factor && !_empty()) {
        return false;
    }
    return _previous->finishOnBreak(sub_empty && _empty());
}

void ArithAutomation::finish(
            ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const& pos)
{
    if (_empty()) {
        stack.reduced(util::mkptr(new EmptyExpr(pos)));
    } else {
        stack.reduced(reduce(_op_stack, _factor_stack));
    }
    stack.top()->finish(wrapper, stack, pos);
}

bool ArithAutomation::_reduce(AutomationStack& stack, Token const& token)
{
    if (_need_factor && !_empty()) {
        error::unexpectedToken(token.pos, token.image);
        return false;
    }
    if (_empty()) {
        stack.reduced(util::mkptr(new EmptyExpr(token.pos)));
    } else {
        stack.reduced(reduce(_op_stack, _factor_stack));
    }
    return true;
}

bool ArithAutomation::_empty() const
{
    return _op_stack.size() == 1 && _factor_stack.empty();
}

void ArithAutomation::_pushOp(AutomationStack& stack, Token const& token)
{
    if ("%" == token.image && _empty()) {
        return stack.replace(util::mkptr(new AsyncPlaceholderAutomation(token.pos)));
    }
    if (UNARY_PRI_MAPPING.find(token.image) != UNARY_PRI_MAPPING.end() && _need_factor) {
        return _op_stack.push_back(util::mkptr(new PreUnaryOperator(token.pos, token.image)));
    }
    if (BINARY_PRI_MAPPING.find(token.image) != BINARY_PRI_MAPPING.end() && !_need_factor) {
        _need_factor = true;
        _reduceBinaryOrPostfix(BINARY_PRI_MAPPING.find(token.image)->second);
        return _op_stack.push_back(util::mkptr(new BinaryOperator(token.pos, token.image)));
    }
    error::unexpectedToken(token.pos, token.image);
}

void ArithAutomation::_pushOpenParen(AutomationStack& stack, misc::position const&)
{
    _accept_list_for_args = true;
    if (_need_factor) {
        _need_factor = false;
        stack.push(util::mkptr(new NestedOrParamsAutomation));
    } else {
        stack.push(util::mkptr(new ExprListAutomation));
    }
}

void ArithAutomation::_pushOpenBracket(AutomationStack& stack, misc::position const&)
{
    if (_need_factor) {
        _need_factor = false;
        stack.push(util::mkptr(new ListLiteralAutomation));
    } else {
        _accept_list_for_args = false;
        stack.push(util::mkptr(new BracketedExprAutomation));
    }
}

void ArithAutomation::_pushOpenBrace(AutomationStack& stack, misc::position const& pos)
{
    if (_need_factor) {
        _need_factor = false;
        return stack.push(util::mkptr(new DictAutomation));
    }
    error::unexpectedToken(pos, "{");
}

ExprListAutomation::ExprListAutomation(TokenType closer_type)
{
    _actions[COMMA] = [&](AutomationStack& stack, Token const& token)
                      {
                          _pushComma(stack, token.pos);
                      };
    _actions[closer_type] = [&](AutomationStack& stack, Token const& token)
                            {
                                _matchClose(stack, token.pos);
                            };
}

void ExprListAutomation::activated(AutomationStack& stack)
{
    stack.push(util::mkptr(new PipelineAutomation));
}

static void checkExprListNotEmpty(std::vector<util::sptr<Expression const>> const& list)
{
    for (size_t i = 0; i < list.size(); ++i) {
        if (list[i]->empty()) {
            return error::invalidEmptyExpr(list[i]->pos);
        }
    }
}

void ExprListAutomation::_matchClose(AutomationStack& stack, misc::position const&)
{
    removeLastEmpty(_list);
    checkExprListNotEmpty(_list);
    stack.reduced(std::move(_list));
}

void ExprListAutomation::accepted(AutomationStack&, util::sptr<Expression const> expr)
{
    _list.push_back(std::move(expr));
}

void ExprListAutomation::_pushComma(AutomationStack& stack, misc::position const&)
{
    activated(stack);
}

void ListLiteralAutomation::_matchClose(AutomationStack& stack, misc::position const& pos)
{
    removeLastEmpty(_list);
    checkExprListNotEmpty(_list);
    stack.reduced(util::mkptr(new ListLiteral(pos, std::move(_list))));
}

NestedOrParamsAutomation::NestedOrParamsAutomation()
    : _wait_for_closing(true)
    , _wait_for_colon(false)
    , _lambda_ret_val(nullptr)
{
    _setFollowings({ IF, ELSE, PIPE_SEP, OPEN_PAREN, OPEN_BRACKET, OPERATOR });
    _actions[COMMA] = [&](AutomationStack& stack, TypedToken const& token)
                      {
                          _pushComma(stack, token);
                      };
    _actions[COLON] = [&](AutomationStack& stack, TypedToken const& token)
                      {
                          _pushColon(stack, token.pos);
                      };
    TokenAction matchClose([&](AutomationStack& stack, TypedToken const& token)
                           {
                               _matchCloser(stack, token);
                           });
    _actions[CLOSE_PAREN] = [&](AutomationStack& stack, TypedToken const& token)
                            {
                                if (_wait_for_closing) {
                                    _wait_for_colon = true;
                                    _wait_for_closing = false;
                                    return removeLastEmpty(_list);
                                }
                                _matchCloser(stack, token);
                            };
    _actions[CLOSE_BRACKET] = matchClose;
    _actions[CLOSE_BRACE] = matchClose;
}

void NestedOrParamsAutomation::_reduceAsNested(AutomationStack& stack, misc::position const& rp)
{
    if (_list.empty()) {
        error::invalidEmptyExpr(rp);
        return stack.reduced(util::mkptr(new EmptyExpr(rp)));
    }
    if (_list.size() != 1) {
        error::excessiveExpr(rp);
    }
    stack.reduced(std::move(_list[0]));
}

void NestedOrParamsAutomation::_matchCloser(AutomationStack& stack, TypedToken const& closer)
{
    if (_wait_for_closing) {
        return error::unexpectedToken(closer.pos, closer.image);
    }
    if (!_wait_for_colon) {
        _reduceAsLambda(stack);
    } else {
        _reduceAsNested(stack, closer.pos);
    }
    stack.top()->nextToken(stack, closer);
}

void NestedOrParamsAutomation::_pushComma(AutomationStack& stack, TypedToken const& token)
{
    if (_wait_for_closing) {
        return ExprListAutomation::_pushComma(stack, token.pos);
    }
    if (!_wait_for_colon) {
        _reduceAsLambda(stack);
    } else {
        _reduceAsNested(stack, token.pos);
    }
    stack.top()->nextToken(stack, token);
}

void NestedOrParamsAutomation::_pushColon(AutomationStack& stack, misc::position const& pos)
{
    if (!_wait_for_colon) {
        error::unexpectedToken(pos, ":");
    }
    _wait_for_colon = false;
    stack.push(util::mkptr(new PipelineAutomation));
}

bool NestedOrParamsAutomation::_reduce(AutomationStack& stack, Token const& token)
{
    _reduceAsNested(stack, token.pos);
    return true;
}

void NestedOrParamsAutomation::_reduceAsLambda(AutomationStack& stack)
{
    Block body;
    misc::position pos(_lambda_ret_val->pos);
    if (_lambda_ret_val->empty()) {
        error::invalidEmptyExpr(_lambda_ret_val->pos);
    }
    body.addStmt(util::mkptr(new Return(pos, std::move(_lambda_ret_val))));
    _reduceAsLambda(stack, pos, std::move(body));
}

void NestedOrParamsAutomation::_reduceAsLambda(
                                    AutomationStack& stack, misc::position const& pos, Block body)
{
    ParamReducingEnv env;
    int index(0);
    std::for_each(_list.begin()
                , _list.end()
                , [&](util::sptr<Expression const> const& param)
                  {
                      param->reduceAsParam(env, index++);
                  });
    if (-1 == env.asyncIndex()) {
        stack.reduced(util::mkptr(new Lambda(pos, env.params(), std::move(body))));
    } else {
        stack.reduced(util::mkptr(new RegularAsyncLambda(
                                        pos, env.params(), env.asyncIndex(), std::move(body))));
    }
}

void NestedOrParamsAutomation::accepted(AutomationStack&, util::sptr<Expression const> expr)
{
    if (_afterColon()) {
        _lambda_ret_val = std::move(expr);
    } else {
        _list.push_back(std::move(expr));
    }
}

void NestedOrParamsAutomation::accepted(
                                AutomationStack& stack, misc::position const& pos, Block&& body)
{
    _reduceAsLambda(stack, pos, std::move(body));
}

bool NestedOrParamsAutomation::finishOnBreak(bool sub_empty) const
{
    if (_afterColon() && sub_empty) {
        return true;
    }
    if (_wait_for_closing) {
        return false;
    }
    return _previous->finishOnBreak(false);
}

void NestedOrParamsAutomation::finish(
                ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const& pos)
{
    if (_afterColon()) {
        if (_lambda_ret_val->empty()) {
            wrapper.pushBlockReceiver(util::mkref(*this));
        } else {
            _reduceAsLambda(stack);
            stack.top()->finish(wrapper, stack, pos);
        }
        return;
    }
    _reduceAsNested(stack, pos);
    stack.top()->finish(wrapper, stack, pos);
}

bool NestedOrParamsAutomation::_afterColon() const
{
    return !(_wait_for_closing || _wait_for_colon);
}

void BracketedExprAutomation::_matchClose(AutomationStack& stack, misc::position const& pos)
{
    if (_list.size() > 3) {
        error::tooManySliceParts(pos);
        return stack.reduced(util::mkptr(new EmptyExpr(pos)));
    }
    if (_list.size() == 1) {
        if (_list[0]->empty()) {
            error::emptyLookupKey(_list[0]->pos);
        }
        return stack.reduced(std::move(_list));
    }
    if (_list.size() == 3 && _list[2]->empty()) {
        error::sliceStepOmitted(pos);
    }
    if (_list.size() == 2) {
        _list.push_back(util::mkptr(new EmptyExpr(pos)));
    }
    stack.reduced(std::move(_list));
}

DictAutomation::DictAutomation()
    : _wait_for_key(true)
    , _wait_for_colon(false)
    , _wait_for_comma(false)
    , _key_cache(nullptr)
{
    _actions[COMMA] = [&](AutomationStack& stack, TypedToken const& token)
                      {
                          _pushComma(stack, token.pos);
                      };
    _actions[COLON] = [&](AutomationStack& stack, TypedToken const& token)
                      {
                          _pushColon(stack, token.pos);
                      };
    _actions[PROP_SEP] = [&](AutomationStack& stack, TypedToken const& token)
                         {
                             _pushPropertySeparator(stack, token.pos);
                         };
    _actions[CLOSE_BRACE] = [&](AutomationStack& stack, TypedToken const& token)
                            {
                                _matchCloseBrace(stack, token);
                            };
}

void DictAutomation::activated(AutomationStack& stack)
{
    stack.push(util::mkptr(new PipelineAutomation));
}

void DictAutomation::_matchCloseBrace(AutomationStack& stack, Token const& closer)
{
    if (!(_wait_for_key || _key_cache->empty())) {
        error::unexpectedToken(closer.pos, closer.image);
    }
    stack.reduced(util::mkptr(new Dictionary(closer.pos, std::move(_items))));
}

void DictAutomation::_pushComma(AutomationStack& stack, misc::position const& pos)
{
    if (!_wait_for_comma) {
        return error::unexpectedToken(pos, ",");
    }
    _wait_for_comma = false;
    stack.push(util::mkptr(new PipelineAutomation));
}

void DictAutomation::accepted(AutomationStack&, util::sptr<Expression const> expr)
{
    if (_wait_for_key) {
        _key_cache = std::move(expr);
        _wait_for_colon = true;
    } else {
        if (_key_cache->empty()) {
            error::invalidEmptyExpr(_key_cache->pos);
        }
        if (expr->empty()) {
            error::invalidEmptyExpr(expr->pos);
        }
        _items.append(std::move(_key_cache), std::move(expr));
        _wait_for_comma = true;
    }
    _wait_for_key = !_wait_for_key;
}

void DictAutomation::_pushPropertySeparator(AutomationStack& stack, misc::position const& pos)
{
    _pushSeparator(stack, pos, "::");
}

void DictAutomation::_pushColon(AutomationStack& stack, misc::position const& pos)
{
    if (_pushSeparator(stack, pos, ":")) {
        _key_cache.reset(new StringLiteral(_key_cache->pos, _key_cache->reduceAsProperty()));
    }
}

bool DictAutomation::_pushSeparator(AutomationStack& stack
                                  , misc::position const& pos
                                  , std::string const& sep)
{
    if (!_wait_for_colon) {
        error::unexpectedToken(pos, sep);
        return false;
    }
    _wait_for_colon = false;
    stack.push(util::mkptr(new PipelineAutomation));
    return true;
}

AsyncPlaceholderAutomation::AsyncPlaceholderAutomation(misc::position const& ps)
    : pos(ps)
{
    static AutomationCreator const createExprList(
            [](TypedToken const&)
            {
                return util::mkptr(new ExprListAutomation);
            });
    _setFollowings({ COMMA, CLOSE_PAREN, CLOSE_BRACKET, CLOSE_BRACE });
    _setShifts({
        { OPEN_PAREN, { createExprList, false } },
    });
}

void AsyncPlaceholderAutomation::pushFactor(
            AutomationStack& stack, util::sptr<Expression const> factor, std::string const& image)
{
    stack.push(util::mkptr(new PipelineAutomation));
    stack.top()->pushFactor(stack, std::move(factor), image);
}

void AsyncPlaceholderAutomation::accepted(AutomationStack& stack, util::sptr<Expression const> expr)
{
    stack.reduced(util::mkptr(new AsyncPlaceholder(pos, std::vector<std::string>({
                            expr->reduceAsName()
                        }))));
}

void AsyncPlaceholderAutomation::accepted(
                AutomationStack& stack, std::vector<util::sptr<Expression const>> list)
{
    stack.reduced(util::mkptr(new AsyncPlaceholder(
                    pos, util::ptrarr<Expression const>(std::move(list)).mapv(
                                                     [&](util::sptr<Expression const> const& e, int)
                                                     {
                                                         return e->reduceAsName();
                                                     }))));
}

bool AsyncPlaceholderAutomation::_reduce(AutomationStack& stack, Token const&)
{
    stack.reduced(util::mkptr(new AsyncPlaceholder(pos, std::vector<std::string>())));
    return true;
}

ThisPropertyAutomation::ThisPropertyAutomation(misc::position const& ps)
    : pos(ps)
{
    _setFollowings({
        PIPE_SEP, COMMA, COLON, PROP_SEP, OPEN_PAREN, OPEN_BRACKET, CLOSE_PAREN, CLOSE_BRACKET,
        CLOSE_BRACE, OPERATOR
    });
}

void ThisPropertyAutomation::pushFactor(AutomationStack& stack
                                      , util::sptr<Expression const> factor
                                      , std::string const& image)
{
    stack.reduced(util::mkptr(new Lookup(util::mkptr(new This(pos))
                                       , util::mkptr(new StringLiteral(factor->pos, image)))));
}

bool ThisPropertyAutomation::finishOnBreak(bool) const
{
    return _previous->finishOnBreak(false);
}

void ThisPropertyAutomation::finish(
                ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const& pos)
{
    stack.reduced(util::mkptr(new This(pos)));
    stack.top()->finish(wrapper, stack, pos);
}

bool ThisPropertyAutomation::_reduce(AutomationStack& stack, Token const&)
{
    stack.reduced(util::mkptr(new This(pos)));
    return true;
}
