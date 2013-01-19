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

void PipelineAutomation::activated(AutomationStack& stack)
{
    stack.push(util::mkptr(new ArithAutomation));
}

void PipelineAutomation::pushPipeSep(AutomationStack& stack, Token const& token)
{
    _tryReducePipe();
    _cache_op_pos = token.pos;
    _cache_pipe_op = token.image;
    stack.push(util::mkptr(new ArithAutomation));
}

void PipelineAutomation::matchClosing(AutomationStack& stack, Token const& closer)
{
    _reduce(stack);
    stack.top()->matchClosing(stack, closer);
}

void PipelineAutomation::pushColon(AutomationStack& stack, misc::position const& pos)
{
    _reduce(stack);
    stack.top()->pushColon(stack, pos);
}

void PipelineAutomation::pushPropertySeparator(AutomationStack& stack, misc::position const& pos)
{
    _reduce(stack);
    stack.top()->pushPropertySeparator(stack, pos);
}

void PipelineAutomation::pushComma(AutomationStack& stack, misc::position const& pos)
{
    _reduce(stack);
    stack.top()->pushComma(stack, pos);
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
        wrapper.pushBlockReceiver(util::mkref(*this));
        return;
    }
    _reduce(stack);
    stack.top()->finish(wrapper, stack, pos);
}

void PipelineAutomation::_tryReducePipe()
{
    if (_cache_section.nul()) {
        return;
    }
    if (_cache_list->empty()) {
        error::invalidEmptyExpr(_cache_list->pos);
    }
    if (_cache_section->empty()) {
        error::invalidEmptyExpr(_cache_section->pos);
    }
    _cache_list.reset(new Pipeline(
                _cache_op_pos, std::move(_cache_list), _cache_pipe_op, std::move(_cache_section)));
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
}

void ArithAutomation::_reduceBinaryOrPostfix(int pri)
{
    while (reducable(_op_stack.back()->pri, pri)) {
        util::sptr<ArithAutomation::Operator const> op(std::move(_op_stack.back()));
        _op_stack.pop_back();
        op->operate(_factor_stack);
    }
}

void ArithAutomation::pushOp(AutomationStack& stack, Token const& token)
{
    if ("%" == token.image && _empty()) {
        stack.replace(util::mkptr(new AsyncPlaceholderAutomation(token.pos)));
        return;
    }
    if (UNARY_PRI_MAPPING.find(token.image) != UNARY_PRI_MAPPING.end() && _need_factor) {
        _op_stack.push_back(util::mkptr(new PreUnaryOperator(token.pos, token.image)));
        return;
    }
    if (BINARY_PRI_MAPPING.find(token.image) != BINARY_PRI_MAPPING.end() && !_need_factor) {
        _reduceBinaryOrPostfix(BINARY_PRI_MAPPING.find(token.image)->second);
        _op_stack.push_back(util::mkptr(new BinaryOperator(token.pos, token.image)));
        _need_factor = true;
        return;
    }
    error::unexpectedToken(token.pos, token.image);
}

void ArithAutomation::pushPipeSep(AutomationStack& stack, Token const& token)
{
    if (_reduceIfPossible(stack, token.pos, token.image)) {
        stack.top()->pushPipeSep(stack, token);
    }
}

void ArithAutomation::pushFactor(
            AutomationStack& stack, util::sptr<Expression const> factor, std::string const& image)
{
    if (_need_factor) {
        _factor_stack.push_back(std::move(factor));
        _need_factor = false;
        return;
    }
    AutomationBase::pushFactor(stack, std::move(factor), image);
}

void ArithAutomation::pushThis(AutomationStack& stack, misc::position const& pos)
{
    if (!_need_factor) {
        error::unexpectedToken(pos, "@");
        return;
    }
    _need_factor = false;
    stack.push(util::mkptr(new ThisPropertyAutomation(pos)));
}

void ArithAutomation::pushOpenParen(AutomationStack& stack, misc::position const&)
{
    _accept_list_for_args = true;
    if (_need_factor) {
        _need_factor = false;
        stack.push(util::mkptr(new NestedOrParamsAutomation));
    } else {
        stack.push(util::mkptr(new ExprListAutomation));
    }
}

