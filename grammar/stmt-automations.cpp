#include <report/errors.h>

#include "clauses.h"
#include "stmt-automations.h"
#include "expr-automations.h"
#include "stmt-nodes.h"

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
        { SUPER, { createPipeline, true } },
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
    _actions[RETURN] = [this](AutomationStack& stack, TypedToken const& token)
                       {
                           stack.replace(util::mkptr(
                                new ReturnAutomation(token.pos, this->_clause)));
                       };
    _actions[THROW] = [this](AutomationStack& stack, TypedToken const& token)
                      {
                          stack.replace(util::mkptr(new ThrowAutomation(token.pos, this->_clause)));
                      };
    _actions[FOR] = [this](AutomationStack& stack, TypedToken const& token)
                    {
                        stack.replace(util::mkptr(new ForAutomation(token.pos)));
                    };
    _actions[BREAK] = [this](AutomationStack& stack, TypedToken const& token)
                      {
                          this->_clause->acceptStmt(util::mkptr(new Break(token.pos)));
                          stack.replace(util::mkptr(new IgnoreAny(false)));
                      };
    _actions[CONTINUE] = [this](AutomationStack& stack, TypedToken const& token)
                         {
                             this->_clause->acceptStmt(util::mkptr(new Continue(token.pos)));
                             stack.replace(util::mkptr(new IgnoreAny(false)));
                         };
    _actions[EXTERN] = [this](AutomationStack& stack, TypedToken const& token)
                       {
                           stack.replace(util::mkptr(
                               new ExternAutomation(token.pos, this->_clause)));
                       };
    _actions[EXPORT] = [this](AutomationStack& stack, TypedToken const& token)
                       {
                           stack.replace(util::mkptr(
                               new ExportAutomation(token.pos, this->_clause)));
                       };
    _actions[ENUM] = [this](AutomationStack& stack, TypedToken const&)
                     {
                         stack.replace(util::mkptr(new EnumAutomation(this->_clause)));
                     };
    _actions[INCLUDE] = [this](AutomationStack& stack, TypedToken const& t)
                        {
                            stack.replace(util::mkptr(new IncludeAutomation(t.pos, this->_clause)));
                        };
}

