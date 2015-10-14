#include <algorithm>

#include <semantic/function.h>
#include <report/errors.h>

#include "clauses.h"
#include "stmt-automations.h"
#include "expr-automations.h"
#include "stmt-nodes.h"
#include "function.h"
#include "class.h"

using namespace grammar;

ExprStmtAutomation::ExprStmtAutomation(util::sref<ClauseBase> clause)
    : _clause(clause)
    , _before_colon(true)
{
    static AutomationCreator const createPipeline(
            [](TypedToken const&)
            {
                return util::mkptr(new PipelineAutomation);
            });
    _setShifts({
        { THIS, { createPipeline, true } },
        { OPEN_PAREN, { createPipeline, true } },
        { OPEN_BRACKET, { createPipeline, true } },
        { OPEN_BRACE, { createPipeline, true } },
        { OPERATOR, { createPipeline, true } },
    });

    _actions[IF] = [](AutomationStack& stack, TypedToken const&)
                   {
                       stack.replace(util::mkptr(new IfAutomation));
                   };
    _actions[ELSE] = [](AutomationStack& stack, TypedToken const& token)
                     {
                         stack.replace(util::mkptr(new ElseAutomation(token.pos)));
                     };
    _actions[IFNOT] = [](AutomationStack& stack, TypedToken const&)
                      {
                          stack.replace(util::mkptr(new IfnotAutomation));
                      };
    _actions[TRY] = [](AutomationStack& stack, TypedToken const& token)
                    {
                        stack.replace(util::mkptr(new TryAutomation(token.pos)));
                    };
    _actions[FUNC] = [](AutomationStack& stack, TypedToken const&)
                     {
                         stack.replace(util::mkptr(new FunctionAutomation));
                     };
    _actions[COLON] = [&](AutomationStack& stack, TypedToken const& token)
                      {
                          _pushColon(stack, token.pos);
                      };
    _actions[CATCH] = [](AutomationStack& stack, TypedToken const& token)
                      {
                          stack.replace(util::mkptr(new CatchAutomation(token.pos)));
                      };
    _actions[CLASS] = [](AutomationStack& stack, TypedToken const& token)
                      {
                          stack.replace(util::mkptr(new ClassAutomation(token.pos)));
                      };
    _actions[CONSTRUCTOR] = [](AutomationStack& stack, TypedToken const& token)
                            {
                                stack.replace(util::mkptr(new CtorAutomation(token.pos)));
                            };
}

void ExprStmtAutomation::pushFactor(AutomationStack& stack
                                  , util::sptr<Expression const> factor
                                  , std::string const& image)
{
    stack.push(util::mkptr(new PipelineAutomation));
    stack.top()->pushFactor(stack, std::move(factor), image);
}

void ExprStmtAutomation::accepted(AutomationStack&, util::sptr<Expression const> expr)
{
    _exprs.push_back(std::move(expr));
}

bool ExprStmtAutomation::finishOnBreak(bool sub_empty) const
{
    return _before_colon || !sub_empty || _exprs.size() == 2;
}

void ExprStmtAutomation::finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&)
{
    if (!_exprs.empty()) {
        _clause->acceptStmt(_reduceAsStmt());
    }
    stack.pop();
}

void ExprStmtAutomation::_pushColon(AutomationStack& stack, misc::position const& pos)
{
    _before_colon = false;
    if (_exprs.size() == 1) {
        stack.push(util::mkptr(new PipelineAutomation));
        return;
    }
    error::unexpectedToken(pos, ":");
}

util::sptr<Statement> ExprStmtAutomation::_reduceAsStmt()
{
    misc::position pos(_exprs[0]->pos);
    if (_exprs.size() == 1) {
        return util::mkptr(new Arithmetics(pos, std::move(_exprs[0])));
    }
    if (_exprs[0]->isName()) {
        return util::mkptr(new NameDef(pos, _exprs[0]->reduceAsName(), std::move(_exprs[1])));
    }
    return util::mkptr(new AttrSet(pos, std::move(_exprs[0]), std::move(_exprs[1])));
}