void ArithAutomation::pushOpenBracket(AutomationStack& stack, misc::position const&)
{
    if (_need_factor) {
        _need_factor = false;
        stack.push(util::mkptr(new ListLiteralAutomation));
    } else {
        _accept_list_for_args = false;
        stack.push(util::mkptr(new BracketedExprAutomation));
    }
}

void ArithAutomation::pushOpenBrace(AutomationStack& stack, misc::position const& pos)
{
    if (_need_factor) {
        _need_factor = false;
        stack.push(util::mkptr(new DictAutomation));
        return;
    }
    error::unexpectedToken(pos, "{");
}

bool ArithAutomation::_reduceIfPossible(
                AutomationStack& stack, misc::position const& pos, std::string const& image)
{
    if (_need_factor && !_empty()) {
        error::unexpectedToken(pos, image);
        return false;
    }
    if (_empty()) {
        stack.reduced(util::mkptr(new EmptyExpr(pos)));
    } else {
        stack.reduced(reduce(_op_stack, _factor_stack));
    }
    return true;
}

void ArithAutomation::matchClosing(AutomationStack& stack, Token const& closer)
{
    if (_reduceIfPossible(stack, closer.pos, closer.image)) {
        stack.top()->matchClosing(stack, closer);
    }
}

void ArithAutomation::pushColon(AutomationStack& stack, misc::position const& pos)
{
    if (_reduceIfPossible(stack, pos, ":")) {
        stack.top()->pushColon(stack, pos);
    }
}

void ArithAutomation::pushPropertySeparator(AutomationStack& stack, misc::position const& pos)
{
    if (_reduceIfPossible(stack, pos, "::")) {
        stack.top()->pushPropertySeparator(stack, pos);
    }
}

void ArithAutomation::pushComma(AutomationStack& stack, misc::position const& pos)
{
    if (_reduceIfPossible(stack, pos, ",")) {
        stack.top()->pushComma(stack, pos);
    }
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
        _op_stack.push_back(util::mkptr(new FuncCall(list.size())));
        return;
    }
    if (list.size() == 1) {
        _op_stack.push_back(util::mkptr(new LookupOp));
    } else {
        _op_stack.push_back(util::mkptr(new ListSliceOp));
    }
}