void ExprStmtAutomation::pushFactor(AutomationStack& stack, FactorToken& factor)
{
    stack.push(util::mkptr(new PipelineAutomation));
    stack.top()->pushFactor(stack, factor);
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

void ForAutomation::pushFactor(AutomationStack& stack, FactorToken& factor)
{
    if (this->_before_ref) {
        this->_before_ref = false;
        this->_ref = factor.expr->reduceAsName();
    } else if (factor.image == "range") {
        stack.push(util::mkptr(new PipelineAutomation));
        this->_actions[COMMA] = [this](AutomationStack& stack, TypedToken const&)
                                {
                                    stack.push(util::mkptr(new PipelineAutomation));
                                };
    } else {
        factor.unexpected();
        stack.push(util::mkptr(new IgnoreAny(true)));
        this->_finished = true;
    }
}

void ForAutomation::accepted(AutomationStack&, util::sptr<Expression const> expr)
{
    this->_range_args.push_back(std::move(expr));
    this->_finished = true;
}

void ForAutomation::finish(ClauseStackWrapper& wrapper, AutomationStack& stack
                         , misc::position const&)
{
    for (auto const& e: this->_range_args) {
        if (e->empty()) {
            error::invalidEmptyExpr(e->pos);
        }
    }
    if (this->_range_args.size() > 3) {
        error::excessiveExprInForRange(this->_range_args[3]->pos);
    }

    util::sptr<Expression const> begin(new IntLiteral(this->_pos, "0"));
    util::sptr<Expression const> end(new EmptyExpr(this->_pos));
    util::sptr<Expression const> step(new IntLiteral(this->_pos, "1"));

    if (this->_range_args.size() == 1) {
        end = std::move(this->_range_args[0]);
    } else if (this->_range_args.size() == 2) {
        begin = std::move(this->_range_args[0]);
        end = std::move(this->_range_args[1]);
    } else if (this->_range_args.size() == 3) {
        begin = std::move(this->_range_args[0]);
        end = std::move(this->_range_args[1]);
        step = std::move(this->_range_args[2]);
    }

    wrapper.pushClause(util::mkptr(new ForClause(
        wrapper.last_indent, std::move(this->_ref), std::move(begin), std::move(end)
      , std::move(step), wrapper.lastClause())));
    stack.pop();
}

void FunctionAutomation::pushFactor(AutomationStack&, FactorToken& factor)
{
    if (!_before_open_paren) {
        return factor.unexpected();
    }
    _before_open_paren = false;
    _pos = factor.pos;
    _func_name = factor.expr->reduceAsName();
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

void ReturnAutomation::finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&)
{
    this->_clause->acceptStmt(util::mkptr(new Return(this->pos, std::move(this->_expr))));
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
    if (_expr->empty()) {
        error::invalidEmptyExpr(_expr->pos);
    }
    _clause->acceptStmt(util::mkptr(new Throw(_expr->pos, std::move(_expr))));
    stack.pop();
}

util::sptr<ClauseBase> TryAutomation::createClause(ClauseStackWrapper& wrapper)
{
    return util::mkptr(new TryClause(wrapper.last_indent, pos, wrapper.lastClause()));
}

void CatchAutomation::pushFactor(AutomationStack& stack, FactorToken& factor)
{
    stack.push(util::mkptr(new IgnoreAny(false)));
    this->_except_name = factor.expr->reduceAsName();
}

util::sptr<ClauseBase> CatchAutomation::createClause(ClauseStackWrapper& wrapper)
{
    return util::mkptr(new CatchClause(wrapper.last_indent, pos, wrapper.lastClause(),
                                       std::move(this->_except_name)));
}

void ClassAutomation::pushFactor(AutomationStack&, FactorToken& factor)
{
    if (_class_name.empty()) {
        _class_name = factor.expr->reduceAsName();
        _actions[COLON] = [this](AutomationStack& stack, TypedToken const&)
                          {
                              stack.push(util::mkptr(new ConditionalAutomation));
                              _actions[COLON] = AutomationBase::discardToken;
                          };
        return;
    }
    factor.unexpected();
}

void ClassAutomation::accepted(AutomationStack&, util::sptr<Expression const> expr)
{
    this->_base_class = std::move(expr);
}

bool ClassAutomation::finishOnBreak(bool) const
{
    return !this->_class_name.empty();
}

void ClassAutomation::finish(
                ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&)
{
    wrapper.pushClause(util::mkptr(new ClassClause(
            wrapper.last_indent, this->_pos, this->_class_name
          , std::move(this->_base_class), wrapper.lastClause())));
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
                                     stack.push(util::mkptr(new ExprListAutomation));
                                     this->_list_accepted = CtorAutomation::_acceptParams;
                                     this->_actions[OPEN_PAREN] = AutomationBase::discardToken;
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

namespace {

    struct IdentListAutomation
        : AutomationBase
    {
        explicit IdentListAutomation(TokenType seperator)
            : _need_sep(false)
        {
            this->_actions[seperator] = [this](AutomationStack& s, TypedToken const& t)
                                        {
                                            this->pushSep(s, t);
                                        };
        }

        void pushSep(AutomationStack& s, TypedToken const& t)
        {
            if (!this->_need_sep) {
                AutomationBase::discardToken(s, t);
            }
            this->_need_sep = false;
        }

        void pushFactor(AutomationStack&, FactorToken& factor)
        {
            if (this->_need_sep) {
                return factor.unexpected();
            }
            this->_list.push_back(std::move(factor.expr));
            this->_need_sep = true;
        }

        void accepted(AutomationStack&, util::sptr<Expression const>) {}

        bool finishOnBreak(bool) const
        {
            return this->_need_sep && !this->_list.empty();
        }

        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&)
        {
            stack.reduced(std::move(this->_list));
        }
    protected:
        std::vector<util::sptr<Expression const>> _list;
        bool _need_sep;
    };

    struct ExportPointAutomation
        : IdentListAutomation
    {
        ExportPointAutomation()
            : IdentListAutomation(OPERATOR)
        {
            this->_actions[OPERATOR] = [this](AutomationStack& s, TypedToken const& t)
                                       {
                                           if (t.image != ".") {
                                               return AutomationBase::discardToken(s, t);
                                           }
                                           this->pushSep(s, t);
                                       };
            this->_actions[COLON] = [this](AutomationStack& stack, TypedToken const& t)
                                    {
                                        if (!this->_need_sep) {
                                            AutomationBase::discardToken(stack, t);
                                        }
                                        stack.reduced(std::move(this->_list));
                                    };
        }
    };

}

void ExternAutomation::activated(AutomationStack& stack)
{
    stack.push(util::mkptr(new IdentListAutomation(COMMA)));
}

void ExternAutomation::accepted(AutomationStack&, std::vector<util::sptr<Expression const>> list)
{
    for (auto const& e: list) {
        this->_externs.push_back(e->reduceAsName());
    }
}

void ExternAutomation::finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&)
{
    this->_clause->acceptStmt(util::mkptr(new Extern(this->_pos, std::move(this->_externs))));
    stack.pop();
}

