#include <report/errors.h>

#include "class.h"
#include "function.h"
#include "stmt-nodes.h"
#include "expr-nodes.h"
#include "scope-impl.h"

using namespace semantic;

static util::sptr<Statement const> superInitCall(
        misc::position const& pos, bool call_super
      , std::string const& class_name, util::ptrarr<Expression const> super_ctor_args)
{
    if (!call_super) {
        return util::sptr<Statement const>(nullptr);
    }
    return util::mkptr(new Arithmetics(pos, util::mkptr(
                    new SuperConstructorCall(pos, class_name, std::move(super_ctor_args)))));
}

Constructor::Constructor(misc::position const& ps, std::vector<std::string> params
                       , Block b, std::string const& class_name, bool si
                       , util::ptrarr<Expression const> super_ctor_args)
    : pos(ps)
    , param_names(std::move(params))
    , super_init(::superInitCall(ps, si, class_name, std::move(super_ctor_args)))
    , body(std::move(b))
{}

util::sptr<output::Constructor const> Constructor::compile(
                    util::sref<SymbolTable> st, bool has_base_class) const
{
    if (has_base_class && !this->superInit()) {
        error::contructorNotCallSuper(this->pos);
    } else if (!has_base_class && this->superInit()) {
        error::noSuperClass(this->pos);
    }

    SyncFunctionScope ctor_scope(pos, st, this->param_names, false);
    if (has_base_class) {
        ctor_scope.referenceThis(this->pos);
    }
    if (this->super_init.not_nul()) {
        this->super_init->compile(util::mkref(ctor_scope));
    }
    ctor_scope.allowSuper(has_base_class);
    this->body.compile(util::mkref(ctor_scope));
    return util::mkptr(new output::Constructor(this->param_names, ctor_scope.deliver()));
}

void Class::compile(util::sref<Scope> scope) const
{
    util::sptr<output::Constructor const> ct(nullptr);
    if (this->ctor_or_nul.not_nul()) {
        ct = this->ctor_or_nul->compile(scope->sym(), this->hasBaseClass());
    } else if (this->hasBaseClass()) {
        error::contructorNotCallSuper(this->pos);
    }

    util::sptr<output::Expression const> base_class(
            this->hasBaseClass()
          ? this->base_class->compile(scope)
          : util::sptr<output::Expression const>(nullptr));
    scope->sym()->defName(this->pos, this->name);

    util::sptr<output::Block const> class_body(new output::Block);
    std::map<std::string, misc::position> memfn_defs_pos;
    std::map<std::string, util::sptr<output::Expression const>> memfuncs;
    this->body.getFuncs().iter(
        [&](util::sptr<Function const> const& func, int)
        {
            if (memfn_defs_pos.find(func->name) != memfn_defs_pos.end()) {
                return error::duplicateMemFunc(memfn_defs_pos[func->name], func->pos, func->name);
            }
            memfn_defs_pos.insert(std::make_pair(func->name, func->pos));
            memfuncs.insert(std::make_pair(func->name, func->compileToLambda(scope->sym(), true)));
        });
    scope->addStmt(this->pos, util::mkptr(new output::Class(
            this->name, std::move(base_class), std::move(memfuncs), std::move(ct))));
}