bool ArithAutomation::finishOnBreak(bool) const
{
    if (_need_factor && !_empty()) {
        return false;
    }
    return _previous->finishOnBreak(_empty());
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

bool ArithAutomation::_empty() const
{
    return _op_stack.size() == 1 && _factor_stack.empty();
}

void ExprListAutomation::activated(AutomationStack& stack)
{
    stack.push(util::mkptr(new PipelineAutomation));
}

static void checkExprListNotEmpty(std::vector<util::sptr<Expression const>> const& list)
{
    for (size_t i = 0; i < list.size(); ++i) {
        if (list[i]->empty()) {
            error::invalidEmptyExpr(list[i]->pos);
            return;
        }
    }
}

void ExprListAutomation::matchClosing(AutomationStack& stack, Token const& closer)
{
    if (")" == closer.image) {
        removeLastEmpty(_list);
        checkExprListNotEmpty(_list);
        stack.reduced(std::move(_list));
        return;
    }
    error::unexpectedToken(closer.pos, closer.image);
}

void ExprListAutomation::pushComma(AutomationStack& stack, misc::position const&)
{
    activated(stack);
}

void ExprListAutomation::accepted(AutomationStack&, util::sptr<Expression const> expr)
{
    _list.push_back(std::move(expr));
}

void ListLiteralAutomation::matchClosing(AutomationStack& stack, Token const& closer)
{
    if ("]" == closer.image) {
        removeLastEmpty(_list);
        checkExprListNotEmpty(_list);
        stack.reduced(util::mkptr(new ListLiteral(closer.pos, std::move(_list))));
        return;
    }
    error::unexpectedToken(closer.pos, closer.image);
}

void NestedOrParamsAutomation::_reduceAsNested(AutomationStack& stack, misc::position const& rp)
{
    if (_list.empty()) {
        error::invalidEmptyExpr(rp);
        stack.reduced(util::mkptr(new EmptyExpr(rp)));
        return;
    }
    if (_list.size() != 1) {
        error::excessiveExpr(rp);
    }
    stack.reduced(std::move(_list[0]));
}

void NestedOrParamsAutomation::pushOp(AutomationStack& stack, Token const& token)
{
    _reduceAsNested(stack, token.pos);
    stack.top()->pushOp(stack, token);
}

void NestedOrParamsAutomation::pushPipeSep(AutomationStack& stack, Token const& token)
{
    _reduceAsNested(stack, token.pos);
    stack.top()->pushPipeSep(stack, token);
}

void NestedOrParamsAutomation::pushOpenParen(AutomationStack& stack, misc::position const& pos)
{
    _reduceAsNested(stack, pos);
    stack.top()->pushOpenParen(stack, pos);
}

void NestedOrParamsAutomation::pushOpenBracket(AutomationStack& stack, misc::position const& pos)
{
    _reduceAsNested(stack, pos);
    stack.top()->pushOpenBracket(stack, pos);
}

void NestedOrParamsAutomation::matchClosing(AutomationStack& stack, Token const& closer)
{
    if (!_wait_for_closing) {
        if (!_wait_for_colon) {
            _reduceAsLambda(stack);
        } else {
            _reduceAsNested(stack, closer.pos);
        }
        stack.top()->matchClosing(stack, closer);
        return;
    }
    if (")" != closer.image || _wait_for_colon) {
        error::unexpectedToken(closer.pos, closer.image);
        return;
    }
    removeLastEmpty(_list);
    _wait_for_colon = true;
    _wait_for_closing = false;
}

void NestedOrParamsAutomation::pushColon(AutomationStack& stack, misc::position const& pos)
{
    if (!_wait_for_colon) {
        error::unexpectedToken(pos, ":");
    }
    _wait_for_colon = false;
    stack.push(util::mkptr(new PipelineAutomation));
}

void NestedOrParamsAutomation::pushComma(AutomationStack& stack, misc::position const& pos)
{
    if (_wait_for_closing) {
        ExprListAutomation::pushComma(stack, pos);
        return;
    }
    if (!_wait_for_colon) {
        _reduceAsLambda(stack);
    } else {
        _reduceAsNested(stack, pos);
    }
    stack.top()->pushComma(stack, pos);
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
    std::vector<std::string> param_names;
    std::for_each(_list.begin()
                , _list.end()
                , [&](util::sptr<Expression const> const& p)
                  {
                      param_names.push_back(p->reduceAsName());
                  });
    stack.reduced(util::mkptr(new Lambda(pos, param_names, std::move(body))));
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

void BracketedExprAutomation::matchClosing(AutomationStack& stack, Token const& closer)
{
    if ("]" != closer.image) {
        error::unexpectedToken(closer.pos, closer.image);
        return;
    }
    if (_list.size() > 3) {
        error::tooManySliceParts(closer.pos);
        stack.reduced(util::mkptr(new EmptyExpr(closer.pos)));
        return;
    }
    if (_list.size() == 1) {
        if (_list[0]->empty()) {
            error::emptyLookupKey(_list[0]->pos);
        }
        stack.reduced(std::move(_list));
        return;
    }
    if (_list.size() == 3 && _list[2]->empty()) {
        error::sliceStepOmitted(closer.pos);
    }
    if (_list.size() == 2) {
        _list.push_back(util::mkptr(new EmptyExpr(closer.pos)));
    }
    stack.reduced(std::move(_list));
}

void DictAutomation::activated(AutomationStack& stack)
{
    stack.push(util::mkptr(new PipelineAutomation));
}

void DictAutomation::matchClosing(AutomationStack& stack, Token const& closer)
{
    if ("}" != closer.image) {
        error::unexpectedToken(closer.pos, closer.image);
        return;
    }
    if (!(_wait_for_key || _key_cache->empty())) {
        error::unexpectedToken(closer.pos, closer.image);
    }
    stack.reduced(util::mkptr(new Dictionary(closer.pos, std::move(_items))));
}

void DictAutomation::pushComma(AutomationStack& stack, misc::position const& pos)
{
    if (!_wait_for_comma) {
        error::unexpectedToken(pos, ",");
        return;
    }
    _wait_for_comma = false;
    stack.push(util::mkptr(new PipelineAutomation));
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

void DictAutomation::pushColon(AutomationStack& stack, misc::position const& pos)
{
    if (_pushSeparator(stack, pos, ":")) {
        _key_cache.reset(new StringLiteral(_key_cache->pos, _key_cache->reduceAsProperty()));
    }
}

void DictAutomation::pushPropertySeparator(AutomationStack& stack, misc::position const& pos)
{
    _pushSeparator(stack, pos, "::");
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

void AsyncPlaceholderAutomation::pushFactor(
            AutomationStack& stack, util::sptr<Expression const> factor, std::string const& image)
{
    stack.push(util::mkptr(new PipelineAutomation));
    stack.top()->pushFactor(stack, std::move(factor), image);
}

void AsyncPlaceholderAutomation::pushOpenParen(AutomationStack& stack, misc::position const&)
{
    stack.push(util::mkptr(new ExprListAutomation));
}

void AsyncPlaceholderAutomation::matchClosing(AutomationStack& stack, Token const& closer)
{
    stack.reduced(util::mkptr(new AsyncPlaceholder(pos, std::vector<std::string>())));
    stack.top()->matchClosing(stack, closer);
}

void AsyncPlaceholderAutomation::pushComma(AutomationStack& stack, misc::position const& pos)
{
    stack.reduced(util::mkptr(new AsyncPlaceholder(pos, std::vector<std::string>())));
    stack.top()->pushComma(stack, pos);
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
    stack.reduced(util::mkptr(new AsyncPlaceholder(pos
                                                 , util::ptrarr<Expression const>(std::move(list))
                                                                            .mapv(
                                                     [&](util::sptr<Expression const> const& e, int)
                                                     {
                                                         return e->reduceAsName();
                                                     }))));
}

void ThisPropertyAutomation::pushOp(AutomationStack& stack, Token const& token)
{
    stack.reduced(util::mkptr(new This(this->pos)));
    stack.top()->pushOp(stack, token);
}

void ThisPropertyAutomation::pushPipeSep(AutomationStack& stack, Token const& token)
{
    stack.reduced(util::mkptr(new This(this->pos)));
    stack.top()->pushPipeSep(stack, token);
}

void ThisPropertyAutomation::pushFactor(AutomationStack& stack
                                      , util::sptr<Expression const> factor
                                      , std::string const& image)
{
    stack.reduced(util::mkptr(new Lookup(util::mkptr(new This(pos))
                                       , util::mkptr(new StringLiteral(factor->pos, image)))));
}

void ThisPropertyAutomation::pushOpenParen(AutomationStack& stack, misc::position const& pos)
{
    stack.reduced(util::mkptr(new This(this->pos)));
    stack.top()->pushOpenParen(stack, pos);
}

void ThisPropertyAutomation::pushOpenBracket(AutomationStack& stack, misc::position const& pos)
{
    stack.reduced(util::mkptr(new This(this->pos)));
    stack.top()->pushOpenBracket(stack, pos);
}

void ThisPropertyAutomation::matchClosing(AutomationStack& stack, Token const& closer)
{
    stack.reduced(util::mkptr(new This(this->pos)));
    stack.top()->matchClosing(stack, closer);
}

void ThisPropertyAutomation::pushComma(AutomationStack& stack, misc::position const& pos)
{
    stack.reduced(util::mkptr(new This(this->pos)));
    stack.top()->pushComma(stack, pos);
}

void ThisPropertyAutomation::pushPropertySeparator(
                    AutomationStack& stack, misc::position const& pos)
{
    stack.reduced(util::mkptr(new This(this->pos)));
    stack.top()->pushColon(stack, pos);
}

void ThisPropertyAutomation::pushColon(AutomationStack& stack, misc::position const& pos)
{
    stack.reduced(util::mkptr(new This(this->pos)));
    stack.top()->pushColon(stack, pos);
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
