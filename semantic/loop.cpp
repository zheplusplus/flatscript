#include <output/expr-nodes.h>
#include <output/list-pipe.h>
#include <report/errors.h>

#include "loop.h"
#include "scope-impl.h"
#include "symbol-table.h"

using namespace semantic;

bool RangeIteration::isAsync() const
{
    return this->begin->isAsync() || this->end->isAsync() || this->loop->isAsync();
}

static void compileRangeSync(
        misc::position const& pos, util::sref<Scope> scope, util::uid loop_id
      , std::string const& ref, util::sptr<output::Expression const> begin
      , util::sptr<output::Expression const> end, double step, util::sref<Statement const> loop)
{
    util::sptr<Scope> loop_scope(scope->makeSyncRangeScope(loop_id));
    loop_scope->sym()->defName(pos, ref);
    loop->compile(*loop_scope);
    output::Method ext_return(output::method::place());
    bool has_ret = false;
    if (loop_scope->firstReturn().not_nul()) {
        ext_return = scope->retMethod(misc::position(loop_scope->firstReturn()->line));
        has_ret = true;
    }
    scope->addStmt(pos, util::mkptr(new output::SyncRangeIteration(
            util::mkptr(new output::Reference(pos, ref))
          , std::move(begin), std::move(end), step, loop_scope->deliver()
          , std::move(ext_return), loop_id, has_ret, loop_scope->hasBreak())));
}

static void compileRangeAsync(
        misc::position const& pos, util::sref<Scope> scope, util::uid loop_id
      , std::string const& ref, util::sptr<output::Expression const> begin
      , util::sptr<output::Expression const> end, double step, util::sref<Statement const> loop)
{
    util::sref<output::Block> current_flow(scope->block());
    util::sptr<output::Block> succession_flow(new output::Block);
    scope->setAsyncSpace(pos, std::vector<std::string>(), *succession_flow);

    util::sptr<Scope> loop_scope(scope->makeAsyncRangeScope(loop_id));
    loop_scope->sym()->defName(pos, ref);
    loop->compile(*loop_scope);
    current_flow->addStmt(util::mkptr(new output::AsyncRangeIteration(
            util::mkptr(new output::Reference(pos, ref)), std::move(begin), std::move(end), step
          , loop_scope->deliver(), std::move(succession_flow), loop_id)));
}

void RangeIteration::compile(util::sref<Scope> scope) const
{
    if (!this->step->isLiteral(scope->sym())) {
        return error::rangeIterStepNonLiteral(this->step->pos);
    }
    double step_v = 0;
    if (this->step->literalType(scope->sym()) == "int") {
        step_v = this->step->intValue(scope->sym()).get_d();
    } else if (this->step->literalType(scope->sym()) == "float") {
        step_v = this->step->floatValue(scope->sym()).get_d();
    }
    if (step_v == 0) {
        return error::invalidRangeIterStep(this->step->pos);
    }
    auto compl_begin(this->begin->compile(scope));
    auto compl_end(this->end->compile(scope));

    if (this->loop->isAsync()) {
        return ::compileRangeAsync(
            this->pos, scope, this->loop_id, this->reference
          , std::move(compl_begin), std::move(compl_end), step_v, *this->loop);
    }
    ::compileRangeSync(
            this->pos, scope, this->loop_id, this->reference
          , std::move(compl_begin), std::move(compl_end), step_v, *this->loop);
}