void ExportAutomation::activated(AutomationStack& stack)
{
    stack.push(util::mkptr(new ExportPointAutomation));
}

void ExportAutomation::accepted(AutomationStack& stack
                              , std::vector<util::sptr<Expression const>> list)
{
    for (auto const& e: list) {
        this->_export_point.push_back(e->reduceAsName());
    }
    stack.push(util::mkptr(new PipelineAutomation));
}

void ExportAutomation::accepted(AutomationStack&, util::sptr<Expression const> e)
{
    this->_value = std::move(e);
}

void ExportAutomation::finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&)
{
    this->_clause->acceptStmt(util::mkptr(
            new Export(this->_pos, std::move(this->_export_point), std::move(this->_value))));
    stack.pop();
}

EnumAutomation::EnumAutomation(util::sref<ClauseBase> clause)
    : _current_value(0)
    , _clause(clause)
    , _after_name(false)
{
    this->_actions[COMMA] = [this](AutomationStack&, TypedToken const& t)
    {
        if (!this->_after_name) {
            return t.unexpected();
        }
        this->_after_name = false;
    };
}

void EnumAutomation::pushFactor(AutomationStack&, FactorToken& factor)
{
    if (this->_after_name) {
        return factor.unexpected();
    }
    this->_defs.push_back(util::mkptr(new NameDef(
            factor.pos, factor.expr->reduceAsName(), util::mkptr(
                    new IntLiteral(factor.pos, this->_current_value)))));
    ++this->_current_value;
    this->_after_name = true;
}

void EnumAutomation::finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&)
{
    for (auto& s: this->_defs) {
        this->_clause->acceptStmt(std::move(s));
    }
    stack.pop();
}

IncludeAutomation::IncludeAutomation(misc::position const& pos, util::sref<ClauseBase> clause)
    : _pos(pos)
    , _next_factor([this](FactorToken& f) { this->_fill_path(f); })
    , _clause(clause)
{}

void IncludeAutomation::pushFactor(AutomationStack&, FactorToken& factor)
{
    if (bool(this->_next_factor)) {
        return this->_next_factor(factor);
    }
    factor.unexpected();
}

void IncludeAutomation::finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&)
{
    this->_clause->acceptStmt(util::mkptr(new IncludeFile(
            std::move(this->_pos), std::move(this->_file_path), std::move(this->_alias))));
    stack.pop();
}

void IncludeAutomation::_fill_path(FactorToken& factor)
{
    this->_file_path = factor.expr->reduceAsProperty();
    this->_next_factor = [this](FactorToken& f) { this->_as(f); };
}

void IncludeAutomation::_as(FactorToken& factor)
{
    if (factor.expr->reduceAsName() != "as") {
        factor.unexpected();
    }
    this->_next_factor = [this](FactorToken& f) { this->_fill_alias(f); };
}

void IncludeAutomation::_fill_alias(FactorToken& factor)
{
    this->_alias = factor.expr->reduceAsName();
    this->_next_factor = nullptr;
}

IgnoreAny::IgnoreAny(bool reported)
    : _reported(reported)
{
    std::fill(_actions, _actions + TOKEN_TYPE_COUNT
            , [this](AutomationStack&, TypedToken const& token)
              {
                  this->_report(token);
              });
}

void IgnoreAny::pushFactor(AutomationStack&, FactorToken& factor)
{
    this->_report(factor);
}

void IgnoreAny::_report(Token const& t)
{
    if (!this->_reported) {
        t.unexpected();
        this->_reported = true;
    }
}

void IgnoreAny::finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&)
{
    stack.pop();
}