void StandaloneKeyWordAutomation::finish(
                ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&)
{
    wrapper.pushClause(createClause(wrapper));
    stack.pop();
}

void IfAutomation::activated(AutomationStack& stack)
{
    stack.push(util::mkptr(new PipelineAutomation));
}

void IfAutomation::accepted(AutomationStack&, util::sptr<Expression const> expr)
{
    _pred_cache = std::move(expr);
}

bool IfAutomation::finishOnBreak(bool sub_empty) const
{
    return !sub_empty;
}

void IfAutomation::finish(
                ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&)
{
    if (_pred_cache->empty()) {
        error::invalidEmptyExpr(_pred_cache->pos);
    }
    wrapper.pushClause(util::mkptr(new IfClause(
                        wrapper.last_indent, std::move(_pred_cache), wrapper.lastClause())));
    stack.pop();
}

util::sptr<ClauseBase> ElseAutomation::createClause(ClauseStackWrapper& wrapper)
{
    return util::mkptr(new ElseClause(wrapper.last_indent, pos, wrapper.lastClause()));
}

void IfnotAutomation::finish(
                ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&)
{
    if (_pred_cache->empty()) {
        error::invalidEmptyExpr(_pred_cache->pos);
    }
    wrapper.pushClause(util::mkptr(new IfnotClause(
                        wrapper.last_indent, std::move(_pred_cache), wrapper.lastClause())));
    stack.pop();
}

void FunctionAutomation::pushFactor(
                AutomationStack&, util::sptr<Expression const> factor, std::string const& image)
{
    if (!_before_open_paren) {
        error::unexpectedToken(factor->pos, image);
        return;
    }
    _before_open_paren = false;
    _pos = factor->pos;
    _func_name = factor->reduceAsName();
    _actions[OPEN_PAREN] = [=](AutomationStack& stack, TypedToken const&)
                           {
                               _actions[OPEN_PAREN] = AutomationBase::discardToken;
                               stack.push(util::mkptr(new ExprListAutomation));
                           };
}

void FunctionAutomation::accepted(AutomationStack&, std::vector<util::sptr<Expression const>> list)
{
    ParamReducingEnv env;
    int index(0);
    std::for_each(list.begin()
                , list.end()
                , [&](util::sptr<Expression const> const& param)
                  {
                      param->reduceAsParam(env, index++);
                  });
    _params = env.params();
    _async_param_index = env.asyncIndex();
    _finished = true;
}

bool FunctionAutomation::finishOnBreak(bool) const
{
    return _finished;
}

void FunctionAutomation::finish(
                ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&)
{
    wrapper.pushClause(util::mkptr(new FunctionClause(wrapper.last_indent
                                                    , _pos
                                                    , _func_name
                                                    , _params
                                                    , _async_param_index
                                                    , wrapper.lastClause())));
    stack.pop();
}

void ExprReceiver::activated(AutomationStack& stack)
{
    stack.push(util::mkptr(new PipelineAutomation));
}

void ExprReceiver::accepted(AutomationStack&, util::sptr<Expression const> expr)
{
    _expr = std::move(expr);
}

void ExprReceiver::finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&)
{
    if (_expr->empty()) {
        error::invalidEmptyExpr(_expr->pos);
    }
    _clause->acceptExpr(std::move(_expr));
    stack.pop();
}

void ReturnAutomation::finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&)
{
    if (_expr->empty()) {
        _clause->acceptStmt(util::mkptr(new ReturnNothing(_expr->pos)));
    } else {
        _clause->acceptStmt(util::mkptr(new Return(_expr->pos, std::move(_expr))));
    }
    stack.pop();
}

void ExportStmtAutomation::finish(
                        ClauseStackWrapper&, AutomationStack& stack, misc::position const&)
{
    if (_expr->empty()) {
        error::invalidEmptyExpr(_expr->pos);
    }
    _clause->acceptStmt(util::mkptr(new Export(_expr->pos, export_point, std::move(_expr))));
    stack.pop();
}

void ThrowAutomation::finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&)
{
    _clause->acceptStmt(util::mkptr(new Throw(_expr->pos, std::move(_expr))));
    stack.pop();
}

util::sptr<ClauseBase> TryAutomation::createClause(ClauseStackWrapper& wrapper)
{
    return util::mkptr(new TryClause(wrapper.last_indent, pos, wrapper.lastClause()));
}

util::sptr<ClauseBase> CatchAutomation::createClause(ClauseStackWrapper& wrapper)
{
    return util::mkptr(new CatchClause(wrapper.last_indent, pos, wrapper.lastClause()));
}

void ClassAutomation::pushFactor(
                AutomationStack&, util::sptr<Expression const> factor, std::string const& image)
{
    if (_class_name.empty()) {
        _class_name = factor->reduceAsName();
        _actions[COLON] = [this](AutomationStack&, TypedToken const&)
                          {
                              _actions[COLON] = AutomationBase::discardToken;
                              _before_colon = false;
                          };
        return;
    }
    if (_base_class_name.empty()) {
        _base_class_name = factor->reduceAsName();
        return;
    }
    error::unexpectedToken(factor->pos, image);
}

bool ClassAutomation::finishOnBreak(bool) const
{
    return !_class_name.empty() && (_before_colon || !_base_class_name.empty());
}

void ClassAutomation::finish(
                ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&)
{
    wrapper.pushClause(util::mkptr(new ClassClause(
            wrapper.last_indent, _pos, _class_name, _base_class_name, wrapper.lastClause())));
    stack.pop();
}

CtorAutomation::CtorAutomation(misc::position const& pos)
    : _pos(pos)
    , _list_accepted(nullptr)
    , _finished(false)
    , _super_init(false)
{
    this->_actions[OPEN_PAREN] = [this](AutomationStack& stack, TypedToken const&)
                                 {
                                     this->_actions[OPEN_PAREN] = AutomationBase::discardToken;
                                     stack.push(util::mkptr(new ExprListAutomation));
                                     this->_list_accepted = CtorAutomation::_acceptParams;
                                 };
}

void CtorAutomation::accepted(AutomationStack&, std::vector<util::sptr<Expression const>> list)
{
    this->_list_accepted(this, std::move(list));
}

bool CtorAutomation::finishOnBreak(bool) const
{
    return this->_finished;
}

void CtorAutomation::finish(
            ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&)
{
    wrapper.pushClause(util::mkptr(new CtorClause(
                wrapper.last_indent, this->_pos, std::move(this->_params)
              , this->_super_init, std::move(this->_super_ctor_args), wrapper.lastClause())));
    stack.pop();
}

void CtorAutomation::_acceptParams(
        CtorAutomation* self, std::vector<util::sptr<Expression const>> list)
{
    for (auto const& e: list) {
        self->_params.push_back(e->reduceAsName());
    }
    self->_finished = true;
    self->_actions[SUPER] =
        [=](AutomationStack&, TypedToken const&)
        {
            self->_finished = false;
            self->_actions[OPEN_PAREN] =
                [=](AutomationStack& stack, TypedToken const&)
                {
                    stack.push(util::mkptr(new ExprListAutomation));
                    self->_list_accepted = CtorAutomation::_acceptSuperArgs;
                    self->_actions[OPEN_PAREN] = AutomationBase::discardToken;
                };
        };
}

void CtorAutomation::_acceptSuperArgs(
        CtorAutomation* self, std::vector<util::sptr<Expression const>> list)
{
    self->_super_ctor_args = std::move(list);
    self->_super_init = true;
    self->_finished = true;
}
